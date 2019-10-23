SET SRC_DIR=E:\Projects\RnD\STK\projects\CAPI\x64\Release
SET DST_DIR=E:\Projects\RnD\Noodle\libs\x64\windows

if not exist %DST_DIR%\ mkdir %DST_DIR%

@ECHO off
XCOPY %SRC_DIR%\STK.dll %DST_DIR% /y
REM if exist %SRC_DIR%\scripts XCOPY %SRC_DIR%\scripts %SCRIPT_DIR% /s /i /y
ECHO " ---> Deployed STK Library 1.0 : "%DST_DIR%
