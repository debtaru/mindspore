# Copyright 2019 Huawei Technologies Co., Ltd
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

import pytest
from mindspore import Tensor
import mindspore.nn as nn
from mindspore.common.api import ms_function
import numpy as np
import mindspore.context as context

class NetSparseSoftmaxCrossEntropyWithLogits(nn.Cell):
    def __init__( self):
        super(NetSparseSoftmaxCrossEntropyWithLogits, self).__init__()
        self.loss = nn.SoftmaxCrossEntropyWithLogits(is_grad=False, sparse=True)
        self.dlogits = nn.SoftmaxCrossEntropyWithLogits(is_grad=True, sparse=True)

    def construct(self, logits, labels):
        return (self.loss(logits, labels), self.dlogits(logits, labels))


@pytest.mark.level0
@pytest.mark.platform_x86_gpu_training
@pytest.mark.env_onecard
def test_sparse_softmax_cross_entropy_with_logits():
    logits = Tensor(np.array([[1,1,10],
                              [1,10,1],
                              [10,1,1]]).astype(np.float32))
    labels = Tensor(np.array([2,1,0]).astype(np.int32))
    expect_loss = 0.0002467
    expect_dlogits = np.array([[4.1126452e-05, 4.1126452e-05, -8.2234539e-05],
                               [4.1126452e-05, -8.2234539e-05, 4.1126452e-05],
                               [-8.2234539e-05, 4.1126452e-05, 4.1126452e-05]]).astype(np.float32)

    context.set_context(mode=context.GRAPH_MODE, device_target='GPU')
    sparse_softmax_cross_entropy_with_logits = NetSparseSoftmaxCrossEntropyWithLogits()
    output = sparse_softmax_cross_entropy_with_logits(logits, labels)
    error0 = 1.0e-6
    diff0 = output[0].asnumpy() - expect_loss
    assert np.all(abs(diff0) < error0)

    error1 = np.ones(shape=[3, 3]) * 1.0e-6
    diff1 = output[1].asnumpy() - expect_dlogits
    assert np.all(abs(diff1) < error1)

    context.set_context(mode=context.PYNATIVE_MODE, device_target='GPU')
    sparse_softmax_cross_entropy_with_logits = NetSparseSoftmaxCrossEntropyWithLogits()
    output = sparse_softmax_cross_entropy_with_logits(logits, labels)
    error0 = 1.0e-6
    diff0 = output[0].asnumpy() - expect_loss
    assert np.all(abs(diff0) < error0)

    error1 = np.ones(shape=[3, 3]) * 1.0e-6
    diff1 = output[1].asnumpy() - expect_dlogits
    assert np.all(abs(diff1) < error1)
