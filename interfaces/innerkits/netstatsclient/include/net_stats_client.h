/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_CLIENT_H
#define NET_STATS_CLIENT_H

#include <string>

#include "parcel.h"
#include "singleton.h"

#include "i_net_stats_service.h"
#include "net_stats_constants.h"
#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsClient {
    DECLARE_DELAYED_SINGLETON(NetStatsClient)

public:
    /**
     * Register network card traffic monitoring
     *
     * @param callback callback function
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback);

    /**
     * Unregister network card traffic monitoring
     *
     * @param callback callback function
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback);

    /**
     * Get the received traffic of the network card
     *
     * @param stats Traffic (bytes)
     * @param interfaceName network card name
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName);

    /**
     * Get the send traffic of the network card
     *
     * @param stats Traffic (bytes)
     * @param interfaceName network card name
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName);

    /**
     * Get received traffic from the cell
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetCellularRxBytes(uint64_t &stats);

    /**
     * Get send traffic from the cell
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetCellularTxBytes(uint64_t &stats);

    /**
     * Get all received traffic
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllRxBytes(uint64_t &stats);

    /**
     * Get all send traffic
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllTxBytes(uint64_t &stats);

    /**
     * Get the received traffic for the specified UID of application
     *
     * @param stats Traffic (bytes)
     * @param uid The specified UID of application.
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidRxBytes(uint64_t &stats, uint32_t uid);

    /**
     * Get the send traffic for the specified UID of application
     *
     * @param stats Traffic (bytes)
     * @param uid The specified UID of application.
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidTxBytes(uint64_t &stats, uint32_t uid);

    /**
     * Get traffic details for all network cards
     *
     * @param infos all network cards informations 
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllStatsInfo(std::vector<NetStatsInfo> &infos);

    /**
     * Get the historical traffic details of the specified network card
     *
     * @param iface network cards name
     * @param start start time
     * @param end end time
     * @param statsInfo traffic information
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end, NetStatsInfo &statsInfo);

    /**
     * Get the historical traffic details from UID of application.
     *
     * @param iface network cards name
     * @param uid The specified UID of application.
     * @param start start time
     * @param end end time
     * @param statsInfo traffic information
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                              NetStatsInfo &statsInfo);

    /**
     * Update the traffic of the specified network card
     *
     * @param iface network cards name
     * @param start start time
     * @param end end time
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end, const NetStatsInfo &stats);

    /**
     * Update network card traffic data
     *
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateStatsData();

    /**
     * Clear network card traffic
     *
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t ResetFactory();

private:
    class NetStatsDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetStatsDeathRecipient(NetStatsClient &client) : client_(client) {}
        ~NetStatsDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetStatsClient &client_;
    };

private:
    sptr<INetStatsService> GetProxy();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    std::mutex mutex_;
    sptr<INetStatsService> netStatsService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CLIENT_H
