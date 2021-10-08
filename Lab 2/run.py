import subprocess
import sys

subprocess.run(["g++", "l022.cpp", "-std=c++11", "-o", "l022"]) #, "-Wall"])
process = subprocess.Popen("./l022.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)