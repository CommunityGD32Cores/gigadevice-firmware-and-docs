@echo off
:: Preprocess to generate ICF file
set PROJ_DIR=%1
set TOOLKIT_DIR=%2

set IFILE=%PROJ_DIR%\..\..\trusted-firmware-m\platform\ext\common\iar\tfm_common_s.i
set OFILE=%PROJ_DIR%\tfm_common_s.icf

IF EXIST %OFILE%  del %OFILE%
%TOOLKIT_DIR%\bin\iccarm.exe %IFILE% ^
                             --preprocess=sn ^
                             %OFILE% ^
                             -I %PROJ_DIR%\..\..\trusted-firmware-m\platform\ext\target\gdm32\gd32w51x\partition ^
                             -I %PROJ_DIR%\..\..\..\config ^
                             -DTFM_LVL=1 ^
                             -DGD_MODIFY_TAG ^
                             -DPLATFORM_GDM32