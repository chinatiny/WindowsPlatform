#-*- coding:utf-8 -*-
import os
import sys
import os.path
import shutil
import tempfile

if __name__ == '__main__':
    Cmd = sys.argv[1]
    if "debug" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Debug/008dll劫持注入-被劫持的dll.dll")
        Dst = os.path.join(os.path.abspath('../..'), "Debug/008dll劫持注入-被劫持的dllOrg.dll")
        shutil.move(Src, Dst)
        print(Src)
        print(Dst)

        Src2 = os.path.join(os.path.abspath('../..'), "Debug/008dll劫持注入-AheadLib.exe生成的dll工程.dll")
        Dst2 = os.path.join(os.path.abspath('../..'), "Debug/008dll劫持注入-被劫持的dll.dll")
        shutil.move(Src2, Dst2)
        print(Src2)
        print(Dst2)
    elif "release" == Cmd:
        Src = os.path.join(os.path.abspath('../..'), "Release/008dll劫持注入-被劫持的dll.dll")
        Dst = os.path.join(os.path.abspath('../..'), "Release/008dll劫持注入-被劫持的dllOrg.dll")
        print(Src)
        print(Dst)
        shutil.move(Src, Dst)

        Src2 = os.path.join(os.path.abspath('../..'), "Release/008dll劫持注入-AheadLib.exe生成的dll工程.dll")
        Dst2 = os.path.join(os.path.abspath('../..'), "Release/008dll劫持注入-被劫持的dll.dll")
        shutil.move(Src2, Dst2)
        print(Src2)
        print(Dst2)



