#!/bin/bash
set -e
ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
mkdir -p ${ROOTDIR}/tmp/autotools

#cd ${ROOTDIR}/tmp/autotools
#../../autogen.sh
cd ${ROOTDIR}
autoreconf --force --install --verbose .
cd ${ROOTDIR}/tmp/autotools
${ROOTDIR}/configure

cd ${ROOTDIR}/../external/lua-5.2.2
make generic
cd ${ROOTDIR}/tmp/autotools
make
