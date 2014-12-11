#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=../..

pushd ${SCRIPTDIR}
pwd
${TOPDIR}/bin/metabuilder --input metabuilder.lua --gen gnumakeclang_posix --metabase ${TOPDIR}/metabase --outdir metatmp
popd

