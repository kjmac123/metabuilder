@SET local
@SET TOPDIR=%~dp0
@SET MBDIR=metatmp
@SET GEN=gnumakegcc_windows

@pushd %TOPDIR%
"bin\windows\metabuilder.exe" --input "%~dp0metabuilder.lua" --gen %GEN% --metabase "metabase" --outdir "%MBDIR%"
@SET RESULT=%ERRORLEVEL%
@popd

@IF %RESULT% neq 0 GOTO error

@GOTO success

:error
@pause
@exit /b 1

:success
@exit /b 0
