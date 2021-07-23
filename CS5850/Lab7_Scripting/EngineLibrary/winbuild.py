# Why am I not using Make?
# 1.)   I want total control over the system.
#       Occassionally I want to have some logic
#       in my compilation process.
# 2.)   Realistically our projects are 'small' enough
#       this will not matter.
# 3.)   Feel free to implement your own make files.
# 4.)   It is handy to know Python

import os
import platform
# Compile with g++ -std=c++14 -shared -fPIC -static-libgcc -static-libstdc++ -I./include/ -I./pybind11/include/ `python3 -m pybind11 --includes` ./src/*.cpp -o mygameengine.pyd `python3-config --ldflags` -lSDL2 -lmingw32 -mwindows -L libwinpthread-1.dll

COMPILER="g++"

SOURCE="./src/*.cpp"

# You can can add other arguments as you see fit.
# What does the "-D LINUX" command do?
ARGUMENTS="-std=c++14 -shared -fPIC -static-libgcc -static-libstdc++"

# Which directories do we want to include.
INCLUDE_DIR="-I./include/ -I./pybind11/include/ `python3 -m pybind11 --includes`"

# What libraries do we want to include
LIBRARIES="`python3-config --ldflags` -lSDL2 -lmingw32 -mwindows -L libwinpthread-1.dll"

# The name of our module
EXECUTABLE="mygameengine.pyd"

# Build a string of our compile commands that we run in the terminal
compileString=COMPILER+" "+ARGUMENTS+" "+INCLUDE_DIR+" "+SOURCE+" -o "+EXECUTABLE+" "+LIBRARIES

# Print out the compile string
print(compileString)

# Run our command
os.system(compileString)