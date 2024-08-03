#!/bin/bash

# Make sure to have the dependencies installed before running this
# pip install empy==3.3.4
# pip install pexpect

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
PRJ_DIR="${SCRIPT_DIR}/.."
DSDLC="${PRJ_DIR}/third-party/dronecan_dsdlc/dronecan_dsdlc.py"

OUTPUT_DIR="${SCRIPT_DIR}"
NAMESPACE_DIRS="${PRJ_DIR}/third-party/DSDL/uavcan ${SCRIPT_DIR}/dsdl/sine_esc"

python3 ${DSDLC} -O ${OUTPUT_DIR} ${NAMESPACE_DIRS}
