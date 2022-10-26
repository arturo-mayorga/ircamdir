# Camera Director for iRacing #

## Getting Started ##
### Download ###
 * Download the latest build from [here](https://drive.google.com/drive/folders/1TNDiMQwAWX3isMNr9zUv8-KJj9hbfN3q?usp=sharing).
 * Load iRacing into watching a race/replay session you want to watch
 * run camdir.exe
 
### Local Build ###
 * Download the code
 * Make sure CMake is installed on you dev machine
 * Run `build.sh` in something like GitBash
 * Load iRacing into a race/replay session you want to watch
 * run `./build/Debug/camdir.exe

### About the Code ###

The camera director is implemented using a Entity Component System-based Game Loop.  Components (gray) are data stores and Systems (blue) operate based on the data contained within each system.
![System Diagram](https://docs.google.com/drawings/d/e/2PACX-1vSKYhsDnpEdOsxjvnDTE_Y190N2_dRg-cbnHvDMkW-szTfzznUXfRF3tOQaOQ7mKi-YsKk1tn2xOIoX/pub?w=1974&h=1047)
