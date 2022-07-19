/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dns_config_client.h"
#include "securec.h"

#include "netsys_client.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t Min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

static bool NonBlockConnect(int sock, struct sockaddr *addr, socklen_t addrLen)
{
    int ret = connect(sock, addr, addrLen);
    if (ret >= 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        return false;
    }

    fd_set set = {0};
    FD_ZERO(&set);
    FD_SET(sock, &set);
    struct timeval timeout = {
        .tv_sec = DEFAULT_CONNECT_TIMEOUT,
        .tv_usec = 0,
    };

    ret = select(sock + 1, NULL, &set, NULL, &timeout);
    if (ret < 0) {
        DNS_CONFIG_PRINT("select error: %s", strerror(errno));
        return false;
    } else if (ret == 0) {
        DNS_CONFIG_PRINT("timeout!");
        return false;
    }

    int err = 0;
    socklen_t optLen = sizeof(err);
    ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)(&err), &optLen);
    if (ret < 0) {
        return false;
    }
    if (err != 0) {
        return false;
    }
    return true;
}

static int CreateConnectionToNetSys(void)
{
    int32_t sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("socket failed %d", errno);
        return -errno;
    }
    if (!MakeNonBlock(sockFd)) {
        DNS_CONFIG_PRINT("MakeNonBlock");
        close(sockFd);
        return -errno;
    }

    struct sockaddr_un address = {0};
    address.sun_family = AF_UNIX;

    if (strcpy_s(address.sun_path, sizeof(address.sun_path), DNS_SOCKET_PATH)) {
        DNS_CONFIG_PRINT("str copy failed ");
        close(sockFd);
        return -errno;
    }

    if (!NonBlockConnect(sockFd, (struct sockaddr *)&address, sizeof(address))) {
        close(sockFd);
        return -errno;
    }

    return sockFd;
}

static bool MakeKey(char *hostName, char *serv, struct addrinfo *hints, char key[static MAX_KEY_LENGTH])
{
    if (serv && hints) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %s %u %u %u %u", hostName, serv, hints->ai_family, hints->ai_flags,
                         hints->ai_protocol, hints->ai_socktype) > 0;
    }

    if (hints) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %u %u %u %u", hostName, hints->ai_family, hints->ai_flags,
                         hints->ai_protocol, hints->ai_socktype) > 0;
    }

    if (serv) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %s", hostName, serv) > 0;
    }

    return sprintf_s(key, MAX_KEY_LENGTH, "%s", hostName) > 0;
}

static void NetsysGetDefaultConfig(struct ResolvConfig *config)
{
    if (memset_s(config, sizeof(struct ResolvConfig), 0, sizeof(struct ResolvConfig)) != EOK) {
        DNS_CONFIG_PRINT("NetsysGetDefaultConfig memset_s failed");
        return;
    }
    config->timeoutMs = DEFAULT_TIMEOUT;
    config->retryCount = DEFAULT_RETRY;
    if (strcpy_s(config->nameservers[0], sizeof(config->nameservers[0]), DEFAULT_SERVER) <= 0) {
        DNS_CONFIG_PRINT("NetsysGetDefaultConfig strcpy_s failed");
    }
}

