# Copyright 2023 Huawei Technologies Co., Ltd
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
"""
utils for mslite bench
"""

from mslite_bench.utils.arg_parser import ArgParser
from mslite_bench.utils.infer_log import InferLogger
from mslite_bench.utils.type_assert import typeassert

__all__ = ['ArgParser', 'InferLogger', 'typeassert']
