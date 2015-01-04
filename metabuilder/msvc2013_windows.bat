@SETLOCAL
@set TOPDIR=%~dp0
@set MBDIR=metatmp
@set GEN=msvc2013_windows

@REM Grab current dir name (not full path)
@for %%* in (.) do set PROJECTNAME=%%~n*
@set SOLUTION=%MBDIR%\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln

"%TOPDIR%\bin\windows\metabuilder.exe" --input "%~dp0metabuilder.lua" --gen %GEN% --metabase "%TOPDIR%\metabase" --outdir "%MBDIR%"
@IF %ERRORLEVEL% neq 0 GOTO error

@SET MSVCLOCATION=%VS120COMNTOOLS%\..\IDE\devenv.exe

@IF EXIST "%MSVCLOCATION%" SET MSVCLOCATION=%MSVCLOCATION% 

start "%GEN%" "%MSVCLOCATION%" "%SOLUTION%"
GOTO success

:error
pause
exit /b 1

:success
exit /b 0
