#!/usr/bin/python3
import os

def walkDir(start):
    subfolders = [ f.path for f in os.scandir(start) if f.is_dir() ]
    subfiles = [ f.path for f in os.scandir(start) if f.is_file() ]
    for subf in subfiles:
        if ".c" in subf:
            subf = subf.replace("test_", '');
            subf = "../../"+subf
            subf = os.path.abspath(subf);
            if (os.path.isfile(subf)):
                print(subf, end=' ');
    for subfold in subfolders:
        walkDir(subfold);

walkDir("test_ex2_obc_software");
