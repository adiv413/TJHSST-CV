import subprocess
import sys

subprocess.run(["g++", "l052.cpp", "-std=c++11", "-o", "l052"]) #, "-Wall"])
process = subprocess.Popen("./l052.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)