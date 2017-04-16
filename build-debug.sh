#!/bin/bash

_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${_DIR}/build/debug
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

cmake ${_DIR} \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMAKE_BUILD_TYPE=Debug

make -j2

