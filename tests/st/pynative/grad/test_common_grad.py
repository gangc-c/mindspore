# Copyright 2020 Huawei Technologies Co., Ltd
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
""" test_bprop """
import numpy as np
import pytest
import mindspore.nn as nn
from mindspore import context
from mindspore.common import Tensor
from mindspore.common.api import jit
from mindspore.common.parameter import Parameter
from mindspore.ops import operations as P
from tests.mindspore_test_framework.utils.bprop_util import bprop
from tests.st.pynative.utils import GradOfFirstInput

def setup_module():
    context.set_context(mode=context.PYNATIVE_MODE)


class Net(nn.Cell):
    """ Net definition """

    def __init__(self):
        super(Net, self).__init__()
        self.matmul = P.MatMul()
        self.z = Parameter(Tensor(np.array([1.0], np.float32)), name='z')

    @jit
    def construct(self, x, y):
        x = x * self.z
        out = self.matmul(x, y)
        return x, out


def test_bprop_no_sens():
    grads = bprop(Net(), Tensor(np.ones([2, 3]).astype(np.float32)),
                  Tensor(np.ones([3, 2]).astype(np.float32)), wrt=['inputs'])
    print(grads)


def test_bprop_sens():
    grads = bprop(Net(), Tensor(np.ones([2, 3]).astype(np.float32)), Tensor(np.ones([3, 2]).astype(np.float32)),
                  grads_wrt_outputs=(Tensor(np.ones([2, 3]).astype(np.float32)),
                                     Tensor(np.ones([2, 2]).astype(np.float32))), wrt=['inputs'])
    print(grads)


def test_bprop_first_only():
    grads = bprop(Net(), Tensor(np.ones([2, 3]).astype(np.float32)), Tensor(np.ones([3, 2]).astype(np.float32)),
                  grads_wrt_outputs=(Tensor(np.ones([2, 3]).astype(np.float32)),
                                     Tensor(np.ones([2, 2]).astype(np.float32))))
    print(grads)


def test_bprop_wrt_params():
    net = Net()
    grads = bprop(net, Tensor(np.ones([2, 3]).astype(np.float32)), Tensor(np.ones([3, 2]).astype(np.float32)),
                  grads_wrt_outputs=(Tensor(np.ones([2, 3]).astype(np.float32)),
                                     Tensor(np.ones([2, 2]).astype(np.float32))),
                  wrt=['params'],
                  params=net.trainable_params())
    print(grads)


def test_bprop_wrt_params_no_sens():
    net = Net()
    grads = bprop(net, Tensor(np.ones([2, 3]).astype(np.float32)), Tensor(np.ones([3, 2]).astype(np.float32)),
                  wrt=['params'],
                  params=net.trainable_params())
    print(grads)


def test_bprop_wrt_inputs_and_params():
    net = Net()
    grads = bprop(net, Tensor(np.ones([2, 3]).astype(np.float32)), Tensor(np.ones([3, 2]).astype(np.float32)),
                  grads_wrt_outputs=(Tensor(np.ones([2, 3]).astype(np.float32)),
                                     Tensor(np.ones([2, 2]).astype(np.float32))),
                  wrt=['inputs', 'params'],
                  params=net.trainable_params())
    print(grads)


@pytest.mark.level0
@pytest.mark.platform_x86_cpu
@pytest.mark.env_onecard
def test_network_with_dict_output():
    """
    Feature: Test sens dict
    Description: Net out is dict
    Expectation: Success
    """
    class DicNet(nn.Cell):
        def __init__(self):
            super().__init__()
            self.relu = P.ReLU()

        def construct(self, x):
            y = self.relu(x)
            out = {Tensor(True): y}
            return out

    x = np.array([[0.8, 0.6, 0.2], [1.8, 1.3, 1.1]])
    ms_net = DicNet()
    # No sens
    ms_grad = GradOfFirstInput(ms_net, False)
    grad_out = ms_grad(Tensor(x))
    assert np.allclose(np.ones_like(x), grad_out.asnumpy())

    # Have sens
    out = ms_net(Tensor(x))
    ms_grad = GradOfFirstInput(ms_net, True)
    grad_out = ms_grad(Tensor(x), out)
    assert np.allclose(x, grad_out.asnumpy())
