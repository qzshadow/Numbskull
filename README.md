# Numbskull
A distributed Gibbs smapling framework
Still developing...

## Installation Dependency
### Ubuntu 16.04
```bash
sudo apt update
sudo apt upgrade
sudo apt install libboost-all-dev cmake vim
```
These commands will install boost 1.58.0, openmpi 1.10.2 and cmake 3.5.1

## Import the project to Clion
Download Clion 2018.1 at https://www.jetbrains.com/clion/ and Install it

In the "welcome to Clion" screen, choose "Check out from Version Control"
Use the correct URL and login info to check the project to local

After check out, you will get into the Clion editor with this project open

Choose "File->Settings", search for "Toolchains", in the right panel, change C Compiler to "/usr/bin/mpicc", change C++ Compiler to "/usr/bin/mpic++", you do not need to change CMake and Debugger, using the Bundled is fine.

Choose "Run->Edit Configurations", add a Application called Numbskull and configure is in the following ways:


