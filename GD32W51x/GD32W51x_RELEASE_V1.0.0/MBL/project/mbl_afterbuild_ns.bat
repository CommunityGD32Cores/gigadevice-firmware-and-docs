@echo off
set SIP=%1
set AESK=%~2
set DEBUGGER=%3
set OUTPUT_PATH=%4
set OUTPUT_NAME=%5
set TOOLKIT=%6
set TOOLKIT_PATH=%7

set "OUTPUT_PATH=%OUTPUT_PATH:/=\%"

set ROOT=%OUTPUT_PATH%\..\..\..\..\..
set IMGTOOL=%ROOT%\scripts\imgtool\imgtool.py
set AESTOOL=%ROOT%\scripts\imgtool\aestool.py
set SREC_CAT=%ROOT%\scripts\imgtool\srec_cat.exe
set DLTOOL=%ROOT%\scripts\imgtool\download.py
set CONFIG_FILE=%ROOT%\config\config_gdm32_ntz.h
set OUTPUT_HEX_PATH=%ROOT%\scripts\images
set OUTPUT_FILE=%OUTPUT_PATH%\..\%OUTPUT_NAME%

:: Save Orignal Working Path
set WORK_PATH=%CD%

if "%SIP%" == "SIP" (
    set ALGO=GD32W51x
) else (
    set ALGO=GD32W51x_Q
)

if "%AESK%" NEQ "" (
    set AES_SUFFIX=-aes
) else (
    set AES_SUFFIX=
)

if "%TOOLKIT%" == "KEIL" (
    :: Generate txt for debug
    %TOOLKIT_PATH%\ARM\ARMCC\bin\fromelf.exe --text -c -d --output=%OUTPUT_FILE%.txt %OUTPUT_PATH%\%OUTPUT_NAME%.axf

    :: Generate binary image
    %TOOLKIT_PATH%\ARM\ARMCC\bin\fromelf.exe --bin --8x1  --bincombined --output=%OUTPUT_FILE%.bin %OUTPUT_PATH%\%OUTPUT_NAME%.axf
)
if "%TOOLKIT%" == "IAR" (
    :: Generate ASM file
    %TOOLKIT_PATH%\bin\ielfdumparm.exe %OUTPUT_PATH%\%OUTPUT_NAME%.axf  --output %OUTPUT_FILE%.asm --code

    :: Generate binary image
    %TOOLKIT_PATH%\bin\ielftool.exe %OUTPUT_PATH%\%OUTPUT_NAME%.axf  --bin %OUTPUT_FILE%.bin
)

if "%TOOLKIT%" == "GCC" (
    arm-none-eabi-objdump -xS  %OUTPUT_PATH%/%OUTPUT_NAME%.axf > %OUTPUT_FILE%.txt
    arm-none-eabi-objcopy -Obinary %OUTPUT_PATH%/%OUTPUT_NAME%.axf %OUTPUT_FILE%.bin
)

IF EXIST %OUTPUT_HEX_PATH%\mbl_ns* del %OUTPUT_HEX_PATH%\mbl_ns*

if "%AESK%" == "" (
    %SREC_CAT%  %OUTPUT_FILE%.bin -Binary -offset 0x08000000 -o %OUTPUT_FILE%.hex -Intel
) else (
    :: Pad mbl.bin to 32K with 0xFF
    python %IMGTOOL% pad -s 0x8000 %OUTPUT_FILE%.bin %OUTPUT_FILE%-pad.bin

    :: Encrypt MBL with AES Key
    python %AESTOOL% --c %CONFIG_FILE%   ^
        -t "MBL" ^
        -i %OUTPUT_FILE%-pad.bin ^
        -o %OUTPUT_FILE%-aes.bin ^
        -k %AESK%

    :: Convert to HEX file
    %SREC_CAT%  %OUTPUT_FILE%-aes.bin -Binary -offset 0x08000000 -o %OUTPUT_FILE%-aes.hex -Intel

    IF EXIST %OUTPUT_FILE%-pad.bin del %OUTPUT_FILE%-pad.bin
    echo AES encrypted!
)

:: Copy to output hex path
copy %OUTPUT_FILE%%AES_SUFFIX%.bin %OUTPUT_HEX_PATH%
copy %OUTPUT_FILE%%AES_SUFFIX%.hex %OUTPUT_HEX_PATH%

:: Download to FLASH
cd %ROOT%
python %DLTOOL% -a %ALGO% -f %OUTPUT_FILE%%AES_SUFFIX%.hex --debugger %DEBUGGER%
cd %WORK_PATH%

::exit
