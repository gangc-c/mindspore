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
#include <vector>
#include <memory>
#include "common/common_test.h"
#include "ops/nextafter.h"
#include "ir/dtype/type.h"
#include "abstract/dshape.h"
#include "utils/tensor_construct_utils.h"
#include "ir/primitive.h"
#include "abstract/abstract_value.h"
#include "ops/test_ops.h"
#include "include/backend/optimizer/helper.h"

namespace mindspore {
namespace ops {
class TestNextAfter : public TestOps, public testing::WithParamInterface<BroadcastOpParams> {};

TEST_P(TestNextAfter, dyn_shape) {
  const auto &param = GetParam();
  auto x = std::make_shared<abstract::AbstractTensor>(param.x_type, param.x_shape);
  auto y = std::make_shared<abstract::AbstractTensor>(param.y_type, param.y_shape);
  auto expect = std::make_shared<abstract::AbstractTensor>(param.out_type, param.out_shape);
  ASSERT_NE(x, nullptr);
  ASSERT_NE(y, nullptr);
  auto next_after_op = std::make_shared<NextAfter>();
  next_after_op->Init();
  auto prim = std::make_shared<Primitive>(kNameNextAfter);
  auto out_abstract = opt::CppInferShapeAndType(prim, {x, y});
  ASSERT_NE(out_abstract, nullptr);
  ASSERT_TRUE(*out_abstract == *expect);
}

INSTANTIATE_TEST_CASE_P(TestNextAfterGroup, TestNextAfter,
                        testing::Values(
                          BroadcastOpParams{{1, 3}, kFloat32, {2, 1}, kFloat32, {2, 3}, kFloat32},
                          BroadcastOpParams{{-1, 3}, kFloat32, {-1, 1}, kFloat32, {-1, 3}, kFloat32},
                          BroadcastOpParams{{-1, 3}, kFloat32, {-1, 1}, kFloat32, {-1, 3}, kFloat32},
                          BroadcastOpParams{{-1, 1, 3}, kFloat32, {1, -1, 3}, kFloat32, {-1, -1, 3}, kFloat32},
                          BroadcastOpParams{{-1, 2, 3}, kFloat32, {2, -1, 3}, kFloat32, {2, 2, 3}, kFloat32},
                          BroadcastOpParams{{-2}, kFloat32, {2, 3}, kFloat32, {-2}, kFloat32}));
}  // namespace ops
}  // namespace mindspore
