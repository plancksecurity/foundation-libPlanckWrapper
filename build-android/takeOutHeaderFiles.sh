#!/bin/sh

set -e
dir="$1"
echo "$dir/build-android/include/pEp"
mkdir -p "$dir/build-android/include/pEp"
cd "."
echo `pwd`
cp ../../*.h* "$dir/build-android/include/pEp"

