#-*- coding:utf-8 -*-
import os
import sys
import os.path
import shutil
import tempfile

if __name__ == '__main__':
    Cmd = sys.argv[1]
    if "debug" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Debug/001延时导入dll.lib")
        Dst = os.path.join(os.path.abspath('../'), "001使用延时导入表/001延时导入dll.lib")
        DstDir = os.path.join(os.path.abspath('../..'), "Debug/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)
    elif "release" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Release/001延时导入dll.lib")
        Dst = os.path.join(os.path.abspath('../'), "001使用延时导入表/001延时导入dll.lib")
        DstDir = os.path.join(os.path.abspath('../..'), "Release/")
        if not os.path.isdir(DstDir):
            os.mkdir(DstDir)
        print(Src)
        print(Dst)
        shutil.copy(Src, Dst)



