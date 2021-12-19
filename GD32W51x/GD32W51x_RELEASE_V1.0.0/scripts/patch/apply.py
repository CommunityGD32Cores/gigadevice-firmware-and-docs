import os, sys
import subprocess
import re
import argparse
from pathlib import Path
from io import StringIO
from traceback import print_tb

# https://github.com/techtonik/python-patch/
# with the patch below to support creating and deleting files
# https://storage.googleapis.com/google-code-attachments/python-patch/issue-28/comment-2/patch-support-new-files-20141214.diff
import patch

VIRTUAL_ENVIRONMENT_PATH = Path('venv')
PATCH_FILE = 'pyocd.diff'


def run(venv, revert=False):

    patchdir = os.path.dirname(os.path.abspath(__file__))
    patchset = patch.fromfile(Path(patchdir) / PATCH_FILE)
    if not patchset:
        print('Failed to read patch file')
        patch.setdebug()
        patchset = patch.fromfile(Path(patchdir) / PATCH_FILE)
        return 1
    try:
        if revert:
            ret = patchset.revert(root=venv)
        else:
            ret = patchset.apply(root=venv)
        if not ret:
            raise AssertionError('Failed to patch')
    except:
        exc_type, exc_value, exc_tb = sys.exc_info()
        print(f'Exception: {exc_type} {exc_value}')
        print_tb(exc_tb)
        print('Retry with debug log on:')
        patch.setdebug()
        if revert:
            patchset.revert(root=venv)
        else:
            patchset.apply(root=venv)
    else:
        print('Patched successfully')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--revert', action='store_true', help='revert the changes')
    parser.add_argument('--venv', help='the path to the virtual environment')
    args = parser.parse_args()

    if args.venv:
        venv = args.venv
    else:
        try:
            venv = subprocess.check_output('poetry env info --path', shell=True, universal_newlines=True).strip()
        except subprocess.CalledProcessError:
            #print(f'poetry not found, use {VIRTUAL_ENVIRONMENT_PATH} instead')
            venv = VIRTUAL_ENVIRONMENT_PATH

    venv = Path(venv)
    if os.name == 'nt':
        venv =  venv / 'Lib'
    elif os.name == 'posix':
        venv =  venv / 'lib' / 'python{}.{}'.format(sys.version_info.major, sys.version_info.minor)
    else:
        print('Unknown OS {}'.format(os.name))
        sys.exit(1)

    sys.exit(run(venv, args.revert))
