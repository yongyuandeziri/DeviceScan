@ECHO OFF

:START

cd /d %~dp0

echo run WSMBIOS.EXE /c -memnum %1 >>chkmem.txt
WSMBIOS.EXE /c -memnum %1 >chkmem.txt
IF ERRORLEVEL 1 GOTO Check_FAIL
goto Check_Pass


:Check_FAIL
echo check fail >>chkmem.txt
echo Check Install Memory Device Fail >>DevChkError.txt
exit /b 255

:Check_Pass
echo check pass >>chkmem.txt
exit /b 0