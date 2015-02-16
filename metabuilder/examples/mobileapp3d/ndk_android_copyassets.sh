#!/bin/sh
set -e

TMPDIR=metatmp
APPNAME=mobileapp3d
GEN=ndk_android

OUTDIR=${TMPDIR}/${APPNAME}/${GEN}/${APPNAME}

ASSETSRCDIR=projects/android/assets
DATASRCDIR=data

function CopyAssetsForConfig()
{
  SRC_RELATIVE_DIR=$1
  DST_RELATIVE_DIR=$2

  DST_DIR=${OUTDIR}/${DST_RELATIVE_DIR}
  mkdir -p "${DST_DIR}"

  rsync -av "${SRC_RELATIVE_DIR}/" "${DST_DIR}/"
}

CopyAssetsForConfig "${ASSETSRCDIR}" "Debug/assets"
CopyAssetsForConfig "${ASSETSRCDIR}" "Release/assets"
CopyAssetsForConfig "${ASSETSRCDIR}" "Master/assets"

CopyAssetsForConfig "${DATASRCDIR}/gles" "Debug/assets/gles"
CopyAssetsForConfig "${DATASRCDIR}/gles" "Release/assets/gles"
CopyAssetsForConfig "${DATASRCDIR}/gles" "Master/assets/gles"
