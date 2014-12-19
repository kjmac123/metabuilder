#!/bin/bash
set -e
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
TOPDIR=${SCRIPTDIR}/..
MBBIN=${TOPDIR}/bin/metabuilder

pushd ${TOPDIR}
source ${SCRIPTDIR}/generatepremade_allplatforms_common.sh
popd
