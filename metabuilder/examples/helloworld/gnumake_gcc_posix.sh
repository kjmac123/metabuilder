#!/bin/bash
set -e
ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

${ROOTDIR}/../../bin/metabuilder --input metabuilder.lua --gen gnumake_gcc_posix --metabase ./metabase --outdir metatmp
