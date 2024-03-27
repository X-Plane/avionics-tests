#!/bin/sh

TARGET=avionics
BUILD_ZIP="$TARGET.zip"

(./build_redist && lima ./build_redist) || exit 1
zip -rq $BUILD_ZIP "$TARGET" -x ".*" -x "__MACOSX" 