/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "set_power_save_allow_list_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_policy_constants.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
SetPowerSaveAllowListContext::SetPowerSaveAllowListContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager)
{
}

void SetPowerSaveAllowListContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("Check params failed");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    uid_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    if (uid_ < 0) {
        NETMANAGER_BASE_LOGE("Check params failed");
        SetErrorCode(POLICY_ERR_INVALID_UID);
        return;
    }
    isAllow_ = NapiUtils::GetBooleanValue(GetEnv(), params[ARG_INDEX_1]);
    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_2]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool SetPowerSaveAllowListContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number
               && NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_boolean;
    }

    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_boolean &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_function;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
