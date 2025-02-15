# Copyright 2021-2023 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
""" test syntax for logic expression """

import pytest
import numpy as np

import mindspore.nn as nn
import mindspore
from mindspore import context
from mindspore.common.tensor import Tensor
from mindspore.ops import operations as P

context.set_context(mode=context.GRAPH_MODE)


class Net(nn.Cell):
    def __init__(self,):
        super().__init__()
        self.matmul = P.MatMul()

    def construct(self, x, y):
        out = self.matmul(x, y)
        return out


@pytest.mark.level1
@pytest.mark.platform_x86_gpu_training
@pytest.mark.platform_arm_ascend_training
@pytest.mark.platform_x86_ascend_training
@pytest.mark.env_onecard
def test_call():
    """
    Feature: simple expression
    Description: call network.
    Expectation: No exception
    """
    x = Tensor(np.ones(shape=[1, 3]), mindspore.float32)
    y = Tensor(np.ones(shape=[3, 4]), mindspore.float32)
    net = Net()
    ret = net(x, y)
    assert (ret.asnumpy() == [[3, 3, 3, 3]]).all()
