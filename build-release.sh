#!/bin/bash

_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd ${_DIR}/build/release

cmake ${_DIR} \
  -DCMAKE_INSTALL_PREFIX=/usr \
  -DCMAKE_BUILD_TYPE=Release

make -j2

