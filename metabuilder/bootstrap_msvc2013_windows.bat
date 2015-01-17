@SETLOCAL
@call "%VS120COMNTOOLS%\VsDevCmd.bat"
@IF %ERRORLEVEL% neq 0 GOTO error

@set GEN=msvc2013_windows
@set PROJECTNAME=metabuilder
@set BUILDCONFIG=Release

msbuild "premade\%PROJECTNAME%\%GEN%\%PROJECTNAME%.sln" /t:metabuilder:Rebuild /p:Configuration=%BUILDCONFIG%
@IF %ERRORLEVEL% neq 0 GOTO error

copy /y premade\%PROJECTNAME%\%GEN%\out\%PROJECTNAME%\%BUILDCONFIG%\metabuilder.exe bin\windows\
@IF %ERRORLEVEL% neq 0 GOTO error

GOTO success

:error
pause
exit /b 1

:success
exit /b 0
