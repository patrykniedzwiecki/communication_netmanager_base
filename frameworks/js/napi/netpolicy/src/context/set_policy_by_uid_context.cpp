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

#include "set_policy_by_uid_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
SetPolicyByUidContext::SetPolicyByUidContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void SetPolicyByUidContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        return;
    }

    uid_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_0]);
    policy_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_1]);

    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_2]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool SetPolicyByUidContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number;
    }

    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_function;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS