/**
 * Copyright 2020-2022 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_CCSRC_PLUGIN_DEVICE_CPU_KERNEL_MAP_CACHE_IDX_CPU_KERNEL_H_
#define MINDSPORE_CCSRC_PLUGIN_DEVICE_CPU_KERNEL_MAP_CACHE_IDX_CPU_KERNEL_H_

#include <map>
#include <cmath>
#include <vector>
#include <memory>
#include <unordered_map>
#include <utility>
#include "plugin/device/cpu/kernel/cpu_kernel.h"
#include "plugin/factory/ms_factory.h"

namespace mindspore {
namespace kernel {
class MapCacheIdxCpuKernelMod : public NativeCpuKernelMod {
 public:
  MapCacheIdxCpuKernelMod() = default;
  ~MapCacheIdxCpuKernelMod() override = default;

  bool Init(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
            const std::vector<KernelTensorPtr> &outputs) override;

  int Resize(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
             const std::vector<KernelTensorPtr> &outputs, const std::map<uint32_t, tensor::TensorPtr> &) override;

  bool Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
              const std::vector<AddressPtr> &outputs) override {
    MS_EXCEPTION_IF_NULL(kernel_func_);
    return kernel_func_(this, inputs, workspace, outputs);
  }

 protected:
  std::vector<KernelAttr> GetOpSupport() override;
  void SyncOutputShape() override;

 private:
  template <typename T>
  bool LaunchKernel(const std::vector<kernel::AddressPtr> &inputs, const std::vector<kernel::AddressPtr> &,
                    const std::vector<kernel::AddressPtr> &outputs);

  using MapCacheIdxFunc =
    std::function<bool(MapCacheIdxCpuKernelMod *, const std::vector<kernel::AddressPtr> &,
                       const std::vector<kernel::AddressPtr> &, const std::vector<kernel::AddressPtr> &)>;
  static std::vector<std::pair<KernelAttr, MapCacheIdxFunc>> func_list_;
  MapCacheIdxFunc kernel_func_;

  size_t batch_size_{1};
  size_t hashmap_length_{1};
  std::vector<TypeId> dtypes_ = {};
  size_t miss_count_{0};
  size_t outputs_size_{0};
};
}  // namespace kernel
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_PLUGIN_DEVICE_CPU_KERNEL_MAP_CACHE_IDX_CPU_KERNEL_H_
