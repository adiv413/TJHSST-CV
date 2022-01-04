import subprocess
import sys

subprocess.run(["g++", "l051.cpp", "-std=c++11", "-o", "l051"]) #, "-Wall"])
process = subprocess.Popen("./l051.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)