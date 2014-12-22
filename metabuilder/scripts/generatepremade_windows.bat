setlocal

SET MBBIN=bin\windows\metabuilder

pushd ..\
REM %MBBIN% --input metabuilder.lua --gen gnumakegcc_cygwin --metabase metabase --outdir metatmp
REM %MBBIN% --input metabuilder.lua --gen gnumakegcc_windows --metabase metabase --outdir metatmp
REM %MBBIN% --input metabuilder.lua --gen gnumakegcc_posix --metabase metabase --outdir metatmp

REM %MBBIN% --input metabuilder.lua --gen gnumakeclang_cygwin --metabase metabase --outdir metatmp
REM %MBBIN% --input metabuilder.lua --gen gnumakeclang_windows --metabase metabase --outdir metatmp
REM %MBBIN% --input metabuilder.lua --gen gnumakeclang_posix --metabase metabase --outdir metatmp

%MBBIN% --input metabuilder.lua --gen msvc2010_windows --metabase metabase --outdir metatmp
%MBBIN% --input metabuilder.lua --gen msvc2012_windows --metabase metabase --outdir metatmp
%MBBIN% --input metabuilder.lua --gen msvc2013_windows --metabase metabase --outdir metatmp
%MBBIN% --input metabuilder.lua --gen msvc2015_windows --metabase metabase --outdir metatmp

robocopy metatmp premade /s
popd