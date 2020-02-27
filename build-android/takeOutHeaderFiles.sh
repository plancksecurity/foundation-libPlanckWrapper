#!/bin/sh

echo "Install script"
set -e
engine_dir="$1"
adapter_dir="$2"

mkdir -p "$engine_dir/build-android/include/pEp"
cp $2/*.h* "$engine_dir/build-android/include/pEp"

