#!/bin/bash
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

set -e
BASEPATH=$(
  cd "$(dirname "$0")"
  pwd
)
PROJECT_PATH=${BASEPATH}/../../..
if [ $BUILD_PATH ]; then
  echo "BUILD_PATH = $BUILD_PATH"
else
  BUILD_PATH=${PROJECT_PATH}/build
  echo "BUILD_PATH = $BUILD_PATH"
fi
cd ${BUILD_PATH}/mindspore/tests/ut/cpp

export LD_LIBRARY_PATH=${BUILD_PATH}/mindspore/googletest/googlemock/gtest:${PROJECT_PATH}/mindspore/python/mindspore:${PROJECT_PATH}/mindspore/python/mindspore/lib:${PROJECT_PATH}/graphengine/910/third_party/prebuild/x86_64:${PROJECT_PATH}/graphengine/910/third_party/prebuild/aarch64:${LD_LIBRARY_PATH}
export PYTHONPATH=${PROJECT_PATH}/tests/ut/cpp/python_input:$PYTHONPATH:${PROJECT_PATH}/mindspore/python:${PROJECT_PATH}/tests/ut/python:${PROJECT_PATH}
export GLOG_v=2
export GC_COLLECT_IN_CELL=1
## set op info config path
export MINDSPORE_OP_INFO_PATH=${PROJECT_PATH}/tests/ut/cpp/stub/config/op_info.config

## prepare data for dataset & mindrecord
cp -fr $PROJECT_PATH/tests/ut/data ${PROJECT_PATH}/build/mindspore/tests/ut/cpp/
## prepare album dataset, uses absolute path so has to be generated
python ${PROJECT_PATH}/build/mindspore/tests/ut/cpp/data/dataset/testAlbum/gen_json.py

RET=0
if [ $# -gt 0 ]; then
  ./ut_CORE_tests --gtest_filter=$1
  ./ut_API_tests --gtest_filter=$1
  ./ut_FRONTEND_tests --gtest_filter=$1
  ./ut_OLD_BACKEND_tests --gtest_filter=$1
  ./ut_BACKEND_tests --gtest_filter=$1
  ./ut_PS_tests --gtest_filter=$1
  ./ut_OTHERS_tests --gtest_filter=$1
  ./ut_MINDDATA0_tests --gtest_filter=$1
  ./ut_MINDDATA1_tests --gtest_filter=$1
  exit 0
fi

pids=()
tasks=(./ut_CORE_tests ./ut_API_tests ./ut_FRONTEND_tests ./ut_OLD_BACKEND_tests ./ut_BACKEND_tests ./ut_PS_tests ./ut_OTHERS_tests ./ut_MINDDATA0_tests ./ut_MINDDATA1_tests)
set +e
for task in "${tasks[@]}"; do
  $task &
  pids+=($!)
done
cd -
for pid in "${pids[@]}"; do
  wait $pid
  status=$?
  if [ $status != 0 ]; then
    RET=$status
  fi
done

exit $RET
