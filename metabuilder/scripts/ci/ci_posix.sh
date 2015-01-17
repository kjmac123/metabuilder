#!/bin/bash
set -e

NUMCORES=`grep -c ^processor /proc/cpuinfo`
echo BUILDING USING ${NUMCORES} threads

SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/../..

GEN=gnumakegcc_posix

if [ "${CXX}" = "clang++" ]; then
   GEN=gnumakeclang_posix
fi
            
pushd ${TOPDIR}/premade/metabuilder/${GEN}/
make clean BUILDCONFIG=Release
make -j ${NUMCORES} BUILDCONFIG=Release

cp out/metabuilder/Release/metabuilder ${TOPDIR}/bin
popd

${TOPDIR}/scripts/generatepremade_posix.sh

