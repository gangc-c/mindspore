/**
 * Copyright 2023 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_UT_MOCK_RUNTIME_EVENT_H_
#define MINDSPORE_UT_MOCK_RUNTIME_EVENT_H_

#include <gmock/gmock.h>
#include "runtime/event.h"
#include "mock/mock_helper.h"
#include "acl/acl_rt.h"

MOCK_H(rtEventReset, rtError_t, rtEvent_t, rtStream_t);
MOCK_H(aclrtResetEvent, aclError, aclrtEvent, aclrtStream);

#endif  // MINDSPORE_UT_MOCK_MOCK_HELPER_H_
