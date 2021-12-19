#! /usr/bin/env python3
import os
import subprocess
import argparse
import string

def convert_to_array(args):
    with open(args.infile, mode='rb') as f:
        payload = f.read()
    #print("payload[0] = " + hex(payload[0]))
    
    buf = "#define " + args.name.upper() + "_SZ " + str(len(payload)) + "\n"
    buf += "const uint8_t " + args.name + "[] = {"
    for i in range(len(payload)):
        if (i % 16 == 0):
            buf += "\n\t"  
        buf += str(hex(payload[i])) + ", "
    buf += "\n};\n\n"
    with open(args.outfile, 'w') as fo:
        fo.write(buf)

def main():
    parser = argparse.ArgumentParser()
  
    parser.add_argument('-name', '--name', metavar='array name')
    parser.add_argument("infile")
    parser.add_argument("outfile")
    
    args = parser.parse_args()
    convert_to_array(args)

if __name__ == '__main__':
    main()