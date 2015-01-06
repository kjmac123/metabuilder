set -e

pushd premade/metabuilder/gnumakegcc_posix
make BUILDCONFIG=Release
popd

cp -v premade/metabuilder/gnumakegcc_posix/out/metabuilder/Release/metabuilder bin/
