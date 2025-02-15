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

#include "plugin/device/gpu/kernel/arrays/coalesce_gpu_kernel.h"
#include <utility>
#include <map>
namespace mindspore {
namespace kernel {
namespace {
template <typename T>
std::unique_ptr<cukernel::GpuKernelHelperBase> CreateCoalesceKernelPtr(const std::string &kernel_name,
                                                                       const uint32_t &device_id) {
  return std::make_unique<cukernel::CoalesceHelperGpuKernel<T>>(kernel_name, device_id);
}
using CoalescePtrCreatorFunc =
  std::function<std::unique_ptr<cukernel::GpuKernelHelperBase>(const std::string &, const uint32_t &)>;

const std::vector<std::pair<KernelAttr, CoalescePtrCreatorFunc>> kernel_attr = {{KernelAttr()
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddInputAttr(kNumberTypeFloat32)
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeFloat32)
                                                                                   .AddOutputAttr(kNumberTypeInt64),
                                                                                 CreateCoalesceKernelPtr<float>},
                                                                                {KernelAttr()
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddInputAttr(kNumberTypeFloat16)
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeFloat16)
                                                                                   .AddOutputAttr(kNumberTypeInt64),
                                                                                 CreateCoalesceKernelPtr<half>},
                                                                                {KernelAttr()
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddInputAttr(kNumberTypeFloat64)
                                                                                   .AddInputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeInt64)
                                                                                   .AddOutputAttr(kNumberTypeFloat64)
                                                                                   .AddOutputAttr(kNumberTypeInt64),
                                                                                 CreateCoalesceKernelPtr<double>}};
}  // namespace

bool CoalesceGpuKernelMod::Launch(const std::vector<AddressPtr> &inputs, const std::vector<AddressPtr> &workspace,
                                  const std::vector<AddressPtr> &outputs, void *stream_ptr) {
  std::vector<void *> input_ptrs = ConvertPtrs(inputs);
  std::vector<void *> work_ptrs = ConvertPtrs(workspace);
  std::vector<void *> output_ptrs = ConvertPtrs(outputs);
  cuda_stream_ = reinterpret_cast<cudaStream_t>(stream_ptr);
  int ret = helper_ptr_->Process(input_ptrs, output_ptrs, work_ptrs, stream_ptr);
  int FALSE_1 = 1;
  int FALSE_2 = 2;
  int FALSE_3 = 3;
  if (ret == FALSE_1) {
    MS_EXCEPTION(ValueError) << "For coalesce, indices cannot be less than 0";
  }
  if (ret == FALSE_2) {
    MS_EXCEPTION(ValueError) << "For coalesce, shape must be greater than 0";
  }
  if (ret == FALSE_3) {
    MS_EXCEPTION(ValueError) << "For coalesce, indices must be less than shape of the corresponding dimension";
  }
  return true;
}

bool CoalesceGpuKernelMod::Init(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
                                const std::vector<KernelTensorPtr> &outputs) {
  auto [is_match, index] = MatchKernelAttr(GetKernelAttrFromTensors(inputs, outputs), GetOpSupport());
  if (!is_match) {
    return false;
  }
  helper_ptr_ = kernel_attr[index].second(kernel_name_, device_id_);
  helper_ptr_ = std::move(kernel_attr[index].second(kernel_name_, device_id_));
  is_need_retrieve_output_shape_ = true;
  return true;
}

int CoalesceGpuKernelMod::Resize(const BaseOperatorPtr &base_operator, const std::vector<KernelTensorPtr> &inputs,
                                 const std::vector<KernelTensorPtr> &outputs,
                                 const std::map<uint32_t, tensor::TensorPtr> &inputsOnHost) {
  std::vector<std::vector<int64_t>> input_shapes;
  for (const auto &input : inputs) {
    auto input_shape = input->GetShapeVector();
    if (!IsValidShape(input_shape)) {
      return KRET_UNKNOWN_SHAPE;
    }
    input_shapes.emplace_back(input_shape);
  }
  auto input_indices_shape = inputs[kIndex0]->GetShapeVector();
  auto input_shape_shape = inputs[kIndex2]->GetShapeVector();
  // push back the max shape to output_size_list_
  auto output_max_indices_shape = input_indices_shape;
  std::vector<int64_t> output_max_values_shape = {input_indices_shape[1]};
  std::vector<std::vector<int64_t>> output_shapes;
  output_shapes.emplace_back(output_max_indices_shape);
  output_shapes.emplace_back(output_max_values_shape);
  output_shapes.emplace_back(input_shape_shape);
  if (helper_ptr_->CalMemSize(input_shapes, output_shapes) == -1) {
    return KRET_RESIZE_FAILED;
  }
  input_size_list_ = helper_ptr_->GetInputSizeList();
  output_size_list_ = helper_ptr_->GetOutputSizeList();
  workspace_size_list_ = helper_ptr_->GetWorkSizeList();
  return KRET_OK;
}

void CoalesceGpuKernelMod::SyncOutputShape() {
  CHECK_CUDA_RET_WITH_EXCEPT_NOTRACE(cudaStreamSynchronize(cuda_stream_), "Coalesce cudaStreamSynchronized failed");
  auto dyn_out = helper_ptr_->GetOutputTensorInfo();
  size_t output_num = outputs_.size();
  for (size_t i = 0; i < output_num; ++i) {
    std::vector<int64_t> shape = outputs_[i]->GetShapeVector();
    std::replace(std::begin(shape), std::end(shape), -1, dyn_out.shapes[0][0]);
    outputs_[i]->SetShapeVector(std::vector<int64_t>(shape.begin(), shape.end()));
  }
}

std::vector<KernelAttr> CoalesceGpuKernelMod::GetOpSupport() {
  std::vector<KernelAttr> support_list;
  (void)std::transform(kernel_attr.begin(), kernel_attr.end(), std::back_inserter(support_list),
                       [](const std::pair<KernelAttr, CoalescePtrCreatorFunc> &item) { return item.first; });
  return support_list;
}

MS_KERNEL_FACTORY_REG(NativeGpuKernelMod, Coalesce, CoalesceGpuKernelMod);
}  // namespace kernel
}  // namespace mindspore
