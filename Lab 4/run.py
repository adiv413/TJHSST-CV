import subprocess
import sys

subprocess.run(["g++", "l041.cpp", "-std=c++11", "-o", "l041"]) #, "-Wall"])
process = subprocess.Popen("./l041.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)