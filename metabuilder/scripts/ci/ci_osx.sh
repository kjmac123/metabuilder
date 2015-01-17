#!/bin/bash
set -e

NUMCORES=1
echo BUILDING USING ${NUMCORES} threads

SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/../..

GEN=gnumakeclang_osx
           
pushd ${TOPDIR}/premade/metabuilder/${GEN}/
#cat Makefile
make clean BUILDCONFIG=Release
make -j ${NUMCORES} BUILDCONFIG=Release

cp out/metabuilder/Release/metabuilder ${TOPDIR}/bin
popd

${TOPDIR}/scripts/generatepremade_posix.sh

