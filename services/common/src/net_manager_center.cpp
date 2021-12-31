/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "net_manager_center.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetManagerCenter &NetManagerCenter::GetInstance()
{
    static NetManagerCenter gInstance;
    return gInstance;
}

int32_t NetManagerCenter::GetIfaceNameByType(uint32_t netType, const std::string &ident, std::string &ifaceName)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetIfaceNameByType(netType, ident, ifaceName);
}

int32_t NetManagerCenter::RegisterNetSupplier(uint32_t netType, const std::string &ident, uint64_t netCapabilities,
    uint32_t &supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RegisterNetSupplier(netType, ident, netCapabilities, supplierId);
}

int32_t NetManagerCenter::UnregisterNetSupplier(uint32_t supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UnregisterNetSupplier(supplierId);
}

int32_t NetManagerCenter::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetLinkInfo(supplierId, netLinkInfo);
}

int32_t NetManagerCenter::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
}

void NetManagerCenter::RegisterConnService(const sptr<NetConnBaseService> &service)
{
    connService_ = service;
}

int32_t NetManagerCenter::GetIfaceStatsDetail(const std::string &iface, uint32_t start, uint32_t end,
    NetStatsInfo &info)
{
    if (statsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return statsService_->GetIfaceStatsDetail(iface, start, end, info);
}

void NetManagerCenter::RegisterStatsService(const sptr<NetStatsBaseService> &service)
{
    statsService_ = service;
}
} // namespace NetManagerStandard
} // namespace OHOS