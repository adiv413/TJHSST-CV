import subprocess
import sys

subprocess.run(["g++", "l032.cpp", "-std=c++11", "-o", "l032"]) #, "-Wall"])
process = subprocess.Popen("./l032.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)