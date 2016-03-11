@ECHO OFF

:START

cd /d %~dp0

echo run WPCI.exe /iii %1 >>chkpci.txt
WPCI.exe /iii %1 >chkpci.txt
IF ERRORLEVEL 1 GOTO Check_FAIL
goto Check_Pass


:Check_FAIL
echo check fail >>chkpci.txt
echo Check PCI Device Fail >>DevChkError.txt
exit /b 255

:Check_Pass
echo check pass >>chkpci.txt
exit /b 0