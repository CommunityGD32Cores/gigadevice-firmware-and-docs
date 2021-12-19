:: =================== MBL and PROT ====================
:: Delete MBL key and cert if exist
del mbl-key.pem
del mbl-cert.pem
 
:: Generate MBL key pair and cert request
:: Private Key PEM pass phrase, Please Enter: 12345678
openssl req -newkey ED25519 -new -out mbl-req.csr

:: Save mbl-key.pem, the ED25519 private key
move privkey.pem mbl-key.pem

:: Signed by rot-cert.pem
openssl x509 -req -in mbl-req.csr -out mbl-cert.pem -signkey mbl-key.pem -CA rot-cert.pem -CAkey rot-key.pem -CAcreateserial -days 3650

:: Convert a key pem file to txt. Just for debug help.
:: openssl pkey -in mbl-key.pem -pubout -out mbl-key.txt -text

:: Delete intermediate file
del mbl-req.csr
del mbl-cert.srl
