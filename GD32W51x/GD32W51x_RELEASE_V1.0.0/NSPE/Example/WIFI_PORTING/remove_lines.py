#!/bin/env python
import shutil, sys, os

intent = None
rewrite_intent = None
delete = None

def delete_line(fname, intent):
    file_data = ""
    if delete == 'delete':
        with open(fname, "r", encoding='utf-8') as fd:
            lines = [l for l in fd if l.find(intent) != 0]
        with open(fname, "w", encoding='utf-8') as fd:
            fd.writelines(lines)
    elif delete == 'modify':
        with open(fname, "r", encoding='ISO-8859-1') as fd:
            for line in fd:
                line = line.replace(intent, rewrite_intent)
                file_data +=line
        with open(fname, "w", encoding='ISO-8859-1') as fd:
            fd.writelines(file_data)

def delete_all_file_lines(rootDir):
    if os.path.isdir(rootDir):
        for root, dirs, files in os.walk(rootDir):
            for file in files:
                if os.path.splitext(file)[1] == '.c' or os.path.splitext(file)[1] == '.h':
                    file_path = os.path.join(root, file)
                    delete_line(file_path, intent)
    else:
        delete_line(rootDir, intent)

if __name__ == '__main__':
    argv = sys.argv
    argc = len(argv)
    if argc < 4:
        print("Usage: %s <rootDir> <delete/modify> <intent> [rewrite intent]" %(os.path.basename(argv[0])))
        exit()

    root = argv[1]
    delete = argv[2]
    intent = argv[3]

    if argc == 5:
        # rewrite_intent.append(argv[4])
        rewrite_intent = argv[4]

    delete_all_file_lines(root)
