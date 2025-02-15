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

#ifndef MINDSPORE_CCSRC_BACKEND_OPTIMIZER_PASS_MAX_POOL_WITH_ARGMAX_V2_FUSION_H_
#define MINDSPORE_CCSRC_BACKEND_OPTIMIZER_PASS_MAX_POOL_WITH_ARGMAX_V2_FUSION_H_

#include "include/backend/optimizer/optimizer.h"

namespace mindspore {
namespace opt {
class MaxPoolWithArgmaxV2Fusion : public PatternProcessPass {
 public:
  explicit MaxPoolWithArgmaxV2Fusion(bool multigraph = true)
      : PatternProcessPass("max_pool_with_argmax_v2_fusion", multigraph) {}
  ~MaxPoolWithArgmaxV2Fusion() override = default;

  const BaseRef DefinePattern() const override;
  const AnfNodePtr Process(const FuncGraphPtr &graph, const AnfNodePtr &node, const EquivPtr &) const override;
};
}  // namespace opt
}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_BACKEND_OPTIMIZER_PASS_MAX_POOL_WITH_ARGMAX_V2_FUSION_H_
