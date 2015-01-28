set TOPDIR=%~dp0..\..
set MBDIR=projects\android
set GEN=ndk_android

@for %%* in (.) do set PROJECTNAME=%%~n*

%TOPDIR%\bin\windows\metabuilder --input metabuilder.lua --gen %GEN% --metabase %TOPDIR%\metabase --outdir metatmp

start metatmp\%PROJECTNAME%\%GEN%\

GOTO success

:error
pause
exit /b 1

:success
exit /b 0
