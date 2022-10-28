/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETMANAGERBASE_GETDEFAULTNET_CONTEXT_H
#define COMMUNICATIONNETMANAGERBASE_GETDEFAULTNET_CONTEXT_H

#include <cstddef>
#include <list>

#include "napi/native_api.h"
#include "nocopyable.h"
#include "refbase.h"
#include "net_handle.h"
#include "base_context.h"
#include "event_manager.h"

namespace OHOS::NetManagerStandard {
class GetConnNapiContext final : public BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(GetConnNapiContext);

    GetConnNapiContext() = delete;

    explicit GetConnNapiContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);

    // for GetDefaultNet
    NetHandle netHandle_;

    // for HasDefaultNet
    bool hasDefaultNet_;

    // for GetAllNets
    std::list<sptr<NetHandle>> netHandleList_;

    // for IsDefaultNetMetered
    bool isMetered_;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};

using GetDefaultNetContext = GetConnNapiContext;
using HasDefaultNetContext = GetConnNapiContext;
using GetAllNetsContext = GetConnNapiContext;
using EnableAirplaneModeContext = GetConnNapiContext;
using DisableAirplaneModeContext = GetConnNapiContext;
using IsDefaultNetMeteredContext = GetConnNapiContext;
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_GETDEFAULTNET_CONTEXT_H */
