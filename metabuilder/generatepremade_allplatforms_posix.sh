#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}
MBBIN=${TOPDIR}/bin/metabuilder

pushd ${SCRIPTDIR}
pwd

${MBBIN} --input metabuilder.lua --gen gnumakegcc_cygwin --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakegcc_windows --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakegcc_posix --metabase ${TOPDIR}/metabase --outdir metatmp

${MBBIN} --input metabuilder.lua --gen gnumakeclang_cygwin --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakeclang_windows --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen gnumakeclang_posix --metabase ${TOPDIR}/metabase --outdir metatmp

${MBBIN} --input metabuilder.lua --gen msvc2010_windows --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen msvc2012_windows --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen msvc2013_windows --metabase ${TOPDIR}/metabase --outdir metatmp
${MBBIN} --input metabuilder.lua --gen msvc2015_windows --metabase ${TOPDIR}/metabase --outdir metatmp

cp -rv metatmp/* premade

popd

