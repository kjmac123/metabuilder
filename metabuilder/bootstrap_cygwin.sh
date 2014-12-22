#!/bin/bash
set -e

pushd premade/metabuilder/gnumakegcc_cygwin
make BUILDCONFIG=Release
popd

cp -v premade/metabuilder/gnumakegcc_cygwin/out/metabuilder/Release/metabuilder bin/
