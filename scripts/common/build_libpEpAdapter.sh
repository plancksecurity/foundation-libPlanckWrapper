#!/usr/bin/env sh
set -exo

echo "ENGINE_LIB_PATH=${INSTPREFIX}/lib" >> local.conf
echo "ENGINE_INC_PATH=${INSTPREFIX}/include" >> local.conf

make
make install PREFIX="${INSTPREFIX}"
