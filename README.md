# Procedural-Building-Generator

This was created as part of the Dean's Summer Research Internship (2017). 

![image info](./images/overview.png)

There are two source files included, being the SCAD file, and the C++ file. The SCAD file can be run in OpenSCAD without any additional depdencies, and was used as the "blueprint" design, wise for the actual C++ source code, which is recommended.

This program allows the user to procedurally generate a
random building of different sizes, different shapes,
different amount of windows, etc... The program also allows
the user to manually type and choose their own parameters
instead of having the program generate them randomly and then
export them automatically as a .OFF file for use in mesh editing
programs. This can all be done through the "input_parameters.txt"
text file which is where the user can edit each parameter. See help
for more info.

# Installation

TODO: Add CMake file.

This uses the CGAL library to create shapes. To run it, simply compile the main.cpp program and run it.

Note: 
1. In order to use the C++ code, please make sure your compiler supports C++11
2. In order to run the program, move the "input_parameters.txt" and move this to wherever your C++ compiler looks for input files. 
3. Rename the C++ file to main.cpp

# Running

 
In order to use the program, you must use the "input_parameters.txt"
text file. In here, you will see a list of all the parameters used
in the program. In order to type in an actual value, just replace
the number you wish to replace. In order to type in a range for a random
number from min-max, type in the following: = min, max.

Example: Shape Type = 1,3

Note: This feature cannot be used on the Railings and Remove Windows parameters
at the bottom of the text file.

In order to remove a parameter and have the program use its own ranges,
type "//" before the line.

Example: //Railings = (50,-1,1)

This will comment out the code just like how C++ does.
Note: In order to use the Railings parameter list, create a bracket
containing three variables, such that (size, y, side). The size is the
size of the railing, the y value it will take (if it is -1, it means apply
the railing to ALL y values), and the side. In order to allow multiple sides,
add additional brackets such that (),().

Note: In order to use the remove windows parameter list, it is the same as
railings, however each brackets contains (x,y,side). Once again, if a -1 is
applied, it removes all windows in that axis.

# Outputs

The outputs are found in the meshes/ folder. This folder contains three sets of meshes, where each set contains a C++ .OFF mesh file, openSCAD .oFF mesh file and its output parameters. In order to use the output parameters, just copy and paste the file into the "input_parameters.txt" file. 
	 


