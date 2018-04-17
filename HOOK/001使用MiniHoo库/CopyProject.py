#-*- coding:utf-8 -*-
import os
import sys
import os.path
import shutil
import tempfile

if __name__ == '__main__':
    Cmd = sys.argv[1]
    if "debug" == Cmd:
        Src = os.path.join(os.path.abspath('./MinHook/bin/'), "MinHook.x86.dll")
        Dst = os.path.join(os.path.abspath('../../'), "Debug/MinHook.x86.dll")
        DstDir = os.path.join(os.path.abspath('../..'), "Debug/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)
    elif "release" == Cmd:
        Src = os.path.join(os.path.abspath('./MinHook/bin/'), "MinHook.x86.dll")
        Dst = os.path.join(os.path.abspath('../../'), "Release/MinHook.x86.dll")
        DstDir = os.path.join(os.path.abspath('../..'), "Release/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)



