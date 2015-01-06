set -e

pushd premade/metabuilder/gnumakeclang_posix
make BUILDCONFIG=Release
popd

cp -v premade/metabuilder/gnumakeclang_posix/out/metabuilder/Release/metabuilder bin/
