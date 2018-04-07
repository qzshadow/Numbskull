# Numbskull
A distributed Gibbs sampling framework

Still developing...

## Installation Dependency
### Ubuntu 16.04
```bash
sudo apt update
sudo apt upgrade
sudo apt install libboost-all-dev cmake
```
These commands will install boost 1.58.0, openmpi 1.10.2 and cmake 3.5.1

## Import the project to Clion
1. Download Clion 2018.1 at https://www.jetbrains.com/clion/ and install it

2. In the "welcome to Clion" screen, choose "Check out from Version Control"
Use the correct URL and login info to check the project to local machine

3. After checking out, you will open the project in the Clion editor

4. Choose "File->Settings", search for "Toolchains", in the right panel, change C Compiler to "/usr/bin/mpicc", change C++ Compiler to "/usr/bin/mpic++", you do not need to change CMake and Debugger, using the Bundled is fine.

5. Choose "Run->Edit Configurations", add a Application called Numbskull and configure is in the following ways:
The working directory should correspond to the the ClionProjects folder on your machine.
<img src="doc/configuration.png" width="800" />

6. hit the green triangle button in the right top corner of the editor to run a BDC instance.

7. to run BFD instance, change `main.cpp` to `test/main.cpp` in the CMakeLists.txt file the run again.

8. The correct result for this graph will be count of var0 takes value 1 / value 0 = 0.78 / 0.22;
count of var1 or var2 takes value 1 / value 0 = 0.68 / 0.32;

## Debug
Debug using multiple GDB instance:

In a terminal, get into the folder contains runable "Numbskull", generally it will be located at "~/CLionProjects/Numbskull/cmake-build-debug", using the command
```
mpirun -np 3 xterm -e gdb Numbskull
```
3 is the total machine number of master and workers

## Other things helps you debug
every time see error like "invaild world rank"
check **Executable** in the run configuration (see the graph above), make sure it is **mpirun** rather than Numbskull.
