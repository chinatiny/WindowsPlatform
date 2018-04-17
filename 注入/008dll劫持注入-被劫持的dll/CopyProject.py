#-*- coding:utf-8 -*-
import os
import sys
import os.path
import shutil
import tempfile

if __name__ == '__main__':
    Cmd = sys.argv[1]
    if "debug" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Debug/008dll劫持注入-被劫持的dll.lib")
        Dst = os.path.join(os.path.abspath('../'), "008dll劫持注入-被劫持的EXE/008dll劫持注入-被劫持的dll.lib")
        DstDir = os.path.join(os.path.abspath('../..'), "Debug/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)
    elif "release" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Release/008dll劫持注入-被劫持的dll.lib")
        Dst = os.path.join(os.path.abspath('../'), "008dll劫持注入-被劫持的EXE/008dll劫持注入-被劫持的dll.lib")
        DstDir = os.path.join(os.path.abspath('../..'), "Release/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)



