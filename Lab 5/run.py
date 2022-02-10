import subprocess
import sys

subprocess.run(["g++", "l053.cpp", "-std=c++11", "-o", "l053"]) #, "-Wall"])
process = subprocess.Popen("./l053.exe -L 100 -H 200 -F image.ppm", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)