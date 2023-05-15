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

#include <gtest/gtest.h>
#include <iostream>

#include <memory>
#define private public
#include "net_detection_callback_stub.h"
#undef private

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class MockNetDetectionCallbackStubTest : public NetDetectionCallbackStub {
public:
    MockNetDetectionCallbackStubTest() = default;
    ~MockNetDetectionCallbackStubTest() override {}
    int32_t OnNetDetectionResultChanged(NetDetectionResultCode detectionResult, const std::string &urlRedirect) override
    {
        std::cout << std::endl;
        std::cout << "OnNetDetectionResultChanged" << std::endl;
        return 0;
    }
};

class NetDetectionCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetDetectionCallbackStub> instance_ =
        std::make_shared<MockNetDetectionCallbackStubTest>();
};

void NetDetectionCallbackStubTest::SetUpTestCase() {}

void NetDetectionCallbackStubTest::TearDownTestCase() {}

void NetDetectionCallbackStubTest::SetUp() {}

void NetDetectionCallbackStubTest::TearDown() {}

HWTEST_F(NetDetectionCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDetectionCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int32_t ret = instance_->OnRemoteRequest(0, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDetectionCallbackStubTest, OnNetDetectionResult001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t ret = instance_->OnNetDetectionResult(data, reply);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS