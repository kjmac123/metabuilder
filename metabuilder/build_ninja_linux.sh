pushd tmp/metabuilder/ninja_linux/metabuilder/$1
pwd
../../../../../bin/ubuntu/x86_64/ninja/ninja 
./out/metabuilder_$1
popd


