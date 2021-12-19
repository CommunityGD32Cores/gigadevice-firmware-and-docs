#!/usr/bin/env python
#
# AES cryptography algorithm implementation in Counter mode
#


from Crypto.Cipher import AES
from Crypto.Util import Counter
import sys, argparse
import codecs

VERSION = "aes-ctr.py 0.1"

def main():
    cmd = argparse.ArgumentParser(
        prog="aes-ctr.py",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description = '''
        \rAES implementation in counter mode. This version supports 128 bits key encryption only.
        ''',
        epilog = '''
        \rExemplary usage:\n
        \r1) Encryption
        \r$ python aes-ctr.py -i plaintext -o ciphertext -k abcdef1234567890abcdef1234567890 -s 0x08000000\n
        \r2) Decryption
        \r$ aes-ctr.py -d -i ciphertext -o plaintext -k abcdef1234567890abcdef1234567890 -s 0x08000000
        ''')

    cmd.add_argument("-d", "--decrypt", help="Use decrypt instead of default encrypt", action="store_true")
    cmd.add_argument("-i", "--input", help="File containing plaintext/ciphertext", type=str, required=True, metavar="IN")
    cmd.add_argument("-o", "--output", help="Output file to store result of the program", type=str, required=True, metavar="OUT")
    cmd.add_argument("-k", "--key", help="Encryption 128bits key", type=str, required=True)
    cmd.add_argument("-s", help="The start address of firmware for initial 128 bits counter", type=str, required=True)
    cmd.add_argument("-v", "--version", action="version", version=VERSION)
    args = cmd.parse_args()


    out_file = args.output


    in_file = args.input

    key = validateHex(args.key)

    try:
        startAddress = int(args.s, 16)
    except ValueError:
        print("Invalid Start address")
        return
    # Get the Address[23:4]
    startAddress = (startAddress & 0xffffff) >> 4

    iv = '{:0>32x}'.format(startAddress)
    iv = validateHex(iv)


    if key and iv:
        if args.decrypt:
            crypto(in_file, key, iv, out_file, encrypt=False)
        else:
            crypto(in_file, key, iv, out_file)
    else:
        print("Invalid Key or iv")

'''
 Validate if passed value is hexadecimal and has proper length
 Function returns passed argument if value is correct
 If passed value is not valid, function returns False
'''
def validateHex(hex):
    if len(hex) is not 32:
        return False
    else:
        try:
            int(hex, 16)
            return hex
        except ValueError:
            return False
'''
 Core function that performs encryption / decryption
'''
def crypto(input, key, iv, output, encrypt=True):
    blockSize = 16
    encryptionKey = codecs.decode(key, 'hex')

    # Create new Counter object #
    # Object will automatically increment counter on each cryptographic round #
    counter = Counter.new(128, initial_value=int(iv, 16))
    cipher = AES.new(encryptionKey, AES.MODE_CTR, counter=counter)

    result = b''

    with open(input, 'rb') as fin, open(output, 'wb') as fout:
        while True:
            # AES CTR operates on 16 bytes blocks
            ret = fin.read(blockSize)
            if not ret:
                break
            if encrypt:
                result = cipher.encrypt(ret)
            else:
                result = cipher.decrypt(ret)
            fout.write(result)

if __name__ == '__main__':
    main()