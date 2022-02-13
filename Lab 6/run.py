import subprocess
import sys

subprocess.run(["g++", "l061.cpp", "-std=c++11", "-o", "l061"]) #, "-Wall"])
process = subprocess.Popen("./l061.exe -L 90 -H 150 -F image7.ppm -TC 1", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)