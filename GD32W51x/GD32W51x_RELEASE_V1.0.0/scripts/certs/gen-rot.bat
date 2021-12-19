:: =================== ROOT ====================
:: Generate ROT key pair and cert request
:: Private Key PEM pass phrase, Please Enter: 12345678
::openssl req -newkey ED25519 -new -out rot-req.csr
openssl req -key rot-key.pem -new -out rot-req.csr

:: Save rot-key.pem, the ED25519 private key
move privkey.pem rot-key.pem

:: Self signed cert. The rot-cert.pem will be used to sign MBL cert.
openssl x509 -req -in rot-req.csr -signkey rot-key.pem -out rot-cert.pem -days 3650

:: Convert a key pem file to txt. You can copy the public key to ROTPK[].
openssl pkey -in rot-key.pem -pubout -out rot-key.txt -text

:: Delete intermediate file
del rot-req.csr
del rot-cert.srl

:: You can check the cert info use the following command.
openssl x509 -in rot-cert.pem -out rot-cert.txt -text


