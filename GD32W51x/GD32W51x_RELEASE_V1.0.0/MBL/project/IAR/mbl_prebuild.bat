@echo off
:: Preprocess to generate ICF file
set TZ=%1
set PROJ_DIR=%2
set TOOLKIT_DIR=%3

if "%TZ%" == "TZ" (
    set IFILE=%PROJ_DIR%\..\..\platform\gdm32\iar\mbl_gdm32.i
    set OFILE=%PROJ_DIR%\mbl_gdm32.icf
    set CMSE=-D__ARM_FEATURE_CMSE=3
) else (
    set IFILE=%PROJ_DIR%\..\..\platform\gdm32\iar\mbl_gdm32_ns.i
    set OFILE=%PROJ_DIR%\mbl_gdm32_ns.icf
    set CMSE=
)


IF EXIST %OFILE%  del %OFILE%
%TOOLKIT_DIR%\bin\iccarm.exe %IFILE% ^
                             --preprocess=sn ^
                             %OFILE% ^
                             -I %PROJ_DIR%\..\..\..\config ^
                             -I %PROJ_DIR%\..\..\platform %CMSE%

:: set CONFIG_FILE=%1
:: echo /* config_gdm32*.h symbol list */ > %PROJ_DIR%\..\..\platform\gdm32\iar\%CONFIG_FILE%.icf
:: for /f "eol=/ tokens=1,2,3" %%i in (%PROJ_DIR%\..\..\..\..\config\%CONFIG_FILE%.h ) 
::      do echo define exported symbol %%j = %%k; >> %PROJ_DIR%\..\..\platform\gdm32\iar\%CONFIG_FILE%.icf
