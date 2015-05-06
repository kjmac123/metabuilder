SET ASSETSRCDIR=projects/android/assets
SET DATASRCDIR=data

robocopy %DATASRCDIR% %ASSETSRCDIR% /s

