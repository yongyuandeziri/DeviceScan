@ECHO OFF

:START

cd /d %~dp0
whdd /list -count %1 >scan.txt
IF ERRORLEVEL 1 GOTO Check_FAIL

wpci /iii %2 >>scan.txt
IF ERRORLEVEL 1 GOTO Check_FAIL

:MemoryStart
WSMBIOS.EXE /c -memnum %3 >>scan.txt
IF ERRORLEVEL 1 GOTO Check_FAIL
goto Check_Pass


:Check_FAIL
echo check fail
exit /b 255

:Check_Pass
echo check pass
exit /b 0