@SETLOCAL
@set TOPDIR=%~dp0..\..

SET MSVCVER=%1

@call "%VS120COMNTOOLS%\VsDevCmd.bat"
@IF %ERRORLEVEL% neq 0 GOTO error

@set GEN=%MSVCVER%_windows
@set PROJECTNAME=metabuilder
@set BUILDCONFIG=Release

pushd "%TOPDIR%"
msbuild "premade\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln" /t:metabuilder:Rebuild /p:Configuration=%BUILDCONFIG%
@IF %ERRORLEVEL% neq 0 GOTO error
popd

GOTO success

:error
exit /b 1

:success
exit /b 0
