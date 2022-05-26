import subprocess
import sys

subprocess.run(["g++", "l062.cpp", "-std=c++11", "-o", "l062"]) #, "-Wall"])
process = subprocess.Popen("./l062.exe -L 55 -H 120 -F coins2.ppm -TC 15 -TCircle 15 -rmax 175", stdout=subprocess.PIPE)
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)