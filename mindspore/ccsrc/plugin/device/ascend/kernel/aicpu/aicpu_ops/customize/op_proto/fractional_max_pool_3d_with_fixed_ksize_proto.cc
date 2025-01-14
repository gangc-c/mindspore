/**
 * Copyright (c) 2023-2023 Huawei Technologies Co., Ltd.  All rights reserved.
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

#include "inc/fractional_max_pool_3d_with_fixed_ksize_op.h"
#include "register/op_impl_registry.h"
#include "utils/util.h"

namespace ge {
// -----------------FractionalMaxPool3DWithFixedKsize start----------------
IMPLEMT_COMMON_INFERFUNC(FractionalMaxPool3DWithFixedKsizeInferShape) {
  const size_t DIM_SIZE1 = 1;
  const size_t DIM_SIZE3 = 3;
  const size_t DIM_SIZE4 = 4;
  const size_t DIM_SIZE5 = 5;
  TensorDesc input_desc = op.GetInputDescByName("x");
  TensorDesc random_samples_desc = op.GetInputDescByName("random_samples");
  TensorDesc out_desc = op.GetOutputDescByName("y");
  TensorDesc argmax_desc = op.GetOutputDescByName("argmax");
  Format input_format = input_desc.GetFormat();
  DataType input_type = input_desc.GetDataType();
  DataType argmax_dtype = argmax_desc.GetDataType();

  std::vector<int64_t> input_shape = input_desc.GetShape().GetDims();
  auto input_dims = input_shape.size();
  if ((input_dims != DIM_SIZE4) && (input_dims != DIM_SIZE5)) {
    OP_LOGE(TbeGetName(op).c_str(), "length of x should be 4 or 5!");
    return GRAPH_FAILED;
  }
  std::vector<int64_t> outputshapeList;
  if (GRAPH_SUCCESS != op.GetAttr("output_shape", outputshapeList)) {
    std::string err_msg = GetInputInvalidErrMsg("output_shape");
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }

  if ((outputshapeList.size() != DIM_SIZE1) && (outputshapeList.size() != DIM_SIZE3)) {
    string excepted_size = ConcatString(DIM_SIZE1, " or ", DIM_SIZE3);
    std::string err_msg = GetAttrSizeErrMsg("outputshapeList", ConcatString(outputshapeList.size()), excepted_size);
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }
  if (outputshapeList.size() == DIM_SIZE1) {
    for (int64_t i = 0; i < 3; i++) {
      outputshapeList[i] = outputshapeList[0];
    }
  }

  std::vector<float> ksizeList;
  if (GRAPH_SUCCESS != op.GetAttr("ksize", ksizeList)) {
    std::string err_msg = GetInputInvalidErrMsg("ksize");
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }

  if ((ksizeList.size() != DIM_SIZE1) && (ksizeList.size() != DIM_SIZE3)) {
    string excepted_size = ConcatString(DIM_SIZE1, " or ", DIM_SIZE3);
    std::string err_msg = GetAttrSizeErrMsg("ksizeList", ConcatString(ksizeList.size()), excepted_size);
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }

  if (ksizeList.size() == DIM_SIZE1) {
    for (int64_t i = 0; i < 3; i++) {
      ksizeList[i] = ksizeList[0];
    }
  }

  std::string data_format;
  if (GRAPH_SUCCESS != op.GetAttr("data_format", data_format)) {
    std::string err_msg = GetInputInvalidErrMsg("data_format");
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }
  if (data_format != "NDHWC" && data_format != "NCDHW") {
    string expected_format_list = ConcatString("NDHWC, NCDHW");
    std::string err_msg = GetInputFormatNotSupportErrMsg("data_format", expected_format_list, data_format);
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }

  // set data type
  out_desc.SetDataType(input_type);
  if (op.UpdateOutputDesc("y", out_desc) != GRAPH_SUCCESS) {
    AICPU_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), std::string("update output[y] desc failed."));
    return GRAPH_FAILED;
  }

  if (argmax_dtype == DT_UNDEFINED) {
    argmax_desc.SetDataType(DT_INT64);
  }
  argmax_desc.SetDataType(argmax_dtype);
  if (op.UpdateOutputDesc("argmax", argmax_desc) != GRAPH_SUCCESS) {
    AICPU_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), std::string("update output[argmax] desc failed."));
    return GRAPH_FAILED;
  }

  // set  shape
  if ((input_format == FORMAT_NCDHW && data_format != "NCDHW") ||
      (input_format == FORMAT_NDHWC && data_format != "NDHWC")) {
    string expected_format = ConcatString("Format of input must be same with data_format! input_format:", input_format,
                                          ", data_format:", data_format);
    std::string err_msg = OtherErrMsg(expected_format);
    VECTOR_INFER_SHAPE_INNER_ERR_REPORT(TbeGetName(op).c_str(), err_msg);
    return GRAPH_FAILED;
  }
  std::vector<int64_t> output_size;
  int64_t n_dim = 0;
  int64_t c_dim = 0;
  int64_t outputT = outputshapeList[0];
  int64_t outputH = outputshapeList[1];
  int64_t outputW = outputshapeList[2];

  if (input_dims == 4) {
    if (data_format == "NCDHW") {
      c_dim = input_desc.GetShape().GetDim(0);
      output_size.push_back(c_dim);
      output_size.push_back(outputT);
      output_size.push_back(outputH);
      output_size.push_back(outputW);
    } else {
      c_dim = input_desc.GetShape().GetDim(3);
      output_size.push_back(outputT);
      output_size.push_back(outputH);
      output_size.push_back(outputW);
      output_size.push_back(c_dim);
    }
  } else {
    if (data_format == "NCDHW") {
      n_dim = input_desc.GetShape().GetDim(0);
      c_dim = input_desc.GetShape().GetDim(1);
      output_size.push_back(n_dim);
      output_size.push_back(c_dim);
      output_size.push_back(outputT);
      output_size.push_back(outputH);
      output_size.push_back(outputW);
    } else {
      n_dim = input_desc.GetShape().GetDim(0);
      c_dim = input_desc.GetShape().GetDim(4);
      output_size.push_back(n_dim);
      output_size.push_back(outputT);
      output_size.push_back(outputH);
      output_size.push_back(outputW);
      output_size.push_back(c_dim);
    }
  }
  out_desc.SetFormat(input_format);
  argmax_desc.SetFormat(ge::FORMAT_ND);

  out_desc.SetShape(ge::Shape(output_size));
  if (op.UpdateOutputDesc("y", out_desc) != GRAPH_SUCCESS) {
    OP_LOGE(TbeGetName(op).c_str(), "Fail to update output y!");
    return GRAPH_FAILED;
  }
  argmax_desc.SetShape(ge::Shape(output_size));
  if (op.UpdateOutputDesc("argmax", argmax_desc) != GRAPH_SUCCESS) {
    OP_LOGE(TbeGetName(op).c_str(), "Fail to update output argmax!");
    return GRAPH_FAILED;
  }

  return GRAPH_SUCCESS;
}

CUST_IMPLEMT_VERIFIER(FractionalMaxPool3DWithFixedKsize, FractionalMaxPool3DWithFixedKsizeVerify) {
  return GRAPH_SUCCESS;
}

CUST_COMMON_INFER_FUNC_REG(FractionalMaxPool3DWithFixedKsize, FractionalMaxPool3DWithFixedKsizeInferShape);
CUST_VERIFY_FUNC_REG(FractionalMaxPool3DWithFixedKsize, FractionalMaxPool3DWithFixedKsizeVerify);
// -----------------FractionalMaxPool3DWithFixedKsize end----------------
}  // namespace ge