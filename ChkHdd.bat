@ECHO OFF

:START

cd /d %~dp0
set /a HDD_NUM = %1 + %2
echo run whdd /list -count %HDD_NUM% >>chkhdd.txt 
whdd /list -count %HDD_NUM% >chkhdd.txt
IF ERRORLEVEL 1 GOTO Check_FAIL
goto Check_Pass


:Check_FAIL
echo check fail >>chkhdd.txt
echo Check HDD Device Fail >>DevChkError.txt
exit /b 255

:Check_Pass
echo check pass >>chkhdd.txt
exit /b 0