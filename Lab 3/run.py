import subprocess
import sys

subprocess.run(["g++", "l033.cpp", "-std=c++11", "-o", "l033"]) #, "-Wall"])
process = subprocess.Popen("./l033.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)