@echo off
set TZEN=%1
set SIP=%2
set AESK=%~3
set DEBUGGER=%4
set OUTPUT_PATH=%5
set TOOLKIT=%6
set TOOLKIT_PATH=%7
set "OUTPUT_PATH=%OUTPUT_PATH:/=\%"
echo TOOLKIT=%TOOLKIT%
echo TOOLKIT_PATH=%TOOLKIT_PATH%

set ROOT=%OUTPUT_PATH%\..\..\..\..\..\..
set INDEX=0
set OUTPUT_IMAGE_PATH=%ROOT%\scripts\images
set CERT_PATH=%ROOT%\scripts\certs
set IMGTOOL=%ROOT%\scripts\imgtool\imgtool.py
set HEXTOOL=%ROOT%\scripts\imgtool\hextool.py
set SREC_CAT=%ROOT%\scripts\imgtool\srec_cat.exe
set GENTOOL=%ROOT%\scripts\imgtool\gentool.py
set BIN2ARRAY=%ROOT%\scripts\imgtool\bin2array.py
set AESTOOL=%ROOT%\scripts\imgtool\aestool.py
set DLTOOL=%ROOT%\scripts\imgtool\download.py
set KEY_PASSPHRASE="12345678"
set OUTPUT_FILE=nspe


:: Save Orignal Working Path
set WORK_PATH=%CD%

if "%TZEN%" == "TZ" (
    set CONFIG_FILE=%ROOT%\config\config_gdm32.h
    if "%SIP%" == "SIP" (
        set ALGO=GD32W51x_S
    ) else (
        set ALGO=GD32W51x_Q_S
    )
) else (
    set CONFIG_FILE=%ROOT%\config\config_gdm32_ntz.h
    if "%SIP%" == "SIP" (
        set ALGO=GD32W51x
    ) else (
        set ALGO=GD32W51x_Q
    )
)

if "%AESK%" NEQ "" (
    set AES_SUFFIX=-aes
) else (
    set AES_SUFFIX=
)

IF EXIST %OUTPUT_PATH%\..\nspe*     del %OUTPUT_PATH%\..\nspe*

if "%TOOLKIT%" == "KEIL" (
    :: Generate txt for debug
     %TOOLKIT_PATH%\ARM\ARMCC\bin\fromelf.exe --text -c -d --output=%OUTPUT_PATH%\..\%OUTPUT_FILE%.txt %OUTPUT_PATH%\%OUTPUT_FILE%.axf

    :: Generate binary image
     %TOOLKIT_PATH%\ARM\ARMCC\bin\fromelf.exe --bin --8x1 --bincombined --output=%OUTPUT_PATH%\..\nspe.bin %OUTPUT_PATH%\%OUTPUT_FILE%.axf
)
if "%TOOLKIT%" == "IAR" (
    :: Generate ASM file
    %TOOLKIT_PATH%\bin\ielfdumparm.exe %OUTPUT_PATH%\%OUTPUT_FILE%.axf  --output %OUTPUT_PATH%\..\%OUTPUT_FILE%.asm --code

    :: Generate binary image
    %TOOLKIT_PATH%\bin\ielftool.exe %OUTPUT_PATH%\%OUTPUT_FILE%.axf  --bin %OUTPUT_PATH%\..\%OUTPUT_FILE%.bin
)

if "%TOOLKIT%" == "GCC" (
    arm-none-eabi-objdump -xS  %OUTPUT_PATH%\%OUTPUT_FILE%.axf > %OUTPUT_PATH%\%OUTPUT_FILE%.txt
    arm-none-eabi-objcopy -Obinary %OUTPUT_PATH%\%OUTPUT_FILE%.axf %OUTPUT_PATH%\..\%OUTPUT_FILE%.bin
)

::echo %CONFIG_FILE%
:: Get image version
set cmd="python %IMGTOOL% printver  --config %CONFIG_FILE% -t "NSPE""
FOR /F "tokens=*" %%i IN (' %cmd% ') DO SET IMG_VERSION=%%i

IF EXIST %OUTPUT_IMAGE_PATH%\nspe-sign.bin     del %OUTPUT_IMAGE_PATH%\nspe-sign.bin

