# TDT4230 - Graphics and Visualization
## Final Project - Diorama: 3D Gaussian Splatting Rasterizer

This is my go at implementing a [3D Gaussian Splatting](https://repo-sam.inria.fr/fungraph/3d-gaussian-splatting/) Rasterizer.
Controls are wacky, but try to make them work :P
- UP to move forward
- DOWN to move backward
- LEFT to turn left
- RIGHT to turn right
- SPACE to move up
- Z to move down
- X to look down
- C to look up
- 9 to explode
- 0 to reset explostion

## What do i do?

	git clone --recursive https://github.com/vetlemangrud/diorama.git

Should you forget the `--recursive` bit, just run:

	git submodule update --init


The ply files are large, so they are not included in GitHub. Find a file and put it in the assets folder. Enter the file name in src/diorama.cpp.


### Windows

Install Microsoft Visual Studio Express and CMake.
You may use CMake-gui or the command-line cmake to generate a Visual Studio solution.

### Linux:

Make sure you have a C/C++ compiler such as  GCC, CMake and Git.

	make run

which is equivalent to

	git submodule update --init
	cd build
	cmake ..
	make
	./glowbox
