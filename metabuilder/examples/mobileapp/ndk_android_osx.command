#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=../..

pushd ${SCRIPTDIR}
${TOPDIR}/bin/osx/x86_64/metabuilder --input metabuilder.lua --gen ndk_android --metabase ${TOPDIR}/metabase --outdir metatmp
popd

