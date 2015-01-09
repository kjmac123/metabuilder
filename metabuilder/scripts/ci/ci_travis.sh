#!/bin/bash
set -e

cat /proc/cpuinfo

SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/../..

GEN=gnumakegcc_posix

if [ "${CXX}" = "clang++" ]; then
   GEN=gnumakeclang_posix
fi
            
pushd ${TOPDIR}/premade/metabuilder/${GEN}/
make clean BUILDCONFIG=Release
make BUILDCONFIG=Release

cp out/metabuilder/Release/metabuilder ${TOPDIR}/bin
${SCRIPTDIR}/generatepremade_posix.sh

popd


