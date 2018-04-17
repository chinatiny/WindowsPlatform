import  os
import  os.path
import shutil

def del_spec_file(Path, Ignore, Suffix):
    if (Path[len(Path) - 1] == "\\" and Path[len(Path) - 2] == "\\") or (Path[len(Path) - 1] == "/"):
        pass
    else:
        Path = Path + "/"
    for File in os.listdir(Path):
        if os.path.isdir(Path + File):
            if File == "Debug" or File == "Release" or File == "ipch" or File == "x64":
                shutil.rmtree(Path + File)
            else:
                del_spec_file(Path + File, Ignore, Suffix)
        elif os.path.isfile(Path + File):
            FileSuffix = os.path.splitext(Path + File)[1]
            if File in Ignore:
                pass
            elif FileSuffix in Suffix:
                print("删除文件%s"%(Path + File))
                os.remove(Path + File)



if __name__ == "__main__":
	Suffix = [".idb", ".pdb", ".pch", ".res", ".ilk", ".exe", ".sdf", ".ipch", ".log", ".db", ".obj", ".tlog", ".lastbuildstate"]
	Ignore = ["AheadLib.exe","winmine.exe"]
	del_spec_file("./", Ignore, Suffix)



