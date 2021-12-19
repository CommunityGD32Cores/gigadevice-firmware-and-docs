@echo off
set SIP=%1
set AESK=%~2
set DEBUGGER=%3
set OUTPUT_PATH=%4
set OUTPUT_NAME=%5
set TOOLKIT=%6
set TOOLKIT_DIR=%7

set "OUTPUT_PATH=%OUTPUT_PATH:/=\%"

set ROOT=%OUTPUT_PATH%\..\..\..\..\..\
set KEY_PASSPHRASE="12345678"
set MBL_KEY=%ROOT%\scripts\certs\mbl-key.pem
set ROTPK=%ROOT%\scripts\certs\rot-key.pem
set MBL_CERT=%ROOT%\scripts\certs\mbl-cert.pem
set CONFIG_FILE=%ROOT%\config\config_gdm32.h
set SYSTOOL=%ROOT%\scripts\imgtool\sysset.py
set IMGTOOL=%ROOT%\scripts\imgtool\imgtool.py
set HEXTOOL=%ROOT%\scripts\imgtool\hextool.py
set GENTOOL=%ROOT%\scripts\imgtool\gentool.py
set AESTOOL=%ROOT%\scripts\imgtool\aestool.py
set SREC_CAT=%ROOT%\scripts\imgtool\srec_cat.exe
set DLTOOL=%ROOT%\scripts\imgtool\download.py
set OUTPUT_HEX_PATH=%ROOT%\scripts\images\
set OUTPUT_FILE=%OUTPUT_PATH%\..\%OUTPUT_NAME%

:: Save Orignal Working Path
set WORK_PATH=%CD%

if "%SIP%" == "SIP" (
    set ALGO=GD32W51x_S
) else (
    set ALGO=GD32W51x_Q_S
)

if "%AESK%" NEQ "" (
    set AES_SUFFIX=-aes
) else (
    set AES_SUFFIX=
)

:: Generate system setting hex
python %SYSTOOL% -t "SYS_SET" -c %CONFIG_FILE%  %OUTPUT_HEX_PATH%\sysset.bin

:: Generate system status hex (padding with 0xFF)
:: python %SYSTOOL% -t "SYS_STATUS" -c %CONFIG_FILE%  %OUTPUT_HEX_PATH%\sysstatus.bin

if "%TOOLKIT%" == "KEIL" (
    :: Generate txt for debug
    %TOOLKIT_DIR%\ARM\ARMCC\bin\fromelf.exe --text -c -d --output=%OUTPUT_FILE%.txt %OUTPUT_PATH%\%OUTPUT_NAME%.axf

    :: Generate binary image
    %TOOLKIT_DIR%\ARM\ARMCC\bin\fromelf.exe --bin --8x1  --bincombined --output=%OUTPUT_FILE%.bin %OUTPUT_PATH%\%OUTPUT_NAME%.axf
)
if "%TOOLKIT%" == "IAR" (
    :: Generate ASM file
    %TOOLKIT_DIR%\bin\ielfdumparm.exe %OUTPUT_PATH%\%OUTPUT_NAME%.axf  --output %OUTPUT_FILE%.asm --code

    :: Generate binary image
    %TOOLKIT_DIR%\bin\ielftool.exe %OUTPUT_PATH%\%OUTPUT_NAME%.axf  --bin %OUTPUT_FILE%.bin
)

if "%TOOLKIT%" == "GCC" (
    arm-none-eabi-objdump -xS  %OUTPUT_PATH%/%OUTPUT_NAME%.axf > %OUTPUT_FILE%.txt
    arm-none-eabi-objcopy -Obinary %OUTPUT_PATH%/%OUTPUT_NAME%.axf %OUTPUT_FILE%.bin
)

if exist %OUTPUT_HEX_PATH%\mbl-sign.bin  del %OUTPUT_HEX_PATH%\mbl-sign.bin
:: Add image header, ptlvs and concatenate the cert
python %IMGTOOL% sign --config %CONFIG_FILE% ^
                      -k %MBL_KEY% ^
                      -P %KEY_PASSPHRASE% ^
                      -t "MBL" ^
                      --cert %MBL_CERT% ^
                      %OUTPUT_FILE%.bin %OUTPUT_HEX_PATH%\mbl-sign.bin

python %GENTOOL% --config %CONFIG_FILE% ^
                 --sys_set %OUTPUT_HEX_PATH%\sysset.bin ^
                 --mbl %OUTPUT_HEX_PATH%\mbl-sign.bin ^
                 -o %OUTPUT_HEX_PATH%\mbl-sys.bin
IF EXIST %OUTPUT_HEX_PATH%\sysset.bin del %OUTPUT_HEX_PATH%\sysset.bin
IF EXIST %OUTPUT_HEX_PATH%\mbl-sign.bin del %OUTPUT_HEX_PATH%\mbl-sign.bin

if "%AESK%" == ""  (
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "SYS_SET" ^
            -e %SREC_CAT% ^
            %OUTPUT_HEX_PATH%\mbl-sys.bin ^
            %OUTPUT_HEX_PATH%\mbl-sys.hex
)  else (
    python %IMGTOOL% pad -s 0x8000 ^
                         %OUTPUT_HEX_PATH%\mbl-sys.bin %OUTPUT_HEX_PATH%\mbl-sys-pad.bin
    python %AESTOOL% --c %CONFIG_FILE%   ^
            -t "SYS_SET" ^
            -i %OUTPUT_HEX_PATH%\mbl-sys-pad.bin ^
            -o %OUTPUT_HEX_PATH%\mbl-sys%AES_SUFFIX%.bin ^
            -k %AESK%
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "SYS_SET" ^
            -e %SREC_CAT% ^
            %OUTPUT_HEX_PATH%\mbl-sys%AES_SUFFIX%.bin ^
            %OUTPUT_HEX_PATH%\mbl-sys.hex
    del %OUTPUT_HEX_PATH%\mbl-sys-pad.bin
    echo Encrypted!
)

copy %OUTPUT_HEX_PATH%\mbl-sys.hex %OUTPUT_PATH%\mbl.hex
copy %OUTPUT_PATH%\mbl_cmse_Lib.o %ROOT%\NSPE\WIFI_IOT\lib\%TOOLKIT%

cd %ROOT%
python %DLTOOL%  -a %ALGO% -f %OUTPUT_HEX_PATH%\mbl-sys.hex  --debugger %DEBUGGER%
cd %WORK_PATH%
::exit
