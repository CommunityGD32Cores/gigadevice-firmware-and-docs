:: =================== AROT and NSPE ====================
:: Generate AROT key pair and cert request
:: Private Key PEM pass phrase, Please Enter: 12345678
openssl req -newkey ED25519 -new -out arot-req.csr

:: Save arot-key.pem, the ED25519 private key
move privkey.pem arot-key.pem

:: Signed by mbl-cert.pem
openssl x509 -req -in arot-req.csr -out arot-cert.pem -signkey arot-key.pem -CA mbl-cert.pem -CAkey mbl-key.pem -CAcreateserial -days 3650

:: Convert a key pem file to txt. Just for debug help.
:: openssl pkey -in arot-key.pem -pubout -out arot-key.txt -text

:: Delete intermediate file
del arot-req.csr
del arot-cert.srl
