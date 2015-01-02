setlocal

SET MBBIN=bin\windows\metabuilder

pushd ..\
%MBBIN% --input metabuilder.lua --gen gnumakegcc_cygwin --metabase metabase --outdir metatmp_premade --endstyle unix
%MBBIN% --input metabuilder.lua --gen gnumakegcc_mingw --metabase metabase --outdir metatmp_premade --endstyle unix
%MBBIN% --input metabuilder.lua --gen gnumakegcc_posix --metabase metabase --outdir metatmp_premade --endstyle unix

%MBBIN% --input metabuilder.lua --gen gnumakeclang_cygwin --metabase metabase --outdir metatmp_premade --endstyle unix
%MBBIN% --input metabuilder.lua --gen gnumakeclang_mingw --metabase metabase --outdir metatmp_premade --endstyle unix
%MBBIN% --input metabuilder.lua --gen gnumakeclang_posix --metabase metabase --outdir metatmp_premade --endstyle unix

%MBBIN% --input metabuilder.lua --gen msvc2010_windows --metabase metabase --outdir metatmp_premade
%MBBIN% --input metabuilder.lua --gen msvc2012_windows --metabase metabase --outdir metatmp_premade
%MBBIN% --input metabuilder.lua --gen msvc2013_windows --metabase metabase --outdir metatmp_premade
%MBBIN% --input metabuilder.lua --gen msvc2015_windows --metabase metabase --outdir metatmp_premade

robocopy metatmp_premade premade /s
popd
