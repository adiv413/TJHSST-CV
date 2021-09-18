import subprocess
import sys

subprocess.run(["g++", "l021.cpp", "-std=c++11", "-o", "l021"])
process = subprocess.Popen("./l021.exe", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)