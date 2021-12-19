@echo on
set TZ=%1
set PROJ_DIR=%2
set TOOLKIT_DIR=%3
set ROOT_DIR=%PROJ_DIR%\..\..\..\..

echo TOOLKIT_DIR=%TOOLKIT_DIR%
@echo off

:: Create build date
cmd /c %PROJ_DIR%\..\build_date.bat  %ROOT_DIR%\NSPE\

if "%TZ%" == "TZ" (
    set config_TZ=-D CONFIG_TZ_ENABLED
) else (
    set config_TZ=
)

:: Preprocess to generate ICF file
set IFILE=%ROOT_DIR%\NSPE\Firmware\CMSIS\GD\GD32W51x\Source\IAR\nspe.i
set OFILE=%PROJ_DIR%\nspe.icf
if exist %OFILE%  del %OFILE%

%TOOLKIT_DIR%\bin\iccarm.exe %IFILE% ^
                             --preprocess=sn ^
                             %OFILE% ^
                             -I %ROOT_DIR%\NSPE\WIFI_IOT\app ^
                             -I %ROOT_DIR%\config %config_TZ%

:: Create extern symbols for config_gdm32*.h
:: echo /* config_gdm32*.h symbol list */ > %PROJ_DIR%\config_gdm32.icf
:: for /f "eol=/ tokens=1,2,3" %%i in (%ROOT_DIR%\config\%CONFIG_FILE%.h ) do echo define exported symbol %%j = %%k; >> %PROJ_DIR%\config_gdm32.icf
:: for /f "skip=38 eol=/ tokens=1,2,3" %%i in (%PROJ_DIR%\..\..\..\WIFI_IOT\app\app_cfg.h ) do (
::     if %%j == CONFIG_WIFI_HIGH_PERFORMANCE (
::         echo define symbol CONFIG_WIFI_HIGH_PERFORMANCE=1; >> %PROJ_DIR%\config_gdm32.icf 
::     )
:: )