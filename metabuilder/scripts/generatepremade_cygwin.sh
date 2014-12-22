#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/..
MBBIN=${TOPDIR}/bin/metabuilder

pushd ${TOPDIR}

${MBBIN} --input metabuilder.lua --gen gnumakegcc_cygwin --metabase metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakegcc_posix --metabase metabase --outdir metatmp

${MBBIN} --input metabuilder.lua --gen gnumakeclang_cygwin --metabase metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakeclang_posix --metabase metabase --outdir metatmp

cp -rfv metatmp/* premade

popd
