import subprocess
import sys

subprocess.run(["g++", "l042.cpp", "-std=c++11", "-o", "l042"]) #, "-Wall"])
process = subprocess.Popen("./l042.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)