:: ============================
:: Install python site-packages
:: ============================
:: Please make sure Python3 has been installed for all user

python -m pip install --upgrade pip -i https://pypi.tuna.tsinghua.edu.cn/simple

pip install cbor -i https://pypi.tuna.tsinghua.edu.cn/simple
pip install cryptography -i https://pypi.tuna.tsinghua.edu.cn/simple
pip install pycryptodome -i https://pypi.tuna.tsinghua.edu.cn/simple
pip install pyasn1 -i https://pypi.tuna.tsinghua.edu.cn/simple
pip install intelhex -i https://pypi.tuna.tsinghua.edu.cn/simple

:: Pip sources
:: ������ http://mirrors.aliyun.com/pypi/simple/ 
:: �й��Ƽ���ѧ https://pypi.mirrors.ustc.edu.cn/simple/ 
:: ����(douban) http://pypi.douban.com/simple/ 
:: �廪��ѧ https://pypi.tuna.tsinghua.edu.cn/simple/ 
:: �й���ѧ������ѧ http://pypi.mirrors.ustc.edu.cn/simple/