:: Add image header, ptlvs and concatenate the cert
python %IMGTOOL% sign --config %CONFIG_FILE% ^
         -k %CERT_PATH%\arot-key.pem ^
         -P %KEY_PASSPHRASE% ^
         -t "NSPE" ^
         --cert %CERT_PATH%\arot-cert.pem ^
         %OUTPUT_PATH%\..\nspe.bin ^
         %OUTPUT_IMAGE_PATH%\nspe-sign.bin
@if not exist %OUTPUT_IMAGE_PATH%\nspe-sign.bin goto error

if "%AESK%" NEQ "" (
    python %AESTOOL% -c %CONFIG_FILE% ^
        -t "IMG_%INDEX%_NSPE" ^
        -i %OUTPUT_IMAGE_PATH%\nspe-sign.bin ^
        -o %OUTPUT_IMAGE_PATH%\nspe-sign"%AES_SUFFIX%".bin ^
        -k %AESK%
    echo Encrypted!
)

:: Convert to Intel HEX for debug
python %HEXTOOL% --c %CONFIG_FILE% ^
        -t "IMG_%INDEX%_NSPE" ^
        -e %SREC_CAT% ^
        %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.bin ^
        %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.hex

:: Generate images for mass production or upgrade
del %OUTPUT_IMAGE_PATH%\image*.bin          /q
del %OUTPUT_IMAGE_PATH%\image*.hex          /q

if "%TZEN%" == "TZ" (
    :: Generate fatory image-all-x.x.x.bin
    python %GENTOOL% --config %CONFIG_FILE% ^
            --sys_set %OUTPUT_IMAGE_PATH%\mbl-sys%AES_SUFFIX%.bin ^
            --prot_0 %OUTPUT_IMAGE_PATH%\prot-sign%AES_SUFFIX%.bin ^
            --nspe_0 %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.bin ^
            -o %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.bin

    :: Generate fatory image-all-x.x.x.hex
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "SYS_SET" ^
            -e %SREC_CAT% ^
            %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.bin ^
            %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.hex

    :: Generate upgrade image-x.x.x.bin
    python %GENTOOL% --config %CONFIG_FILE% ^
            --prot_0 %OUTPUT_IMAGE_PATH%\prot-sign%AES_SUFFIX%.bin ^
            --nspe_0 %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.bin ^
            -o %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.bin

    :: Generate upgrade image-x.x.x.hex
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "IMG_"%INDEX%"_PROT" ^
            -e %SREC_CAT% ^
            %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.bin ^
            %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.hex
) else (
    :: Generate fatory image-all-x.x.x.bin
    python %GENTOOL% --config %CONFIG_FILE% ^
            --mbl %OUTPUT_IMAGE_PATH%\mbl%AES_SUFFIX%-ns.bin ^
            --nspe_0 %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.bin ^
            -o %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.bin

    :: Generate fatory image-all-x.x.x.hex
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "MBL" ^
            -e %SREC_CAT% ^
            %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.bin ^
            %OUTPUT_IMAGE_PATH%\image-all-%IMG_VERSION%%AES_SUFFIX%.hex

    :: Generate upgrade image-x.x.x.bin
    python %GENTOOL% --config %CONFIG_FILE% ^
            --nspe_0 %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.bin ^
            -o %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.bin

    :: Generate upgrade image-x.x.x.hex
    python %HEXTOOL% -c %CONFIG_FILE% ^
            -t "IMG_"%INDEX%"_NSPE" ^
            -e %SREC_CAT% ^
            -ns 1 ^
            %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.bin ^
            %OUTPUT_IMAGE_PATH%\image-%IMG_VERSION%%AES_SUFFIX%.hex
)

:: Download to FLASH
cd %ROOT%
if "%DEBUGGER%" == "DAPLINK" (
     if not exist scripts\venv\ exit 2
)
python %DLTOOL% -a %ALGO% -f %OUTPUT_IMAGE_PATH%\nspe-sign%AES_SUFFIX%.hex --debugger %DEBUGGER%
cd %WORK_PATH%
:error
::exit
