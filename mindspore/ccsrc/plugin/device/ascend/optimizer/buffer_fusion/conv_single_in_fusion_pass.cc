/**
 * Copyright 2020-2023 Huawei Technologies Co., Ltd
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
#include "plugin/device/ascend/optimizer/buffer_fusion/conv_single_in_fusion_pass.h"
#include <memory>
#include <string>
#include <vector>
#include "mindspore/core/ops/framework_ops.h"
#include "utils/hash_set.h"
#include "include/backend/anf_runtime_algorithm.h"
#include "include/common/utils/anfalgo.h"
#include "plugin/device/ascend/optimizer/fusion_id_allocator.h"
#include "plugin/device/ascend/hal/common/platform_info_util.h"

namespace mindspore {
namespace opt {
namespace {
constexpr auto kPatternConvolution = "Convolution";
}

void ConvSingleInFusionPass::MatchConvSingleInEltwise(const CNodePtr &cnode, const session::KernelGraph &kernel_graph,
                                                      FusedNodeRecord *candidate_fusion) {
  MS_EXCEPTION_IF_NULL(cnode);
  MS_EXCEPTION_IF_NULL(candidate_fusion);
  mindspore::HashSet<AnfNodePtr> record{cnode};
  auto eltwise_input = cnode->input(kIndex1);
  while (CheckSingleInEltWiseNode(kernel_graph, eltwise_input)) {
    (void)record.insert(eltwise_input);
    auto input_cnode = eltwise_input->cast<CNodePtr>();
    MS_EXCEPTION_IF_NULL(input_cnode);
    eltwise_input = input_cnode->input(kIndex1);
    if (record.size() == MAX_ELTWISE_NUM) {
      break;
    }
  }
  MS_EXCEPTION_IF_NULL(eltwise_input);
  if (!eltwise_input->isa<CNode>() || !AnfUtils::IsRealCNodeKernel(eltwise_input) ||
      fusion_id_allocator->HasFusionIdAttr(eltwise_input)) {
    return;
  }
  if (AnfAlgo::GetKernelType(eltwise_input) == KernelType::TBE_KERNEL &&
      AnfAlgo::GetFusionType(eltwise_input) == kPatternConvolution) {
    (void)record.insert(eltwise_input);
    candidate_fusion->push_back(record);
    SetRecordFusionId(record);
  }
}

void ConvSingleInFusionPass::MatchSingleFusionPattern(const session::KernelGraph &kernel_graph,
                                                      FusedNodeRecord *candidate_fusion) {
  MS_EXCEPTION_IF_NULL(candidate_fusion);
  MS_CHECK_CUBE_VECTOR_SPLIT();
  const auto &node_list = TopoSort(kernel_graph.get_return());
  for (auto &node : node_list) {
    if (!AnfUtils::IsRealCNodeKernel(node) || fusion_id_allocator->HasFusionIdAttr(node) ||
        common::AnfAlgo::CheckPrimitiveType(node, prim::kPrimReturn)) {
      continue;
    }
    auto cnode = node->cast<CNodePtr>();
    MS_EXCEPTION_IF_NULL(cnode);
    if (AnfAlgo::GetKernelType(cnode) == KernelType::TBE_KERNEL && AnfAlgo::GetFusionType(cnode) == kPatternElemWise &&
        cnode->inputs().size() == ELTWISE_INPUT_SIZE) {
      MatchConvSingleInEltwise(cnode, kernel_graph, candidate_fusion);
    }
  }
}
}  // namespace opt
}  // namespace mindspore
