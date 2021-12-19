#! /usr/bin/env python3
#
# Copyright 2017 Linaro Limited
# Copyright (c) 2018-2019, Arm Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function
import os
import re
import argparse
from imglib import keys
from imglib import image
from imglib import version
import sys
import macro_parser

vtor_align_re = re.compile(r"^.*\s*RE_VTOR_ALIGNMENT\s*(.*)")
version_re = re.compile(r"^.*\s*RE_([0-9A-Z_]+)_VERSION\s*(.*)")

# Image Type.
IMG_TYPE_VALUES = {
        'MBL' : 0x1,
        'PROT': 0x2,
        'AROT': 0x4, 
        'NSPE': 0x8,}
        
# Returns the last version number if present, or None if not
def get_last_version(path):
    if (os.path.isfile(path) == False): # Version file not present
        return None
    else: # Version file is present, check it has a valid number inside it
        with open(path, "r") as oldFile:
            fileContents = oldFile.read()
            if version.version_re.match(fileContents): # number is valid
                return version.decode_version(fileContents)
            else:
                return None

def next_version_number(args, defaultVersion, path):
    newVersion = None
    if (version.compare(args.version, defaultVersion) == 0): # Default version
        lastVersion = get_last_version(path)
        if (lastVersion is not None):
            newVersion = version.increment_build_num(lastVersion)
        else:
            newVersion = version.increment_build_num(defaultVersion)
    else: # Version number has been explicitly provided (not using the default)
        newVersion = args.version
    versionString = "{a}.{b}.{c}+{d}".format(
                    a=str(newVersion.major),
                    b=str(newVersion.minor),
                    c=str(newVersion.revision),
                    d=str(newVersion.build)
    )
    with open(path, "w") as newFile:
        newFile.write(versionString)
    print("**[INFO]** Image version number set to " + versionString)
    return newVersion

def gen_rsa2048(args):
    keys.RSAutil.generate().export_private(args.key)

def gen_rsa3072(args):
    keys.RSAutil.generate(key_size=3072).export_private(args.key)

def gen_ecdsa_p256(args):
    keys.ECDSA256P1.generate().export_private(args.key)

def gen_ed25519(keyfile, passwd):
    keys.Ed25519.generate().export_private(path=keyfile, passwd=passwd)

keygens = {
        'rsa-2048': gen_rsa2048,
        'rsa-3072': gen_rsa3072, 
        'ecdsa-p256': gen_ecdsa_p256,
        'ed25519': gen_ed25519,}

def do_keygen(args):
    if args.type not in keygens:
        msg = "Unexpected key type: {}".format(args.type)
        raise argparse.ArgumentTypeError(msg)
    keygens[args.type](args)

def do_getpub(args):
    key = keys.load(args.key, args.password.encode('ascii'))
    print("key", key)
    if args.lang == 'c':
        key.emit_c()
    else:
        msg = "Unsupported language, valid are: c"
        raise argparse.ArgumentTypeError(msg)

def do_sign(args):
    #version_num = next_version_number(args,
    #                                  version.decode_version("0"),
    #                                  "lastVerNum.txt")
    #version_int = ((version_num.major << 24)
    #                             + (version_num.minor << 16)
    #                             + version_num.revision)

    vtor_align = macro_parser.evaluate_macro(args.config, vtor_align_re, 0, 1)

    versions = macro_parser.evaluate_macro(args.config, version_re, 1, 2)
    version_num = versions[args.type]
    
    img = image.Image.load(args.infile,
                           type=args.type,
                           version=version_num,
                           header_size=vtor_align)
#                           pad=args.padto or 0)
    if args.password:
        pwd = args.password.encode('ascii')
    key = keys.load(args.key, pwd) if args.key else None
    img.sign(key, args.cert)
    #if args.pad:
    #    img.pad_to(padto, 1)

    img.save(args.outfile)

def do_pad(args):
    with open(args.infile, 'rb') as f:
        payload = f.read()
    size = int(args.size, 16)
    padding = size - len(payload)
    if padding < 0:
        msg = "Image size (0x{:x}) exceeds requested size 0x{:x}".format(
                    len(payload), size)
        raise Exception(msg)
    pbytes  = b'\xff' * padding
    payload += pbytes

    with open(args.outfile, 'wb') as f:
        f.write(payload)

def do_print_version(args):
    versions = macro_parser.evaluate_macro(args.config, version_re, 1, 2)
    version_num = versions[args.type]
    image_version = (str((version_num >> 24) & 0xff) + '.'
                        + str((version_num >> 16) & 0xff) + '.'
                        + str(version_num & 0xffff))
    print(image_version)

subcmds = {
        'keygen': do_keygen,
        'getpub': do_getpub,
        'sign': do_sign, 
        'pad': do_pad, 
        'printver': do_print_version,
        }

def alignment_value(text):
    value = int(text)
    if value not in [1, 2, 4, 8]:
        msg = "{} must be one of 1, 2, 4 or 8".format(value)
        raise argparse.ArgumentTypeError(msg)
    return value

def intparse(text):
    """Parse a command line argument as an integer.

    Accepts 0x and other prefixes to allow other bases to be used."""
    return int(text, 0)

def args():
    parser = argparse.ArgumentParser()
    subs = parser.add_subparsers(help='subcommand help', dest='subcmd')

    keygenp = subs.add_parser('keygen', help='Generate pub/private keypair')
    keygenp.add_argument('-k', '--key', metavar='filename', required=True)
    keygenp.add_argument('-t', '--type', metavar='type',
                         choices=keygens.keys(), required=True)

    getpub = subs.add_parser('getpub', help='Get public key from keypair')
    getpub.add_argument('-k', '--key', metavar='filename', required=True)
    getpub.add_argument('-l', '--lang', metavar='lang', default='c')
    getpub.add_argument('-P', '--password', metavar='password')

    sign = subs.add_parser('sign', help='Sign an image with a private key')
    sign.add_argument("-t", "--type", metavar='type',
                      choices=IMG_TYPE_VALUES.keys(), required=True)
    sign.add_argument('--config', metavar='filename', required=True,
                          help='Location of the file that contains macros')
    sign.add_argument('-k', '--key', metavar='filename')
    sign.add_argument('-P', '--password', metavar='password')
    #sign.add_argument('--pad', type=int)
    #sign.add_argument("--align", type=alignment_value, required=True)
    #sign.add_argument("-v", "--version", type=version.decode_version,
    #                  default="0.0.0+0")
    #sign.add_argument("-H", "--header-size", type=intparse, default=32)
    sign.add_argument('-c', "--cert", metavar='certfile')
    sign.add_argument("infile")
    sign.add_argument("outfile")
    
    pad = subs.add_parser('pad', help='padding with 0xFF')
    pad.add_argument('-s', '--size', metavar='pad', required=True)
    pad.add_argument("infile")
    pad.add_argument("outfile")

    printver = subs.add_parser('printver', help='print image version')
    printver.add_argument('--config', metavar='filename', required=True,
                          help='Location of the file that contains macros')
    printver.add_argument("-t", "--type", metavar='type',
                      choices=IMG_TYPE_VALUES.keys(), required=True)

    args = parser.parse_args()
    if args.subcmd is None:
        print('Must specify a subcommand', file=sys.stderr)
        sys.exit(1)

    subcmds[args.subcmd](args)

if __name__ == '__main__':
    args()
