#!/bin/bash
set -e

platform="unknown"
unamestr=`uname`
if [[ "$unamestr" == 'Darwin' ]]; then
	platform="osx"
else
	platform="posix"
fi

SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/../..

echo Launching build script for platform ${platform}
${TOPDIR}/scripts/generatepremade_${platform}.sh

