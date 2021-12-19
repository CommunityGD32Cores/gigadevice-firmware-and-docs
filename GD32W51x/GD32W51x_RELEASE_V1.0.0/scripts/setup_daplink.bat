:: =========================================
:: setup DAPLink auto download enviroment
:: =========================================
@echo off

@set targetFile=venv\Lib\site-packages\pyocd\target\builtin\target_gd32e101.py

@if not exist %USERPROFILE%\pip md %USERPROFILE%\pip
@if not exist %USERPROFILE%\pip\pip.ini (
echo [global] > %USERPROFILE%\pip\pip.ini
echo index-url = https://pypi.tuna.tsinghua.edu.cn/simple >> %USERPROFILE%\pip\pip.ini
)

pip install virtualenv==20.2.2 -i https://pypi.tuna.tsinghua.edu.cn/simple

@set env_exists=0
@if exist venv set env_exists=1
@if [%env_exists%]==[0] echo Creating python virtual environment && virtualenv venv
call venv\Scripts\activate

@echo Doing pip install
@REM use project requirements if not specified
if [%1]==[] pip install pyocd==0.28.2 poetry
@REM use custom requirements if specified
if not [%1]==[] pip install -r %1

IF EXIST %targetFile% del %targetFile%
python patch/apply.py
if NOT %ERRORLEVEL%==0 pause
