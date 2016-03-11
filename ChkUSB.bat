@ECHO OFF

:START

cd /d %~dp0

echo run wusb_scan.exe /t %1 -scan %2 >chkusb.txt
wusb_scan.exe /t %1 -scan %2 >>chkusb.txt
IF ERRORLEVEL 1 GOTO Check_FAIL
goto Check_Pass


:Check_FAIL
echo check fail >>chkusb.txt
echo Check USB Device Fail >>DevChkError.txt
exit /b 255

:Check_Pass
echo check pass >>chkusb.txt
exit /b 0