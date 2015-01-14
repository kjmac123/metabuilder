@SETLOCAL
@call "%VS120COMNTOOLS%\VsDevCmd.bat"
@IF %ERRORLEVEL% neq 0 GOTO error

@set TOPDIR=%~dp0..

@set GEN=msvc2013_windows
@set PROJECTNAME=metabuilder

@set MBDIR_CIGENERATION2=%TOPDIR%\metatmp_cigen2
@set MBDIR_CIGENERATION3=%TOPDIR%\metatmp_cigen3

@SET MBBIN_CIGENERATION1=%TOPDIR%\bin\windows\metabuilder.exe
@SET MBBIN_CIGENERATION2=%MBDIR_CIGENERATION2%\%PROJECTNAME%\%GEN%\out\%PROJECTNAME%\Release\%PROJECTNAME%.exe
@SET MBBIN_CIGENERATION3=%MBDIR_CIGENERATION3%\%PROJECTNAME%\%GEN%\out\%PROJECTNAME%\Release\%PROJECTNAME%.exe

REM GENERATION 1: generate project using old executable with latest lua files
pushd "%TOPDIR%"
"%MBBIN_CIGENERATION1%" --input "metabuilder.lua" --gen %GEN% --metabase "metabase" --outdir "%MBDIR_CIGENERATION2%"
@IF %ERRORLEVEL% neq 0 GOTO error
popd

REM GENERATION 2: generate project using new executable with latest lua files
msbuild "%MBDIR_CIGENERATION2%\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln" /t:metabuilder:Rebuild /p:Configuration=Release
@IF %ERRORLEVEL% neq 0 GOTO error

pushd "%TOPDIR%"
"%MBBIN_CIGENERATION2%" --input "metabuilder.lua" --gen %GEN% --metabase "metabase" --outdir "%MBDIR_CIGENERATION3%"
@IF %ERRORLEVEL% neq 0 GOTO error
popd

msbuild "%MBDIR_CIGENERATION3%\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln" /t:metabuilder:Rebuild /p:Configuration=Release
@IF %ERRORLEVEL% neq 0 GOTO error

@call generatepremade_windows.bat %MBBIN_CIGENERATION3%
@IF %ERRORLEVEL% neq 0 GOTO error

GOTO success

:error
pause
exit /b 1

:success
exit /b 0
