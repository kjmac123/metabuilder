#!/bin/bash
set -e

#cat /proc/cpuinfo

TOPDIR=../..

GEN=gnumakegcc_posix

if [ "${CXX}" = "clang++" ]; then
   GEN=gnumakeclang_posix
fi
            
pushd ${TOPDIR}/premade/metabuilder/${GEN}/
cat Makefile
make clean BUILDCONFIG=Release
make BUILDCONFIG=Release

cp out/metabuilder/Release/metabuilder ${TOPDIR}/bin
${TOPDIR}/scripts/generatepremade_posix.sh

popd


