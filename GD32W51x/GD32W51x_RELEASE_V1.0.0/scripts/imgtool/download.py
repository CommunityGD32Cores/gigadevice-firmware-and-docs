import argparse
import subprocess
import sys
import os
from intelhex import IntelHex

HELP = 'prerequisite: Add sections below to jlink database file (C:\\Program Files (x86)\\SEGGER\\JLink_V640b\\JLinkDevices.xml)\n'
DATABASE_PATCH = """
  <Device>
   <ChipInfo Vendor="GD" Name="GD32W51x" Core="JLINK_CORE_CORTEX_M33"  WorkRAMAddr="0x30000000" WorkRAMSize="0x00030000" />
   <FlashBankInfo Name="SIP Flash" BaseAddr="0x08000000" MaxSize="0x00200000" Loader="Devices/GigaDevice/GD32W51x.FLM" LoaderType="FLASH_ALGO_TYPE_OPEN" />
  </Device>
  <Device>
   <ChipInfo Vendor="GD" Name="GD32W51x_S" Core="JLINK_CORE_CORTEX_M33"  WorkRAMAddr="0x30000000" WorkRAMSize="0x00030000" />
   <FlashBankInfo Name="SIP Flash" BaseAddr="0x0C000000" MaxSize="0x00200000" Loader="Devices/GigaDevice/GD32W51x_S.FLM" LoaderType="FLASH_ALGO_TYPE_OPEN" />
  </Device>
  <Device>
   <ChipInfo Vendor="GD" Name="GD32W51x_Q" Core="JLINK_CORE_CORTEX_M33"  WorkRAMAddr="0x30000000" WorkRAMSize="0x00030000" />
   <FlashBankInfo Name="QSPI Flash" BaseAddr="0x08000000" MaxSize="0x00200000" Loader="Devices/GigaDevice/GD32W51x_QSPI.FLM" LoaderType="FLASH_ALGO_TYPE_OPEN" />
  </Device>
  <Device>
   <ChipInfo Vendor="GD" Name="GD32W51x_Q_S" Core="JLINK_CORE_CORTEX_M33"  WorkRAMAddr="0x30000000" WorkRAMSize="0x00030000" />
   <FlashBankInfo Name="QSPI Flash" BaseAddr="0x0C000000" MaxSize="0x00200000" Loader="Devices/GigaDevice/GD32W51x_QSPI_S.FLM" LoaderType="FLASH_ALGO_TYPE_OPEN" />
  </Device>
"""
FLM_PATH = 'Put *.flm files under the following path (C:\\Program Files (x86)\\SEGGER\\JLink_V640b\\Devices\\GigaDevice\\)'



JLINK_PATH = r'C:\Program Files (x86)\SEGGER\JLink\JLink.exe'
JLINK_SCRIPT = 'download_script.jlink'
VENV = 'scripts\\venv'


def download_firmware_jlink(files, algo):
    if not files:
        print('Can\'t find any hex or axf images')
        return

    loadfile_cmd = [f'loadfile {f}' for f in files]
    #cmds = ['r', 'exec EnableEraseAllFlashBanks', 'erase', *loadfile_cmd, 'qc']
    #cmds = ['r', *loadfile_cmd, 'qc']
    if algo == 'GD32W51x_S':
        # Set all flash area as secure world if FMCOB = 1 (SECWM3 = 0x03FF0000)
        flashwm_change = ['w4 50022004 0x45670123', 'w4 50022004 0xCDEF89AB', 'w4 50022008 0x45670123', 'w4 50022008 0xCDEF89AB', 'w4 50022060 0x03FF0000', 'w4 50022010 0x00000080']
        flashwm_revert = ['w4 50022004 0x45670123', 'w4 50022004 0xCDEF89AB', 'w4 50022008 0x45670123', 'w4 50022008 0xCDEF89AB', 'w4 50022060 0x000003FF', 'w4 50022010 0x00000080']    
    else:
        flashwm_change = []
        flashwm_revert = []
    nocompare = ['exec SetCompareMode = 0']
    cmds = ['r'] + nocompare + flashwm_change + [*loadfile_cmd] + flashwm_revert + ['qc']

    script_contents = '\n'.join(cmds) + '\n'
    with open(JLINK_SCRIPT, 'w') as f:
        f.write(script_contents)
    cmd = [JLINK_PATH, '-device', algo, '-if', 'SWD', '-speed', '3000', '-autoconnect', '1', '-JTAGConf', '-1,-1', '-CommanderScript', JLINK_SCRIPT]
    subprocess.run(cmd, check=True)

