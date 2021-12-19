:: =====================================
:: setup Jlink auto download enviroment
:: =====================================
@echo off
set INPUT_PATH=c:\Program Files (x86)\SEGGER\JLink
IF NOT EXIST "%INPUT_PATH%" (
    echo Please enter your JLINK path: 
    echo "default: c:\Program Files (x86)\SEGGER\JLink"
    set /p INPUT_PATH=
)
::echo INPUT_PATH=%INPUT_PATH%
for %%x in ("%INPUT_PATH%") do set JLINK_PATH=%%~sx
IF NOT EXIST %JLINK_PATH% (
    echo JLINK path not exist
    pause
    goto end
)

set DEV_PATH=%JLINK_PATH%\Devices\GigaDevice
::echo DEV_PATH=%DEV_PATH%
IF NOT EXIST %DEV_PATH%  mkdir %DEV_PATH%

::echo current path:%cd%
::echo current disk:%~d0
::echo current bat path: %~dp0
::echo current bat short path: %~sdp0

%~d0
cd %~sdp0
copy flms\*.FLM %DEV_PATH%   /Y
IF EXIST %JLINK_PATH%\JLinkDevices.xml rename %JLINK_PATH%\JLinkDevices.xml JLinkDevices_bk.xml
copy flms\JLinkDevices.xml %JLINK_PATH%\     /Y

pause
:end
