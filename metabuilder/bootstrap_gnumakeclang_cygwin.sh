#!/bin/bash
set -e

pushd premade/metabuilder/gnumakeclang_cygwin
make BUILDCONFIG=Release
popd

cp -v premade/metabuilder/gnumakeclang_cygwin/out/metabuilder/Release/metabuilder bin/
