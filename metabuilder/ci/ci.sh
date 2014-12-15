#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/..

pushd ${TOPDIR}/premade/metabuilder/gnumakegcc_posix/
make clean BUILDCONFIG=Release
make -j4 BUILDCONFIG=Release

cp out/metabuilder/Release/metabuilder ${TOPDIR}/bin
${TOPDIR}/generatepremade_allplatforms_posix.sh

popd

