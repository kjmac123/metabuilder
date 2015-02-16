#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=../..
TMPDIR=metatmp
APPNAME=mobileapp3d
GEN=ndk_android

pushd ${SCRIPTDIR}
${TOPDIR}/bin/osx/x86_64/metabuilder --input metabuilder.lua --gen ${GEN} --metabase ${TOPDIR}/metabase --outdir ${TMPDIR}
popd
