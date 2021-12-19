@ECHO off
set DIR=%1

:: Changes the active console code page to US English
chcp 437

echo /* Do not change the content here, it's auto generated */ > %DIR%\WIFI_IOT\common\_build_date.h

REM build date
echo #define BUILD_DATE "%DATE:~-10% %TIME:~0,-3%" >> %DIR%\WIFI_IOT\common\_build_date.h

echo #define DATE_GENERATED >> %DIR%\WIFI_IOT\common\_build_date.h
:end
