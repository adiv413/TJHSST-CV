import subprocess
import sys

subprocess.run(["g++", "l034.cpp", "-std=c++11", "-o", "l034"]) #, "-Wall"])
process = subprocess.Popen("./l034.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)