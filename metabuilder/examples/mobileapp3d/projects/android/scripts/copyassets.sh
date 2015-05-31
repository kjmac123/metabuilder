#!/bin/sh
set -e

TMPDIR=metatmp
APPNAME=mobileapp3d
GEN=ndk_android

OUTDIR=${TMPDIR}/${APPNAME}/${GEN}/${APPNAME}

ASSETSRCDIR=projects/android/assets

function CopyAssetsForConfig()
{
  CONFIG=$1

  CONFIGASSETDIR=${OUTDIR}/${CONFIG}/assets

  rsync -av "${ASSETSRCDIR}/" "${CONFIGASSETDIR}/"
}

CopyAssetsForConfig "Debug"
CopyAssetsForConfig "Release"
CopyAssetsForConfig "Master"