static int32_t NetSysGetResolvConfInternal(int sockFd, uint16_t netId, struct ResolvConfig *config) //
{
    struct RequestInfo info = {
        .command = GET_CONFIG,
        .netId = netId,
    };

    DNS_CONFIG_PRINT("NetSysGetResolvConfInternal begin netid: %d", info.netId);
    if (!PollSendData(sockFd, (const char *)(&info), sizeof(info))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (!PollRecvData(sockFd, (char *)(config), sizeof(struct ResolvConfig))) {
        DNS_CONFIG_PRINT("receive failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (config->error < 0) {
        DNS_CONFIG_PRINT("get Config error: %d", config->error);
        close(sockFd);
        return config->error;
    }

    DNS_CONFIG_PRINT("NetSysGetResolvConfInternal end netid: %d", info.netId);
    close(sockFd);
    return 0;
}

int32_t NetSysGetResolvConf(uint16_t netId, struct ResolvConfig *config)
{
    DNS_CONFIG_PRINT("NetSysGetResolvConf begin");

    if (config == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvConf CreateConnectionToNetSys connect to netsys err: %d", errno);
        NetsysGetDefaultConfig(config);
        return 0;
    }

    int32_t err = NetSysGetResolvConfInternal(sockFd, netId, config);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvConf NetSysGetResolvConfInternal err: %d", errno);
        NetsysGetDefaultConfig(config);
        return 0;
    }

    DNS_CONFIG_PRINT("GetResolvConfFromNetsys end");
    if (strlen(config->nameservers[0]) == 0) {
        NetsysGetDefaultConfig(config);
        return 0;
    }
    return 0;
}

static int32_t NetSysGetResolvCacheInternal(int sockFd, uint16_t netId, struct ParamWrapper param,
                                            struct AddrInfo addrInfo[static MAX_RESULTS], uint32_t *num)
{
    struct RequestInfo info = {
        .command = GET_CACHE,
        .netId = netId,
    };

    char key[MAX_KEY_LENGTH] = {0};
    if (!MakeKey(param.host, param.serv, param.hint, key)) {
        close(sockFd);
        return -1;
    }

    DNS_CONFIG_PRINT("NetSysGetResolvCacheInternal begin netid: %d", info.netId);
    if (!PollSendData(sockFd, (const char *)(&info), sizeof(info))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    uint32_t nameLen = strlen(key) + 1;
    if (!PollSendData(sockFd, (const char *)&nameLen, sizeof(nameLen))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (!PollSendData(sockFd, key, nameLen)) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (!PollRecvData(sockFd, (char *)num, sizeof(uint32_t))) {
        DNS_CONFIG_PRINT("read failed %d", errno);
        close(sockFd);
        return -errno;
    }

    *num = Min(*num, MAX_RESULTS);
    if (*num == 0) {
        close(sockFd);
        return 0;
    }

    if (!PollRecvData(sockFd, (char *)addrInfo, sizeof(struct AddrInfo) * (*num))) {
        DNS_CONFIG_PRINT("read failed %d", errno);
        close(sockFd);
        return -errno;
    }

    DNS_CONFIG_PRINT("NetSysGetResolvCacheInternal end netid: %d", info.netId);
    close(sockFd);
    return 0;
}

int32_t NetSysGetResolvCache(uint16_t netId, struct ParamWrapper param, struct AddrInfo addrInfo[static MAX_RESULTS],
                             uint32_t *num)
{
    DNS_CONFIG_PRINT("NetSysGetResolvCache begin");

    char *hostName = param.host;
    if (hostName == NULL || strlen(hostName) == 0 || num == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvCache CreateConnectionToNetSys connect to netsys err: %d", errno);
        return sockFd;
    }

    int err = NetSysGetResolvCacheInternal(sockFd, netId, param, addrInfo, num);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvCache NetSysGetResolvCacheInternal err: %d", errno);
        return err;
    }

    DNS_CONFIG_PRINT("NetSysGetResolvCache end");
    return 0;
}

static int32_t FillAddrInfo(struct AddrInfo addrInfo[static MAX_RESULTS], struct addrinfo *res)
{
    if (memset_s(addrInfo, sizeof(struct AddrInfo) * MAX_RESULTS, 0, sizeof(struct AddrInfo) * MAX_RESULTS) < 0) {
        return -1;
    }

    int32_t resNum = 0;
    for (struct addrinfo *tmp = res; tmp != NULL; tmp = tmp->ai_next) {
        addrInfo[resNum].aiFlags = tmp->ai_flags;
        addrInfo[resNum].aiFamily = tmp->ai_family;
        addrInfo[resNum].aiSockType = tmp->ai_socktype;
        addrInfo[resNum].aiProtocol = tmp->ai_protocol;
        addrInfo[resNum].aiAddrLen = tmp->ai_addrlen;
        if (memcpy_s(&addrInfo[resNum].aiAddr, sizeof(addrInfo[resNum].aiAddr), tmp->ai_addr, tmp->ai_addrlen) < 0) {
            DNS_CONFIG_PRINT("memcpy_s failed");
            return -1;
        }
        if (strcpy_s(addrInfo[resNum].aiCanonName, sizeof(addrInfo[resNum].aiCanonName), tmp->ai_canonname) < 0) {
            DNS_CONFIG_PRINT("strcpy_s failed");
            return -1;
        }

        ++resNum;
        if (resNum >= MAX_RESULTS) {
            break;
        }
    }

    return resNum;
}

static int32_t NetSysSetResolvCacheInternal(int sockFd, uint16_t netId, struct ParamWrapper param, struct addrinfo *res)
{
    struct RequestInfo info = {
        .command = SET_CACHE,
        .netId = netId,
    };

    char key[MAX_KEY_LENGTH] = {0};
    if (!MakeKey(param.host, param.serv, param.hint, key)) {
        close(sockFd);
        return -1;
    }

    DNS_CONFIG_PRINT("NetSysSetResolvCacheInternal begin netid: %d", info.netId);
    if (!PollSendData(sockFd, (const char *)(&info), sizeof(info))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    uint32_t nameLen = strlen(key) + 1;
    if (!PollSendData(sockFd, (const char *)&nameLen, sizeof(nameLen))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (!PollSendData(sockFd, key, nameLen)) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    struct AddrInfo addrInfo[MAX_RESULTS] = {};
    uint32_t resNum = FillAddrInfo(addrInfo, res);
    if (resNum < 0) {
        close(sockFd);
        return -1;
    }

    if (!PollSendData(sockFd, (char *)&resNum, sizeof(resNum))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    if (resNum == 0) {
        close(sockFd);
        return 0;
    }

    if (!PollSendData(sockFd, (char *)addrInfo, sizeof(struct AddrInfo) * resNum)) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        close(sockFd);
        return -errno;
    }

    DNS_CONFIG_PRINT("NetSysSetResolvCacheInternal end netid: %d", info.netId);
    close(sockFd);
    return 0;
}

int32_t NetSysSetResolvCache(uint16_t netId, struct ParamWrapper param, struct addrinfo *res)
{
    DNS_CONFIG_PRINT("NetSysSetResolvCache begin");

    char *hostName = param.host;
    if (hostName == NULL || strlen(hostName) == 0 || res == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysSetResolvCache CreateConnectionToNetSys connect to netsys err: %d", errno);
        return sockFd;
    }

    int err = NetSysSetResolvCacheInternal(sockFd, netId, param, res);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysSetResolvCache NetSysSetResolvCacheInternal err: %d", errno);
        return err;
    }

    DNS_CONFIG_PRINT("NetSysSetResolvCache end");
    return 0;
}

#ifdef __cplusplus
}
#endif