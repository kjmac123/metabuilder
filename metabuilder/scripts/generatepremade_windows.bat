@echo on
setlocal

@set TOPDIR=%~dp0..

IF "%1" == "" (
	SET MBBIN=%TOPDIR%\bin\windows\metabuilder
) ELSE (
	SET MBBIN=%1
)

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

%MBBIN% --input metabuilder.lua --gen xcode_osx --metabase metabase --outdir metatmp_premade --endstyle unix

robocopy metatmp_premade premade /s
popd
