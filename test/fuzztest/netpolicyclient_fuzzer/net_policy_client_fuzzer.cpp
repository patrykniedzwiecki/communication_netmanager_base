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

#include <thread>
#include <securec.h>

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include <net_quota_policy.h>
#include "net_policy_constants.h"
#include "net_policy_client.h"
#include "net_mgr_log_wrapper.h"
#include "token_setproc.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const uint8_t *g_base_fuzz_data = nullptr;
size_t g_base_fuzz_size = 0;
size_t g_base_fuzz_pos;
constexpr size_t STR_LEN = 10;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
HapInfoParams testInfoParms = {
    .bundleName = "net_policy_client_fuzzer",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "test"
};

PermissionDef testPermDef = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .bundleName = "net_policy_client_fuzzer",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "Test net policy connectivity internal",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC
};

PermissionStateFull testState = {
    .grantFlags = {2},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .resDeviceID = {"local"}
};

HapPolicyParams testPolicyPrams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {testPermDef},
    .permStateList = {testState}
};
}

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_base_fuzz_data == nullptr || objectSize > g_base_fuzz_size - g_base_fuzz_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_base_fuzz_data + g_base_fuzz_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_base_fuzz_pos += objectSize;
    return object;
}

std::string GetStringFromData(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = GetData<char>();
    }
    std::string str(cstr);
    return str;
}

class AccessToken {
public:
    AccessToken()
    {
        currentID_ = GetSelfTokenID();
        AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParms, testPolicyPrams);
        accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
        SetSelfTokenID(accessID_);
    }
    ~AccessToken()
    {
        AccessTokenKit::DeleteToken(accessID_);
        SetSelfTokenID(currentID_);
    }
private:
    AccessTokenID currentID_ = 0;
    AccessTokenID accessID_ = 0;
};

class INetPolicyCallbackTest : public INetPolicyCallback {
public:
    INetPolicyCallbackTest() : INetPolicyCallback() {}
    virtual ~INetPolicyCallbackTest() {}
};

void SetPolicyByUidFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    uint32_t uid = GetData<uint32_t>();
    NetUidPolicy policy = NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND;
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetPolicyByUid(uid, policy);
}

void GetPolicyByUidFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    uint32_t uid = GetData<uint32_t>();
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetPolicyByUid(uid);
}

void GetUidsByPolicyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    AccessToken token;
    NetUidPolicy policy = NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND;
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetUidsByPolicy(policy);
}

void IsUidNetAccessFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    uint32_t uid = GetData<uint32_t>();
    bool metered = GetData<bool>();
    std::string ifaceName = GetStringFromData(STR_LEN);
    DelayedSingleton<NetPolicyClient>::GetInstance()->IsUidNetAccess(uid, metered);
    DelayedSingleton<NetPolicyClient>::GetInstance()->IsUidNetAccess(uid, ifaceName);
}

void SetBackgroundPolicyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    bool isBackgroundPolicyAllow = GetData<bool>();
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetBackgroundPolicy(isBackgroundPolicyAllow);
}

void SetFactoryPolicyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    std::string simId = GetStringFromData(STR_LEN);
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetFactoryPolicy(simId);
}

void SetSnoozePolicyFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    int8_t netType = GetData<int8_t>();
    std::string simId(reinterpret_cast<const char*>(data), size);
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetSnoozePolicy(netType, simId);
}

void GetBackgroundPolicyByUidFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    uint32_t uid = GetData<uint32_t>();
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetBackgroundPolicyByUid(uid);
}

void SetIdleTrustlistFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    g_base_fuzz_data = data;
    g_base_fuzz_size = size;
    g_base_fuzz_pos = 0;
    AccessToken token;
    uint32_t uid = GetData<uint32_t>();
    bool isTrustlist = *(reinterpret_cast<const bool*>(data));
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetIdleTrustlist(uid, isTrustlist);
}

void SetCellularPoliciesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    std::vector<NetPolicyCellularPolicy> cellularPolicies;
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetCellularPolicies(cellularPolicies);
}

void RegisterNetPolicyCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    AccessToken token;
    sptr<INetPolicyCallbackTest> callback = sptr<INetPolicyCallbackTest>();
    DelayedSingleton<NetPolicyClient>::GetInstance()->RegisterNetPolicyCallback(callback);
}

void UnregisterNetPolicyCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    AccessToken token;
    sptr<INetPolicyCallbackTest> callback = sptr<INetPolicyCallbackTest>();
    DelayedSingleton<NetPolicyClient>::GetInstance()->UnregisterNetPolicyCallback(callback);
}

void GetIdleTrustlistFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    AccessToken token;
    std::vector<uint32_t> uids;
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetIdleTrustlist(uids);
}

void GetNetQuotaPoliciesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    std::vector<NetPolicyQuotaPolicy> quotaPolicies;
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetNetQuotaPolicies(quotaPolicies);
}

void SetNetQuotaPoliciesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    std::vector<NetPolicyQuotaPolicy> quotaPolicies;
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetNetQuotaPolicies(quotaPolicies);
}

void GetCellularPoliciesFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }
    std::vector<NetPolicyCellularPolicy> cellularPolicies;
    DelayedSingleton<NetPolicyClient>::GetInstance()->GetCellularPolicies(cellularPolicies);
}
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::NetManagerStandard::SetPolicyByUidFuzzTest(data, size);
    OHOS::NetManagerStandard::GetPolicyByUidFuzzTest(data, size);
    OHOS::NetManagerStandard::GetUidsByPolicyFuzzTest(data, size);
    OHOS::NetManagerStandard::IsUidNetAccessFuzzTest(data, size);
    OHOS::NetManagerStandard::SetFactoryPolicyFuzzTest(data, size);
    OHOS::NetManagerStandard::SetSnoozePolicyFuzzTest(data, size);
    OHOS::NetManagerStandard::GetBackgroundPolicyByUidFuzzTest(data, size);
    OHOS::NetManagerStandard::SetIdleTrustlistFuzzTest(data, size);
    OHOS::NetManagerStandard::SetCellularPoliciesFuzzTest(data, size);
    OHOS::NetManagerStandard::RegisterNetPolicyCallbackFuzzTest(data, size);
    OHOS::NetManagerStandard::UnregisterNetPolicyCallbackFuzzTest(data, size);
    OHOS::NetManagerStandard::GetNetQuotaPoliciesFuzzTest(data, size);
    OHOS::NetManagerStandard::SetNetQuotaPoliciesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetCellularPoliciesFuzzTest(data, size);
    return 0;
}