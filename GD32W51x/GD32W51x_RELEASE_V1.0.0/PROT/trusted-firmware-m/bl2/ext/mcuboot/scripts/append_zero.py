# -*- coding: utf-8 -*-
import sys
import os
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("python append_zero.py [file_name] [length]")
    else:
        with open(sys.argv[1], 'a') as f:
            l = int(sys.argv[2])
            f.seek(0, os.SEEK_END)
            file_length = f.tell() >> 2
            print(file_length, f.tell())
            if l > file_length:
                f.write('00\n' * (l - file_length))

