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
:: 阿里云 http://mirrors.aliyun.com/pypi/simple/ 
:: 中国科技大学 https://pypi.mirrors.ustc.edu.cn/simple/ 
:: 豆瓣(douban) http://pypi.douban.com/simple/ 
:: 清华大学 https://pypi.tuna.tsinghua.edu.cn/simple/ 
:: 中国科学技术大学 http://pypi.mirrors.ustc.edu.cn/simple/

