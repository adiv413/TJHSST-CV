import subprocess
import sys

subprocess.run(["g++", "l062.cpp", "-std=c++11", "-o", "l062"]) #, "-Wall"])
process = subprocess.Popen("./l062.exe -L 75 -H 115 -F coins_easy.ppm -TC 80", stdout=subprocess.PIPE) # use TC 80
for c in iter(lambda: process.stdout.read(1), b''): 
    sys.stdout.buffer.write(c)