import subprocess
import sys

subprocess.run(["g++", "l01.cpp", "-std=c++11", "-o", "l01"])
process = subprocess.Popen("./l01.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)