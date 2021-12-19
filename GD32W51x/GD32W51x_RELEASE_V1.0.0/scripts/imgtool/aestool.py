
#
# AES cryptography tool
#

import sys, argparse
import codecs
import re
import macro_parser
from imglib.keys.aes_ctr import crypto

VERSION = "aestool.py 0.1"

offset_re = re.compile(r"^.*\s*RE_([0-9A-Z_]+)_OFFSET\s*(.*)")

def main():
    cmd = argparse.ArgumentParser(
        prog="aestool.py",
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

    cmd.add_argument('-c', '--config', metavar='filename', required=True,
                          help='Location of the file that contains macros')
    cmd.add_argument('-t', '--type', metavar='type', required=True,
                          help='SYS_SET / MBL / SYS_STATUS / IMG_0_PROT / IMG_0_AROT / IMG_0_NSPE / IMG_1_PROT / IMG_1_AROT / IMG_1_NSPE')
    cmd.add_argument("-d", "--decrypt", help="Use decrypt instead of default encrypt", action="store_true")
    cmd.add_argument("-i", "--input", help="File containing plaintext/ciphertext", type=str, required=True, metavar="IN")
    cmd.add_argument("-o", "--output", help="Output file to store result of the program", type=str, required=True, metavar="OUT")
    cmd.add_argument("-k", "--key", help="Encryption 128bits key", type=str, required=True)
    cmd.add_argument("-v", "--version", action="version", version=VERSION)
    args = cmd.parse_args()

    offsets = macro_parser.evaluate_macro(args.config, offset_re, 1, 2)

    out_file = args.output


    in_file = args.input

    key = validateHex(args.key)

    try:
        startAddress = offsets[args.type]
        print("startAddress = ", startAddress)
    except KeyError:
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
    if len(hex) != 32:
        return False
    else:
        try:
            int(hex, 16)
            return hex
        except ValueError:
            return False

if __name__ == '__main__':
    main()