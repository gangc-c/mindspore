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

#ifndef MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_HCCL_HCOM_BARRIER_H_
#define MINDSPORE_CCSRC_BACKEND_KERNEL_COMPILER_HCCL_HCOM_BARRIER_H_

#include <memory>
#include <vector>
#include "plugin/device/ascend/kernel/hccl/hccl_kernel.h"

namespace mindspore {
namespace kernel {
class HcomBarrierKernel : public HcclKernel {
 public:
  HcomBarrierKernel() = default;
  ~HcomBarrierKernel() override = default;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs, void *stream_ptr) override;
  const std::vector<size_t> &GetOutputSizeList() const override;
};

MS_HCCL_REG_KERNEL(Barrier, HcomBarrierKernel);
}  // namespace kernel
}  // namespace mindspore

#endif
