@set local
@set TOPDIR=%~dp0..\..
@set MBDIR=metatmp
@set GEN=msvc2010_windowsx64

@REM Grab current dir name (not full path)
@for %%* in (.) do set PROJECTNAME=%%~n*
@set SOLUTION=%MBDIR%\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln

"%TOPDIR%\bin\windows\metabuilder.exe" --input "%~dp0metabuilder.lua" --gen %GEN% --metabase "%TOPDIR%\metabase" --outdir "%MBDIR%"
@IF %ERRORLEVEL% neq 0 GOTO error

@SET MSVCLOCATION_PRO=%VS100COMNTOOLS%\..\IDE\devenv.exe
@SET MSVCLOCATION_EXPRESS1=%VS100COMNTOOLS%\..\IDE\wdexpress.exe
@SET MSVCLOCATION_EXPRESS2=%VS100COMNTOOLS%\..\IDE\VCExpress.exe

@IF EXIST "%MSVCLOCATION_PRO%" SET MSVCLOCATION=%MSVCLOCATION_PRO% 
@IF EXIST "%MSVCLOCATION_EXPRESS1%" SET MSVCLOCATION=%MSVCLOCATION_EXPRESS1% 
@IF EXIST "%MSVCLOCATION_EXPRESS2%" SET MSVCLOCATION=%MSVCLOCATION_EXPRESS2% 

start "%GEN%" "%MSVCLOCATION%" "%SOLUTION%"
GOTO success

:error
pause
exit /b 1

:success
exit /b 0
