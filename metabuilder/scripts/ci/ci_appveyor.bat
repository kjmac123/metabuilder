@SETLOCAL
@set TOPDIR=%~dp0..\..

call %TOPDIR%\scripts\ci\ci_windows.bat msvc2012
IF %ERRORLEVEL% neq 0 GOTO error

GOTO success

:error
pause
exit /b 1

:success
exit /b 0
