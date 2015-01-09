#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/..
MBBIN=${TOPDIR}/bin/osx/x86_64/metabuilder

pushd ${TOPDIR}

${MBBIN} --input metabuilder.lua --gen gnumakegcc_cygwin --metabase metabase --outdir metatmp_premade --endstyle unix
${MBBIN} --input metabuilder.lua --gen gnumakegcc_mingw --metabase metabase --outdir metatmp_premade --endstyle unix
${MBBIN} --input metabuilder.lua --gen gnumakegcc_posix --metabase metabase --outdir metatmp_premade --endstyle unix

${MBBIN} --input metabuilder.lua --gen gnumakeclang_cygwin --metabase metabase --outdir metatmp_premade --endstyle unix
${MBBIN} --input metabuilder.lua --gen gnumakeclang_mingw --metabase metabase --outdir metatmp_premade --endstyle unix
${MBBIN} --input metabuilder.lua --gen gnumakeclang_posix --metabase metabase --outdir metatmp_premade --endstyle unix

${MBBIN} --input metabuilder.lua --gen msvc2010_windows --metabase metabase --outdir metatmp_premade --endstyle windows
${MBBIN} --input metabuilder.lua --gen msvc2012_windows --metabase metabase --outdir metatmp_premade --endstyle windows
${MBBIN} --input metabuilder.lua --gen msvc2013_windows --metabase metabase --outdir metatmp_premade --endstyle windows
${MBBIN} --input metabuilder.lua --gen msvc2015_windows --metabase metabase --outdir metatmp_premade --endstyle windows

${MBBIN} --input metabuilder.lua --gen xcode_osx --metabase metabase --outdir metatmp_premade --endstyle unix

cp -rfv metatmp_premade/* premade

popd
