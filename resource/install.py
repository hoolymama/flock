#!/usr/bin/env python3

import os
import sys
import errno
from shutil import copy2

PKG_DIR =  os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MODULE_NAME = os.path.basename(os.path.dirname(PKG_DIR))
MODULE_DIR = os.path.join(PKG_DIR, "maya")
VERSION = os.path.basename(PKG_DIR)  # 5.1.0
SUPPORTED_MAYA_VERSIONS = [2024]
MAYA_MODULES_DIR = os.path.expanduser("~/Library/Preferences/Autodesk/maya/modules")

def main():
    ensure_directory(MAYA_MODULES_DIR)

    fname = os.path.join(MAYA_MODULES_DIR, "{}.mod".format(MODULE_NAME))
    
    with open(fname, "w") as f:
        for maya_version in SUPPORTED_MAYA_VERSIONS:
            f.write("+ MAYAVERSION:{} {} {} {}\n".format(maya_version, MODULE_NAME, VERSION, MODULE_DIR))

    sys.stdout.write("Completed Maya Module setup! '{}'\n".format(fname))
    
def ensure_directory(directory):
    try:
        os.makedirs(directory)
    except OSError as ex:
        if ex.errno == errno.EEXIST and os.path.isdir(directory):
            pass
        else:
            raise

if __name__ == "__main__":
    main()


