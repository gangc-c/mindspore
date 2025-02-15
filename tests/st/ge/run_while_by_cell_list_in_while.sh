#!/bin/bash
# Copyright 2022 Huawei Technologies Co., Ltd
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
BASE_PATH=$(cd "$(dirname $0)"; pwd)
rm -rf ${BASE_PATH}/while_by_cell_list_in_while
mkdir ${BASE_PATH}/while_by_cell_list_in_while
export PYTHONPATH=${BASE_PATH}/:$PYTHONPATH
export MS_ENABLE_GE=1
export MS_DEV_FORCE_ACL=1
unset SLOG_PRINT_TO_STDOUT
cd ${BASE_PATH}/while_by_cell_list_in_while
echo "start test while by cell list in while with ge backend"
env > env.log
python ../run_while_by_cell_list_in_while.py > test_while_by_cell_list_in_while.log 2>&1 &
process_pid=`echo $!`
wait ${process_pid}
unset MS_ENABLE_GE
unset MS_DEV_FORCE_ACL
status=`echo $?`
if [ "${status}" != "0" ]; then
    echo "[ERROR] test while by cell list in while with ge backend failed. status: ${status}"
    exit 1
else
    echo "[INFO] test while by cell list in while with ge backend success."
fi

exit 0
