@echo off
set SIP=%1
set AESK=%~2
set DEBUGGER=%3
set OUTPUT_PATH=%4
set OUTPUT_NAME=%5
set TOOLKIT=%6
set TOOLKIT_PATH=%7

set INDEX=0

if "%TOOLKIT%" == "KEIL" (
    set ROOT=%4\..\..\..\..\..\..\
) 
if "%TOOLKIT%" == "IAR" (
    set ROOT=%4\..\..\..\..\..\
)
set KEY_PASSPHRASE="12345678"
set CERT_PATH=%ROOT%\scripts\certs\
set CONFIG_FILE=%ROOT%\config\config_gdm32.h
set IMGTOOL=%ROOT%\scripts\imgtool\imgtool.py
set HEXTOOL=%ROOT%\scripts\imgtool\hextool.py
set AESTOOL=%ROOT%\scripts\imgtool\aestool.py
set SREC_CAT=%ROOT%\scripts\imgtool\srec_cat.exe
set BIN2ARRAY=%ROOT%\scripts\imgtool\bin2array.py
set DLTOOL=%ROOT%\scripts\imgtool\download.py
set OUTPUT_IMAGE_PATH=%ROOT%\scripts\images\
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

if exist %OUTPUT_IMAGE_PATH%\prot-sign.bin  del %OUTPUT_IMAGE_PATH%\prot-sign.bin
:: Add image header, ptlvs and concatenate the cert
python %IMGTOOL% sign --config %CONFIG_FILE% ^
                                         -k %CERT_PATH%\mbl-key.pem ^
                                         -P %KEY_PASSPHRASE% ^
                                         -t "PROT"  ^
                                         %OUTPUT_FILE%.bin ^
                                         %OUTPUT_IMAGE_PATH%\prot-sign.bin
if not exist %OUTPUT_IMAGE_PATH%\prot-sign.bin exit

if "%AESK%" == "" (
    :: Convert to Intel HEX for debug
    python %HEXTOOL%  -c %CONFIG_FILE% ^
                      -t "IMG_%INDEX%_PROT" ^
                      -e %SREC_CAT% ^
                      %OUTPUT_IMAGE_PATH%\prot-sign.bin ^
                      %OUTPUT_IMAGE_PATH%\prot-sign.hex

) else (
    python %AESTOOL%  -c %CONFIG_FILE% ^
                      -t "IMG_%INDEX%_PROT" ^
                      -i %OUTPUT_IMAGE_PATH%\prot-sign.bin ^
                      -o %OUTPUT_IMAGE_PATH%\prot-sign-aes.bin ^
                      -k %AESK%
    :: Convert to Intel HEX for debug
    python %HEXTOOL%  -c %CONFIG_FILE% ^
                      -t "IMG_%INDEX%_PROT" ^
                      -e %SREC_CAT% ^
                      %OUTPUT_IMAGE_PATH%\prot-sign-aes.bin ^
                      %OUTPUT_IMAGE_PATH%\prot-sign-aes.hex
    echo Encrypted!
)

if "%TOOLKIT%" == "KEIL" (
    copy %OUTPUT_IMAGE_PATH%\prot-sign%AES_SUFFIX%.hex  %OUTPUT_PATH%\%OUTPUT_NAME%.hex
)

:: Download to FLASH
cd %ROOT%
python %DLTOOL% -a %ALGO% -f %OUTPUT_IMAGE_PATH%\prot-sign%AES_SUFFIX%.hex --debugger %DEBUGGER%
cd %WORK_PATH%

copy %OUTPUT_PATH%\s_veneers.o %ROOT%\NSPE\WIFI_IOT\lib\%TOOLKIT%\mbedtls_s_veneers.o

:: Convert binary to uint8 array
:: python %BIN2ARRAY% -name prot_bin %OUTPUT_IMAGE_PATH%\prot-sign.bin ..\..\..\source\prot_bin.c


::exit
