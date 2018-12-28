#!/bin/bash

# Export environment variables
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${script_dir}/../ >/dev/null
export VGE_ROOT_DIR=$(pwd)
popd >/dev/null

export VGE_BUILD_DIR=${VGE_ROOT_DIR}/build
export VGE_SOURCE_DIR=${VGE_ROOT_DIR}/src
export VGE_SETUP_DIR=${VGE_ROOT_DIR}/setup
export VGE_SCRIPTS_DIR=${VGE_ROOT_DIR}/scripts
export VGE_RESOURCE_DIR=${VGE_ROOT_DIR}/resources

source ${VGE_SCRIPTS_DIR}/vge_compile.sh
