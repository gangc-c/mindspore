/**
 * Copyright 2022 Huawei Technologies Co., Ltd
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

#ifndef NNACL_BASE_SCATTER_ND_BINARY_H_
#define NNACL_BASE_SCATTER_ND_BINARY_H_

#include "nnacl/scatter_nd_parameter.h"

#ifdef __cplusplus
extern "C" {
#endif
int ScatterNDUpdate(void *output, const void *update, int *output_unit_offsets, const ScatterNDParameter *param,
                    int task_id);

int ScatterNDAdd(const void *update, void *output, int *output_unit_offsets, const ScatterNDParameter *param, int type,
                 int task_id);
#ifdef __cplusplus
}
#endif

#endif  // MINDSPORE_NNACL_BASE_SCATTER_ND_BINARY_H_
