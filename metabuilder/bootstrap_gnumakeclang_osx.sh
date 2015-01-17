set -e

pushd premade/metabuilder/gnumakeclang_osx
make BUILDCONFIG=Release
popd

cp -v premade/metabuilder/gnumakeclang_osx/out/metabuilder/Release/metabuilder bin/osx/x86_64/
