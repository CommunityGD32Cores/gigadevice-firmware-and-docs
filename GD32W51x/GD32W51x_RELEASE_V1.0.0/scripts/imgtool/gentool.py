#! /usr/bin/env python3
#
# Copyright 2017 Linaro Limited
# Copyright (c) 2017-2019, Arm Limited.
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

"""
Assemble multiple images into a single image that can be flashed on the device.
"""

import argparse
import errno
import io
import re
import os
import shutil
import macro_parser

offset_re = re.compile(r"^.*\s*RE_([0-9A-Z_]+)_OFFSET\s*(.*)")

class Assembly():
    def __init__(self, config_path, output):
        self.output = output
        self.config_path = config_path
        self.first_offset = -1
        self.overwrite = 0
        self.find_slots()
        try:
            os.unlink(output)
        except OSError as e:
            if e.errno != errno.ENOENT:
                raise

    def find_slots(self):
        self.offsets = macro_parser.evaluate_macro(self.config_path, offset_re, 1, 2)

    def add_image(self, source, partition):
        if self.first_offset == -1:
            self.first_offset = self.offsets[partition]
        offset = self.offsets[partition] - self.first_offset
        if offset < 0:
            raise Exception("Partitions not in order, unsupported")
        print("partition = " , partition ,"  offset = " , offset)
        with open(self.output, 'ab') as ofd:
            #ofd.seek(0, os.SEEK_END)
            pos = ofd.tell()
            if pos > offset:
                if self.overwrite:
                    ofd.seek((offset - pos), os.SEEK_CUR)
                else:
                    raise Exception("Partitions not in order, unsupported")
            if pos < offset:
                ofd.write(b'\xFF' * (offset - pos))
            with open(source, 'rb') as rfd:
                shutil.copyfileobj(rfd, ofd, 0x10000)  #"0x10000 means buffer size"

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--config', required=True,
            help='Location of the file that contains preprocessed macros')
    parser.add_argument('--sys_set', help='System Settings')
    parser.add_argument('--mbl', help='Main Boot Loader')
    parser.add_argument('--sys_status', help='System Status')
    parser.add_argument('--prot_0', help='PSA Root of Image 0')
    parser.add_argument('--arot_0', help='Application ROOT of Image 0')
    parser.add_argument('--nspe_0', help='Non Secure Application of Image 0')
    parser.add_argument('--prot_1', help='PSA Root of Image 1')
    parser.add_argument('--arot_1', help='Application ROOT of Image 1')
    parser.add_argument('--nspe_1', help='Non Secure Application of Image 1')
    parser.add_argument('--user_1', help='User data 1')
    parser.add_argument('--user_2', help='User data 2')
    parser.add_argument('--overwrite', help='Allow to overwrite data')
    parser.add_argument('-o', '--output', required=True,
            help='Filename to write full image to')

    args = parser.parse_args()
    output = Assembly(args.config, args.output)
    if args.overwrite:
        output.overwrite = 1
    if args.sys_set:
        output.add_image(args.sys_set, "SYS_SET")
    if args.mbl:
        output.add_image(args.mbl, "MBL")
    if args.sys_status:
        output.add_image(args.sys_status, "SYS_STATUS")
    if args.prot_0:
        output.add_image(args.prot_0, "IMG_0_PROT")
    if args.arot_0:
        output.add_image(args.arot_0, "IMG_0_AROT")
    if args.nspe_0:
        output.add_image(args.nspe_0, "IMG_0_NSPE")
    if args.prot_1:
        output.add_image(args.prot_1, "IMG_1_PROT")
    if args.arot_1:
        output.add_image(args.arot_1, "IMG_1_AROT")
    if args.nspe_1:
        output.add_image(args.nspe_1, "IMG_1_NSPE")
    if args.user_1:
        output.add_image(args.user_1, "USER_1")
    if args.user_2:
        output.add_image(args.user_2, "USER_2")

if __name__ == '__main__':
    main()