def download_firmware_daplink(files, algo):
    if not files:
        print('Can\'t find any hex or axf images')
        return

    if algo == 'GD32W51x' or algo == 'GD32W51x_S':
        target = 'gd32w51x'
    elif algo == 'GD32W51x_Q' or algo == 'GD32W51x_Q_S':
        target = 'gd32w51x_qspi'

    p = subprocess.Popen(VENV + '\\Scripts\\activate.bat && pyocd cmd ' + f' --probe 0 --target {target}', shell=True, stdin=subprocess.PIPE)
    p.communicate(input=b'ww 0xE000EDD0 2\nexit\n')

    if target == 'gd32w51x':
        subprocess.run(VENV + '\\Scripts\\activate.bat && pyocd flash --disable-keep-unwritten ' + ' '.join(files) + f' --probe 0 --target {target}', shell=True, check=True)
    else:
        subprocess.run(VENV + '\\Scripts\\activate.bat && pyocd flash --disable-keep-unwritten --disable-smart-flash ' + ' '.join(files) + f' --probe 0 --target {target}', shell=True, check=True)

def verify_firmware_jlink(files, algo):
    if not files:
        print('Can\'t find any hex or axf images')
        return

    savefile_cmd = []
    verifybin_cmd = []
    for hex, bin in files:
        ih = IntelHex()
        ih.loadhex(hex)
        savefile_cmd.append(' '.join(['savebin', bin + '.read', f'{ih.minaddr():x}', f'{ih.maxaddr() - ih.minaddr() + 1:x}']))
        verifybin_cmd.append(' '.join(['verifybin', bin, f'{ih.minaddr():x}']))

    cmds = ['r', *savefile_cmd, 'qc']
    script_contents = '\n'.join(cmds) + '\n'
    with open(JLINK_SCRIPT, 'w') as f:
        f.write(script_contents)
    cmd = [JLINK_PATH, '-device', algo, '-if', 'SWD', '-speed', '4000', '-autoconnect', '1', '-JTAGConf', '-1,-1', '-CommanderScript', JLINK_SCRIPT]
    subprocess.run(cmd, check=True)

    cmds = ['r', *verifybin_cmd, 'qc']
    script_contents = '\n'.join(cmds) + '\n'
    with open(JLINK_SCRIPT, 'w') as f:
        f.write(script_contents)
    cmd = [JLINK_PATH, '-device', algo, '-if', 'SWD', '-speed', '4000', '-autoconnect', '1', '-JTAGConf', '-1,-1', '-CommanderScript', JLINK_SCRIPT]
    subprocess.run(cmd, check=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=HELP + DATABASE_PATCH + FLM_PATH)
    parser.add_argument('--dir', '-d', help='specify the folder of images')
    parser.add_argument('--file', '-f', action='append', help='specify the path of images')
    parser.add_argument('--verify', '-v', help='verify the binary contents programmed to the target')
    parser.add_argument('--algo', '-a', help='flash download algorithm', default="GD32101")
    parser.add_argument('--debugger', help='specify the debugger to perform flash programming', default="JLINK")
 
    args = parser.parse_args()

    if not args.dir and not args.file:
        print('Either parameter --dir or --file is required')
        sys.exit(1)

    if args.dir:
        files = [os.path.join(args.dir, f) for f in os.listdir(args.dir)]
        if args.verify:
            hex_files = [f for f in files if os.path.splitext(f)[1] == '.hex']
            bin_files = [f for f in files if os.path.splitext(f)[1] == '.bin']
            if len(hex_files) != len(bin_files):
                print('Error: missing matched binary files or hex files')
                sys.exit(1)
            file_tuples = [(hex, bin) for hex in hex_files for bin in bin_files if os.path.splitext(hex)[0] == os.path.splitext(bin)[0]]
            if args.debugger == 'JLINK':
                verify_firmware_jlink(file_tuples, args.algo)
        else:
            files = [f for f in files if os.path.splitext(f)[1] == '.hex' or os.path.splitext(f)[1] == '.axf']
            if args.debugger == 'JLINK':
                download_firmware_jlink(files, args.algo)
            elif args.debugger == 'DAPLINK':
                download_firmware_daplink(files, args.algo)
    else:
        if args.verify:
            if args.debugger == 'JLINK':
                verify_firmware_jlink(args.file, args.algo)
        else:
            if args.debugger == 'JLINK':
                download_firmware_jlink(args.file, args.algo)
            elif args.debugger == 'DAPLINK':
                download_firmware_daplink(args.file, args.algo)
