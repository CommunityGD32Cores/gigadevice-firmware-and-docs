#! /usr/bin/env python3
import os
import re
import subprocess
import argparse
import macro_parser 

flash_base_s_re = re.compile(r"^.*\s*RE_FLASH_BASE_S\s*(.*)")
flash_base_ns_re = re.compile(r"^.*\s*RE_FLASH_BASE_NS\s*(.*)")
offset_re = re.compile(r"^.*\s*RE_([0-9A-Z_]+)_OFFSET\s*(.*)")

def convert_to_hex(args):
    flash_base_s = macro_parser.evaluate_macro(args.config, flash_base_s_re, 0, 1)
    flash_base_ns = macro_parser.evaluate_macro(args.config, flash_base_ns_re, 0, 1)
    offsets = macro_parser.evaluate_macro(args.config, offset_re, 1, 2)
    if (args.type == 'IMG_0_NSPE' or args.type == 'IMG_1_NSPE') and (args.nsec == 1):
        act_offset = offsets[args.type] + flash_base_ns
    else:
        act_offset = offsets[args.type] + flash_base_s
    #act_offset = offsets[args.type] + flash_base_s
    cmd = args.exe + ' ' + args.infile + ' -Binary -offset ' + str(hex(act_offset)) + ' -o ' + args.outfile + ' -Intel'
    print(cmd)
    subprocess.run(cmd, stdin=None, input=None, stdout=None, stderr=None, shell=True, timeout=None, check=False)    

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--config', metavar='filename', required=True,
                          help='Location of the file that contains macros')
    parser.add_argument('-t', '--type', metavar='type', required=True,
                          help='SYS_SET / MBL / SYS_STATUS / IMG_0_PROT / IMG_0_AROT / IMG_0_NSPE / IMG_1_PROT / IMG_1_AROT / IMG_1_NSPE')
    parser.add_argument('-e', '--exe', metavar='filename', required=True,
                          help='Location of the file that contains macros')    
    parser.add_argument('-ns', '--nsec', type=int, default=0, help='Not security')    
    parser.add_argument("infile")
    parser.add_argument("outfile")
    
    args = parser.parse_args()
    convert_to_hex(args)

if __name__ == '__main__':
    main()