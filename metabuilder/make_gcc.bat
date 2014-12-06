@set local
@set TOPDIR=%~dp0
@set MBDIR=metatmp
@set GEN=make_gcc_linux

"%TOPDIR%\bin\windows\metabuilder.exe" --input "%~dp0metabuilder.lua" --gen %GEN% --metabase "%TOPDIR%\metabase" --outdir "%MBDIR%"
@IF %ERRORLEVEL% neq 0 GOTO error

GOTO success

:error
pause
exit /b 1

:success
exit /b 0
