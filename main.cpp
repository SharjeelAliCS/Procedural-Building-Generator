/*
 |**********************************************************************;
 * Project           : Procedural Building Generation
 * Program name      : DSRI_CGAL_BUILDING_GENERATION.cpp
 * Author            : Ali Sharjeel
 * Date created      : 2018/07/22
 * Purpose           : This program allows the user to procedurally generate a
 random building of different sizes, different shapes,
 different amount of windows, etc... The program also allows
 the user to manually type and choose their own parameters
 instead of having the program generate them randomly and then
 export them automatically as a .OFF file for use in mesh editing
 programs. This can all be done through the "input_parameters.txt"
 text file which is where the user can edit each parameter. See help
 for more info.
 
 * Help              : In order to use the program, you must use the "input_parameters.txt"
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
 
 * Sources:          :https://doc.cgal.org/latest/Nef_3/Nef_3_2transformation_8cpp-example.html
 |**********************************************************************;
 */

//NOTE: COMPLIER MUST support C++11!

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Nef_polyhedron_3.h>
#include <CGAL/boost/graph/convert_nef_polyhedron_to_polygon_mesh.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/IO/Color.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <CGAL/convex_hull_3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stdlib.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Exact_kernel;
typedef CGAL::Polyhedron_3<Exact_kernel> Polyhedron;
typedef CGAL::Polyhedron_3<Exact_kernel> Polyhedron_3;
typedef CGAL::Surface_mesh<Exact_kernel::Point_3> Surface_mesh;
typedef CGAL::Nef_polyhedron_3<Exact_kernel> Nef_polyhedron;
typedef Nef_polyhedron::Plane_3  Plane_3;
typedef Nef_polyhedron::Vector_3  Vector_3;
typedef Nef_polyhedron::Aff_transformation_3  Aff_transformation_3;
typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef Exact_kernel::Point_3 Point_3;

//------------------------------GLOBAL VARIABLES--------------------------


//These variables below are used to first generate cube/cylinder when the program is run.
//Then everytime a new cube or cylinder is created, it copies the cube/cylinder from the variable
//below. This reduces the total time it takes to generate a mesh.

Polyhedron poly_org;
Nef_polyhedron nef_org;
double poly_org_width = 10;
Nef_polyhedron cylinder_org;
int fn = 20; //This is used for the amount of sides of the cylinder.
std::string file_name;
const double PI  =3.141592;

//------------------------------HELPER FUNCTIONS-------------------------

//Below are the two random functions. These are used to generate a random number
//from min-max, and can either be a double or an integer.
double randDNum(double min, double max){
    double rand_num =fmod(rand(), (max - min + 1)) + min;
    return rand_num;
}

int randINum(int min, int max){
    int rand_num = randDNum(min, max);
    return rand_num;
}

//------------------------------TRANSFORMATIONS--------------------------

//The scale function transforms a Nef_polyhedron by scaling it.
void scale(double scale_x, double scale_y, double scale_z, Nef_polyhedron& poly){
    
    Aff_transformation_3 scale_size(scale_x*2,0,0,
                                    0,scale_y*2,0,
                                    0,0,scale_z*2,
                                    2);
    poly.transform(scale_size);
}

//The translate function transforms a Nef_polyhedron by moving it.
void translate(double transl_x, double transl_y, double transl_z, Nef_polyhedron& poly){
    Aff_transformation_3 transl(CGAL::TRANSLATION, Vector_3(transl_x, transl_y, transl_z));
    poly.transform(transl);
}
//The rotate function transforms a Nef_polyhedron by rotating it by the x, y, z axis.
void rotate(double rotate_x, double rotate_y, double rotate_z, Nef_polyhedron& poly){
    
    double cos_x =cos(rotate_x*PI/180);
    double sin_x =sin(rotate_x*PI/180);
    Aff_transformation_3 rot_x(1,0,0,
                               0,cos_x,-sin_x,
                               0,sin_x,cos_x,
                               1);
    poly.transform(rot_x);
    
    cos_x =cos(rotate_y*PI/180);
    sin_x =sin(rotate_y*PI/180);
    Aff_transformation_3 rot_y(cos_x,0,sin_x,
                               0,1,0,
                               -sin_x,0,cos_x,
                               1);
    poly.transform(rot_y);
    
    cos_x =cos(rotate_z*PI/180);
    sin_x =sin(rotate_z*PI/180);
    Aff_transformation_3 rot_z(cos_x,-sin_x,0,
                               sin_x,cos_x,0,
                               0,0,1,
                               1);
    poly.transform(rot_z);
    
}


//------------------------------POLYHEDRON FUNCTIONS--------------------------

//This section handles all the code related to creating polyhedrons.

//This function generates a cube polyhedron of the width defined below,
//and saves it as a Nef_polyhedron nef_org (global variable).
void gen_poly(){
    poly_org_width = poly_org_width/2;
    
    std::string str_width = std::to_string(poly_org_width);
    
    //The below declares the string which contains the various sides (triangular)
    //as well as the coordinates of each vertex.
    std::string input =
    "OFF\n\
    8 12 0\n\
    -X -X -X\n\
    -X X -X\n\
    X X -X\n\
    X -X -X\n\
    -X -X X\n\
    -X X X\n\
    X X X\n\
    X -X X\n\
    3  0 1 3\n\
    3  3 1 2\n\
    3  0 4 1\n\
    3  1 4 5\n\
    3  3 2 7\n\
    3  7 2 6\n\
    3  4 0 3\n\
    3  7 4 3\n\
    3  6 4 7\n\
    3  6 5 4\n\
    3  1 5 6\n\
    3  2 1 6";
    
    //The below replaces every X with the actual value.
    while(input.find('X')!=std::string::npos){
        input.replace(input.find('X'), 1, str_width);
    }
    
    //Converts the string to a polyhedron.
    std::stringstream ss;
    ss << input;
    ss >> poly_org;
    
    Nef_polyhedron nef(poly_org);
    nef_org = nef;
}

//This function is called whenever a cube is created. Here, it uses the
//nef_org cube declared globally as a template and transforms it by the width.
Nef_polyhedron cube(double width){//change int to double
    Nef_polyhedron nef = nef_org;
    scale(width/poly_org_width/2, width/poly_org_width/2, width/poly_org_width/2, nef);
    
    return nef;
}

//This function is called whenever a square is created. Here, it uses the
//nef_org cube declared globally as a template and transforms it by the width and length.
Nef_polyhedron square(double width, double length){//change int to double
    Nef_polyhedron nef = nef_org;
    scale(width/poly_org_width/2, length/poly_org_width/2, 1/poly_org_width/2, nef);
    
    return nef;
    
    
}
//This function creates a 3D convex hull using the inputed polyhedron and the coord array.
void hull(Polyhedron_3& poly, double coord[][3], int size){
    
    std::vector<Point_3> points;
    Point_3 p;
    
    //The below converts the array into a vector point 3 object.
    for(int i =0; i<size; i++){
        points.push_back(Point_3(coord[i][0],coord[i][1],coord[i][2]));
    }
    
    //computes convex hull for the polyhedorn.
    CGAL::convex_hull_3(points.begin(), points.end(), poly);
    
    Surface_mesh sm;
    CGAL::convex_hull_3(points.begin(), points.end(), sm);
    
}

//This function makes use of the hull function from above. It takes a width, height
//and sides as input and uses these to create a n sided polygon.
Nef_polyhedron polygon(double circumradius_addition, double width, double height, int sides){
    
    //Note: circumradius_addition is the amount to add to the width of the shape
    //For use with the overhangs.
    double sin_x =sin((180/sides)*PI/180);
    double circumradius = width/(2*sin_x)+circumradius_addition;//Length to vertex
    double sum_angle = (sides-2)*180;
    double side_angle = sum_angle/sides;
    
    double coord[sides*2][3];
    
    //This computes each coordinate and adds it to the list. Note the two coord sets below.
    //each one stores the top and bottom of the vertex.
    for(int i =0; i<sides;i++){
        double x  = circumradius*cos(((180-side_angle)*i)*PI/180);
        double y  = circumradius*sin(((180-side_angle)*i)*PI/180);
        
        coord[i*2][0]= x;
        coord[i*2][1]= y;
        coord[i*2][2]= height/2;
        
        coord[i*2+1][0]= x;
        coord[i*2+1][1]= y;
        coord[i*2+1][2]= -height/2;
    }
    
    Polyhedron_3 polygon;
    
    hull(polygon, coord, sides*2);
    Nef_polyhedron nef_polygon(polygon);
    
    return nef_polygon;
}

//This creates the global cylinder to be used with all cylinders later on.
void gen_cylinder(double height, int sides){
    
    
    double circumradius = 1;//Length to vertex
    double sum_angle = (sides-2)*180;
    double side_angle = sum_angle/sides;
    
    double coord[sides*2][3];// = {{-10,-10,0},{10,-10,0},{-10,10,0},{10,10,0},{0,0,10}};
    
    for(int i =0; i<sides;i++){
        double x  = circumradius*cos(((180-side_angle)*i)*PI/180);
        double y  = circumradius*sin(((180-side_angle)*i)*PI/180);
        
        coord[i*2][0]= x;
        coord[i*2][1]= y;
        coord[i*2][2]= height/2;
        
        coord[i*2+1][0]= x;
        coord[i*2+1][1]= y;
        coord[i*2+1][2]= -height/2;
        
    }
    
    Polyhedron_3 polygon;
    
    hull(polygon, coord, sides*2);
    Nef_polyhedron nef(polygon);
    cylinder_org =  nef;
    
}
//This is called whenever a cylinder is meant to be created. It uses the cylinder_org global
//polyhedron as a template and transforms it by the height.
Nef_polyhedron cylinder(double height){
    Nef_polyhedron nef = cylinder_org;
    scale(0.5, 0.5, height/2, nef);
    
    return nef;
}

//------------------------------WINDOW GRAMMAR--------------------------

//This function generates a simple door.
Nef_polyhedron gen_door(double door_width, double door_height, double tile_width, double tile_height, double bottom_height){
    
    Nef_polyhedron nef_square1 = square(1, door_width*1.2);
    translate(1,0,door_height/2-(tile_height/2-door_height/2)*0.7, nef_square1);
    
    Nef_polyhedron nef_square2 = square(1,door_width*1.5);
    scale(20,1,10, nef_square2);
    translate(1,0,door_height/2-(tile_height/2-door_height/2), nef_square2);
    
    Nef_polyhedron nef = nef_square2;
    
    Nef_polyhedron nef_cylinder = cylinder(door_height*2);
    scale(20,20,1,nef_cylinder);
    
    Nef_polyhedron railings;
    for(int i = 0; i<=1; i++){
        Nef_polyhedron nef_loop = nef_cylinder;
        translate(10,(door_width/2*1.2-door_width*i*1.2),-(tile_height/2-door_height/2), nef_loop);
        railings+=nef_loop;
        
    }
    translate(-10,0,0, railings);
    nef+=railings;
    translate(0,0,-bottom_height, nef);
    return nef;
}

//The below set of functions contains five different window fuctions, each creating a unique window
//design and returning it as a Nef_polyhedron.

Nef_polyhedron gen_window_1(double window_width, double window_height){
    
    Nef_polyhedron square1 = square(1, window_width*1.5);
    scale(20,1,5, square1);
    translate(-1,0,-window_height/2, square1);
    
    Nef_polyhedron square2 = square(1, window_width*1.5);
    scale(10,1,5, square2);
    translate(-1,0,window_height/2, square2);
    
    Nef_polyhedron square3 = square(1, window_width);
    scale(5,1,3, square3);
    translate(-1,0,0, square3);
    
    Nef_polyhedron nef =square1+square2+square3;
    
    Nef_polyhedron nef_cylinder = cylinder(window_height*2);
    scale(5,5,1,nef_cylinder);
    
    for(int i = 0; i<=1; i++){
        Nef_polyhedron nef_loop =  nef_cylinder;
        translate(-1,window_width/2-window_width*i,0, nef_loop);
        nef+=nef_loop;
    }
    
    return nef;
}

Nef_polyhedron gen_window_2(double window_width, double window_height){
    
    Nef_polyhedron nef;
    
    
    Nef_polyhedron nef1 = square(1, window_width*1.5);
    scale(2, 1, 0.5, nef1);
    translate(-2, 0, -window_height/2, nef1);
    
    Nef_polyhedron nef2 = square(1, window_width*1.5);
    scale(1,1,0.5, nef2);
    translate(-2, 0, window_height/2, nef2);
    
    Nef_polyhedron nef3=cube(window_height);
    scale(0.1,0.1,1, nef3);
    translate(-2, 0,0, nef3);
    nef = nef1+nef2+nef3;
    
    Nef_polyhedron nef4 = cube(window_height);
    scale(0.1,0.15,1, nef4);
    translate(-1,window_width/2*1.2,0, nef4);
    
    Nef_polyhedron nef5 = square(1, window_width);
    scale(0.5,1,2, nef5);
    translate(-2,0,window_height/2*0.3, nef5);
    
    for(int i =0; i<=1; i++){
        rotate(static_cast<double>(180*i), 0, 0, nef4);
        rotate(static_cast<double>(180*i), 0, 0, nef5);
        
        nef+=(nef4+nef5);
        
    }
    
    return nef;
}

Nef_polyhedron gen_window_3(double window_width, double window_height){
    
    Nef_polyhedron square1 = square(1, window_width*1.2);
    scale(30,1,10, square1);
    translate(-1,0,-window_height/2*1.15, square1);
    
    Nef_polyhedron square2 = square(1, window_width*1.3);
    scale(50,1,10, square2);
    translate(-1,0,-window_height/2, square2);
    
    Nef_polyhedron square3 = square(1, window_width*1.3);
    scale(1,1,0.5, square3);
    translate(-1,0,window_height/2, square3);
    
    Nef_polyhedron square4 = square(1, window_width);
    scale(4.5,1,2, square4);
    translate(-1,0,window_height/2*0.2, square4);
    
    Nef_polyhedron square5 = square(window_width, window_width);
    scale(0.5,1.6,10, square5);
    translate(40,0,window_height/2, square5);
    rotate(0,-45,0, square5);
    
    
    Nef_polyhedron cube1 = cube(window_height);
    scale(0.1,0.1,0.6, cube1);
    translate(-1,0,-window_height/5, cube1);
    
    Nef_polyhedron nef =square1+square2+square3+square4+square5+cube1;
    
    Nef_polyhedron cube2 = cube(window_height);
    scale(0.3,0.15,1, cube2);
    translate(-1,window_width/2*1.2,0, cube2);
    
    for(int i = 0; i<=1;i++){
        rotate(180*i,0,0, cube2);
        nef+=cube2;
    }
    
    return nef;
    
}

Nef_polyhedron gen_window_4(double window_width, double window_height){
    Nef_polyhedron nef;
    
    Nef_polyhedron nef_cylinder = cylinder(window_height*2);
    scale(5,5,1,nef_cylinder);
    translate(-0.5,window_width/2,0, nef_cylinder);
    
    Nef_polyhedron nef_square = square(15, window_width);
    scale(1,1,4, nef_square);
    translate(0,0,window_height/2, nef_square);
    
    for(int i =0; i<=1; i++){
        
        rotate(static_cast<double>(180*i),0,0, nef_cylinder);
        rotate(0, static_cast<double>(180*i), 0, nef_square);
        nef+=(nef_square+nef_cylinder);
    }
    return nef;
}

Nef_polyhedron gen_window_5(double window_width, double window_height){
    
    
    Nef_polyhedron square1 = square(1, window_width);
    scale(10,1,3, square1);
    translate(-0.5,0,window_height*1.35/4, square1);
    
    Nef_polyhedron square2 = square(1, window_width*1.2);
    scale(25,1,5, square2);
    translate(-1,0,window_height*1.35/10-window_height*1.35/4, square2);
    
    Nef_polyhedron square3 = square(1, window_width*1.2);
    scale(20,1,7, square3);
    translate(-1,0,window_height*1.35/10+window_height*1.35/4, square3);
    
    Nef_polyhedron square4 = square(1, window_width);
    scale(4,1,1, square4);
    translate(-0.3,0,window_height*1.35/12, square4);
    
    Nef_polyhedron square5 = square(1, window_width);
    scale(4,1,1, square5);
    translate(-0.3,0,-window_height*1.35/12, square5);
    
    Nef_polyhedron cylinder1 = cylinder(window_height);
    scale(5,5,1.35, cylinder1);
    
    Nef_polyhedron cylinder2 = cylinder(window_height);
    scale(2,2,1, cylinder2);
    translate(-0.5,window_width*0.3,window_height*1.35/20, cylinder2);
    
    Nef_polyhedron nef =square1+square2+square3+square4+square5;
    
    for(int i = 0; i<=1; i++){
        Nef_polyhedron nef_loop = cylinder1;
        translate(-0.5,window_width/2-window_width*i,window_height*1.35/10, nef_loop);
        rotate(0,0,180*i, cylinder2);
        nef+=nef_loop+cylinder2;
    }
    return nef;
    
}


//this function calls the specific window generation function based on the window type chosen.
Nef_polyhedron gen_windows(double window_width, int window_op, double tile_width,double window_height, double tile_height){
    Nef_polyhedron nef;
    
    if(window_op==1)     nef = gen_window_1(window_width, window_height);
    else if(window_op==2)nef = gen_window_2(window_width, window_height);
    else if(window_op==3)nef = gen_window_3(window_width, window_height);
    else if(window_op==4)nef = gen_window_4(window_width, window_height);
    else if(window_op==5)nef = gen_window_5(window_width, window_height);
    return nef;
    
}
//This function generates the window style based upon the y row, then calls the gen_windows function.
Nef_polyhedron window_grammar(int shape_type, double window_scale_parameters[], double window_design_parameters[], double window_width, double window_height, double tile_width, double tile_height, int grid_y, int grid_height){
    
    
    Nef_polyhedron nef;
    
    //This is a style rule. If the window scale is 1 (meaning it takes up the entire tile), the program
    //sets the window style to 4.
    if( (window_scale_parameters[0] == 1 && grid_y == 0) || (window_scale_parameters[2] == 1 && grid_y >0 && grid_y<grid_height-1) || (window_scale_parameters[0] == 1 && grid_y == grid_height-1)){
        Nef_polyhedron nef_window = gen_windows(window_width,  1, tile_width, window_height, tile_height);
        if(shape_type==2)
            rotate(0,0,180, nef_window);
        nef+=nef_window;
        
    }
    else{
        //This is used for everything else
        if(grid_y==0){
            Nef_polyhedron nef_window = gen_windows(window_width,  window_design_parameters[0], tile_width, window_height, tile_height);
            if(shape_type==2)
                rotate(0,0,180, nef_window);
            nef+=nef_window;
        }
        else if(grid_y==grid_height-1){
            Nef_polyhedron nef_window = gen_windows(window_width,  window_design_parameters[1], tile_width, window_height, tile_height);
            if(shape_type==2)
                rotate(0,0,180, nef_window);
            nef+=nef_window;
        }
        else{
            Nef_polyhedron nef_window = gen_windows(window_width,  window_design_parameters[2], tile_width, window_height, tile_height);
            if(shape_type==2)
                rotate(0,0,180, nef_window);
            nef+=nef_window;
        }
    }
    
    return nef;
    
    
}

//This function generates the railing by creating it in two steps. One is the bottom and the other the top, where the
//size is based upon the tile sizes and the railing scales.

Nef_polyhedron gen_railing(double tile_width, double tile_height, double railing_scale, double width){
    Nef_polyhedron nef;
    
    Nef_polyhedron nef_top = square(tile_height/5, width);
    rotate(90,90,90,nef_top);
    
    Nef_polyhedron nef_bottom = nef_top;
    scale(railing_scale,1,1, nef_top);
    translate(1,0,tile_height/2-tile_height/5, nef_top);
    
    scale(railing_scale/2,1,1, nef_bottom);
    translate(1,0,tile_height/2-tile_height/3, nef_bottom);
    nef = nef_top+nef_bottom;
    rotate(0,0,180, nef);
    
    return nef;
}


//------------------------------FACADE GRAMMAR--------------------------

//This function generates the facade row and tiles. It does so by looping through the row, and generating a
//tile where a window is placed.
Nef_polyhedron gen_level(std::vector<std::vector <int> > remove_windows,double window_scale_parameters[], double door_parameters[], double grid_bottom_parameters[], double grid_center_parameters[], double grid_top_parameters[], double window_design_parameters[], std::vector <std::vector <int> > railing_parameters, double coord[], double angle[], int grid_width, int grid_height, double width, double height, int shape_type, int door, int y, double width_scale, double height_scale, double vertical_offset, double bottom_height, int side){
    
    //The tile variables is the height/width of each tile or segment of the facade grid.
    double tile_height = (height-bottom_height*2)/grid_height;
    double tile_width = width/grid_width;
    //The window variables are the size of the window based upon the tile parameters.
    double window_height = tile_height/height_scale;
    double window_width = tile_width/width_scale;
    //The bottom window height is the height of the bottom window
    double bottom_window_height = bottom_height*2/height_scale;
    //The door variables are the size of the door based upon the tile parameters.
    double door_height = tile_height/door_parameters[2];
    double door_width = tile_width/door_parameters[1];
    
    Nef_polyhedron nef;
    Nef_polyhedron nef_windows;
    
    //This segment handles the railing generation. It does so by first looping through the
    //railing parameter list and checking if the values stored match the y row of the facade
    //(Or if the values are -1), then calls the gen_railing function.
    
    for(int i = 0; i<railing_parameters.size(); i++){
        
        if((side == railing_parameters[i][2] || railing_parameters[i][2] <0) && (y==railing_parameters[i][1] || railing_parameters[i][1] <0)){
            Nef_polyhedron nef_railing =gen_railing(tile_width, tile_height, railing_parameters[i][0], width);
            translate(0,(width/2),(tile_height/2+bottom_height*2+y*tile_height+vertical_offset), nef_railing);
            nef+= nef_railing;
            std::cout << "railing placed" << std::endl;
            break;
        }
    }
    
    //This segment is identical to the actual tile creation (Further below) but is modifed
    //to generate the bottom row height.
    if(y==0 && bottom_height>0){
        
        nef_windows = window_grammar(shape_type,window_scale_parameters, window_design_parameters, window_width, bottom_window_height, tile_width, tile_height, y, grid_height);
        translate(0, 0, -bottom_height*2+bottom_window_height/2, nef_windows);
        
    }
    //This creates the actual window for the entire row.
    else if(y>0){
        nef_windows = window_grammar(shape_type, window_scale_parameters, window_design_parameters, window_width, window_height, tile_width, tile_height, y, grid_height);
        translate(0, 0, vertical_offset, nef_windows);
        
    }
    
    
    for(int x =0; x<=grid_width-1; x++){
        Nef_polyhedron nef_x;
        
        //This segment checks if there is a door at the x, y value defined in the door_parameters list.
        //If there is, it creates a window for the door and calls the door function.
        if(door == 0 && y==0 && ( x==door_parameters[0]) ){
            nef_x= gen_door(door_width, door_height, tile_width, tile_height, bottom_height*2);
            std::cout << "side: " << side << ", door_y: " << y << ", door_x: " << x << std::endl;
            
        }
        else{
            
            //This checks if the window is to be removed.
            bool remove_window = false;
            for(int i = 0; i<remove_windows.size(); i++){
                
                if( (side==remove_windows[i][2] || remove_windows[i][2]<0) && (x==remove_windows[i][0] || remove_windows[i][0]<0) &&(y==remove_windows[i][1] || remove_windows[i][1]<0) ){
                    remove_window= true;
                    break;
                }
            }
            if(!remove_window){//If the window isn't meant to be removed, it is set to the nef_x.
                nef_x = nef_windows;
                std::cout << "side: " << side << ", y: " << y << ", x: " << x << std::endl;
            }
        }
        
        translate(0, (tile_width/2+x*tile_width),(tile_height/2+bottom_height*2+y*tile_height), nef_x);
        nef+= nef_x;
    }
    
    return nef;
}
//This function splits the facade generation into its y components, meaning it generates the
//bottom row first (y=0),then it moves to the center rows, then finally the top row.
//This allows the bottom, center and top to each have different styles of windows available.
Nef_polyhedron gen_facade(std::vector<std::vector <int> > remove_windows, double door_parameters[],double window_scale_parameters[], double window_design_parameters[], std::vector <std::vector <int> > railing_parameters, double coord[], double angle[], double grid_bottom_parameters[], double grid_center_parameters[], double grid_top_parameters[], double width, double height, int shape_type, int door, int side, double vertical_offset){
    
    Nef_polyhedron nef;
    int grid_height =grid_top_parameters[6];
    
    nef+=gen_level(remove_windows, window_scale_parameters, door_parameters, grid_bottom_parameters, grid_center_parameters,  grid_top_parameters, window_design_parameters, railing_parameters, coord, angle, grid_bottom_parameters[side], grid_height, width, height, shape_type, door, 0, window_scale_parameters[0], window_scale_parameters[1], vertical_offset, grid_bottom_parameters[6], (side+1));
    
    for(int y =1; y<=grid_height-2; y++)
        nef+=gen_level(remove_windows, window_scale_parameters, door_parameters, grid_bottom_parameters, grid_center_parameters,  grid_top_parameters, window_design_parameters, railing_parameters, coord, angle, grid_center_parameters[side], grid_height, width, height, shape_type, door, y, window_scale_parameters[2], window_scale_parameters[3], vertical_offset, grid_bottom_parameters[6], (side+1));
    
    nef+=gen_level(remove_windows, window_scale_parameters, door_parameters, grid_bottom_parameters, grid_center_parameters,  grid_top_parameters, window_design_parameters, railing_parameters, coord, angle, grid_top_parameters[side], grid_height, width, height, shape_type, door, grid_height-1, window_scale_parameters[4], window_scale_parameters[5], vertical_offset, grid_bottom_parameters[6], (side+1));
    
    
    translate(coord[0], coord[1], coord[2], nef);
    rotate(angle[0], angle[1], angle[2], nef);
    
    return nef;
    
}

//This function is essentially a gateway into the actual facade generation.
//Here, it just checks the shape type and sends the coordinates based on the
//shape to the gen_facade function.

Nef_polyhedron facade_grammar(std::vector<std::vector <int> > remove_windows, int sides, int shape_type, double building_parameters[],std::vector <std::vector <int> > railing_parameters, double grid_bottom_parameters[], double grid_center_parameters[], double grid_top_parameters[], double window_scale_parameters[], double  window_design_parameters[], double door_parameters[], double vertical_offset){
    
    double building_width = building_parameters[0];
    double building_length = building_parameters[1];
    double building_height = building_parameters[4];
    
    Nef_polyhedron nef;
    
    //The rectangle facade is split into two parts, one for each side
    //(As the widths for the two side types are different)
    if(shape_type==1){
        
        for(int i = 0; i<=1; i++){
            double coord[3] = {-building_width/2,-building_length/2,-building_height/2};
            double angle[3] {0,0,static_cast<double>(180*i)};
            
            
            nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord, angle, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_length, building_height, shape_type, i, 0, vertical_offset);
        }
        for(int i = 0; i<=1; i++){
            double coord[3] = {-building_length/2,-building_width/2,-building_height/2};
            double angle[3] {0,0,90+static_cast<double>(180*i)};
            
            nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord, angle, grid_bottom_parameters, grid_center_parameters, grid_top_parameters, building_width, building_height, shape_type, 1, 1, vertical_offset);
        }
    }
    //Due to the polygon having the same side length, it is just repeated in a loop for n times
    if(shape_type==2){
        
        double tan_x =tan( (180/sides)*PI/180);
        double sum_angle = (sides-2)*180;
        double side_angle = sum_angle/sides;
        double inradius = building_width/(2*tan_x);//Midpoint of side
        double facade_angle = 180-(side_angle/2+90);
        
        
        double coord[3] = {inradius,-building_width/2,-building_height/2};
        
        //Note: It is sides*2, and being incremented by 2 instead of one due to the
        //facade placement onto the shape.
        for(int i =1; i<=sides*2; i+=2){
            
            double angle[3] {0,0,(facade_angle)*i};
            
            nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord, angle, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_width, building_height, shape_type, (i-1), i/2, vertical_offset);
        }
        
    }
    //Due to the L shape having six different side lengths, gen_facade is called six times.
    else if(shape_type ==3){
        double building_width_2 = building_parameters[2];
        double building_length_2 = building_parameters[3];
        
        double coord1[3] = {-building_length_2/2,-building_width/2,-building_height/2};
        double coord2[3] = {-(building_width/2+building_width_2),-building_length_2/2,-building_height/2};
        double coord3[3] = {-building_width/2,-(building_length_2/2+building_length),-building_height/2};
        double coord4[3] = {-(building_length_2/2+building_length),-building_width/2,-building_height/2};
        double coord5[3] = {-building_length_2/2,-(building_width/2+building_width_2),-building_height/2};
        double coord6[3] = {-building_width/2,building_length_2/2,-building_height/2};
        
        double angle1[3] = {0,0,0};
        double angle2[3] = {0,0,270};
        double angle3[3] = {0,0,90};
        double angle4[3] = {0,0,180};
        double angle5[3] = {0,0,180};
        double angle6[3] = {0,0,270};
        
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord1, angle1, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,(building_width+building_width_2), building_height, shape_type, 0, 0, vertical_offset);
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord2, angle2, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_length_2, building_height, shape_type, 1, 1, vertical_offset);
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord3, angle3, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,(building_length+building_length_2), building_height, shape_type, 1, 2, vertical_offset);
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord4, angle4, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_width, building_height, shape_type, 1, 3, vertical_offset);
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord5, angle5, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_width_2, building_height, shape_type, 1, 4, vertical_offset);
        nef+=gen_facade(remove_windows, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord6, angle6, grid_bottom_parameters, grid_center_parameters, grid_top_parameters,building_length, building_height, shape_type, 1, 5, vertical_offset);
    }
    
    return nef;
    
}


//------------------------------BUIDLING GRAMMAR--------------------------

//The building grammar handles the generation of the actual building shape by first calling shape_grammar, then from
//there calling the various other modules (Rectangle, L, polygon, etc...) to create the unique shape as well as
//applying the overhang onto the building.

//The shape_grammar function takes the entiriety of the parameters then checks what shape type was selected.
//From there, it calls the specific module for generating each shape.
Nef_polyhedron gen_rectangle(double building_parameters[5], double overhang_parameters[3]){
    
    double building_width = building_parameters[0];
    double building_length = building_parameters[1];
    double building_height = building_parameters[4];
    
    double overhang_width = overhang_parameters[0];
    double overhang_thickness = overhang_parameters[1];
    double overhang_height = overhang_parameters[2];
    
    Nef_polyhedron nef_building =  cube(building_width);
    scale(1, building_length/building_width,building_height/building_width, nef_building);
    
    Nef_polyhedron nef_overhang =  square( (building_width+overhang_width*2), (building_length+overhang_width*2));
    scale(1,1,overhang_height, nef_overhang);
    translate(0,0,building_height/2, nef_overhang);
    
    Nef_polyhedron nef_subtract = square( (building_width+overhang_width*2-overhang_thickness*2), (building_length+overhang_width*2-overhang_thickness*2));
    scale(1,1,overhang_height, nef_subtract);
    
    translate(0,0,building_height/2, nef_subtract);
    
    
    Nef_polyhedron nef_roof =  square( (building_width+overhang_width*1.99), (building_length+overhang_width*1.99));
    translate(0,0,building_height/2, nef_roof);
    
    return nef_building+nef_roof+ (nef_overhang-nef_subtract);
    
}

//This function generates a n sided polygon. It does so by calculating the circumradius and using that value to
//generate a n amount of cylinders to be used with the hull function to fill the volume, generating a polygon.
//The overhang works similarly.
Nef_polyhedron gen_polygon(double building_parameters[5], double overhang_parameters[3], int sides){
    double building_width = building_parameters[0];
    double building_height = building_parameters[4];
    
    double overhang_width = overhang_parameters[0];
    double overhang_thickness = overhang_parameters[1];
    double overhang_height = overhang_parameters[2];
    
    Nef_polyhedron nef_building = polygon(0, building_width, building_height, sides);
    Nef_polyhedron nef_roof = polygon(overhang_width*0.9, building_width, overhang_height*0.1, sides);
    Nef_polyhedron nef_overhang = polygon(overhang_width, building_width, overhang_height/2, sides);
    Nef_polyhedron nef_overhang_subtract = polygon(overhang_width-overhang_thickness, building_width, overhang_height/2, sides);
    
    translate(0,0,building_height/2, nef_roof);
    translate(0,0,building_height/2+overhang_height/2, nef_overhang);
    translate(0,0,building_height/2+overhang_height/2*1.01, nef_overhang_subtract);
    
    return nef_building+nef_roof+(nef_overhang-nef_overhang_subtract);
}

//This function generates an L shaped building. It does so by first generating two cubes of width, height 1, 2.
//Then it translates them so that the two widths become one large one. The overhang is created using three
//convex hulls, where each hull is comprised of eight vertices.
Nef_polyhedron gen_L(double building_parameters[5], double overhang_parameters[3]){
    
    double building_width_1 = building_parameters[0];
    double building_length_1 = building_parameters[1];
    double building_width_2 = building_parameters[2];
    double building_length_2 = building_parameters[3];
    double building_height = building_parameters[4];
    
    double overhang_width = overhang_parameters[0];
    double overhang_thickness = overhang_parameters[1];
    double overhang_height = overhang_parameters[2];
    
    //This segment handles the actual L shape generation. Here, it splits the process into two parts,
    //one for each "cube" it generates
    
    Nef_polyhedron nef_cube1 =cube(building_width_1);
    scale((building_length_1+building_length_2)/building_width_1, 1,building_height/building_width_1, nef_cube1);
    
    Nef_polyhedron nef_roof1 =  square(building_length_2+overhang_width*1.8, building_width_2+overhang_width*1.8);
    translate(0,0,building_height/2, nef_roof1);
    
    Nef_polyhedron nef_cube2 =  cube(building_width_2);
    scale(building_length_2/building_width_2,1,building_height/building_width_2, nef_cube2);
    nef_cube2+=nef_roof1;
    translate(-building_length_1/2,(building_width_1+building_width_2)/2,0, nef_cube2);
    
    
    Nef_polyhedron nef_roof2 = square( (building_length_1+building_length_2+overhang_width*1.8), building_width_1+overhang_width*1.8);
    translate(0,0,building_height/2, nef_roof2);
    
    Nef_polyhedron building_L = nef_cube1+nef_cube2+nef_roof2;
    translate(building_length_1/2,0,0, building_L);
    
    //The convex hull generation for the overhang.
    
    double coord_overhang_1[8][3] = {
        {building_length_2/2+building_length_1+overhang_width,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+building_length_1+overhang_width,-building_width_1/2-overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+building_length_1+overhang_width,building_width_1/2+overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+building_length_1+overhang_width,-building_width_1/2-overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width,building_height/2-overhang_height/2}
    };
    double coord_overhang_subtract_1[8][3] = {
        {building_length_2/2+building_length_1+overhang_width-overhang_thickness,building_width_1/2+overhang_width-overhang_thickness,building_height/2+overhang_height},
        {building_length_2/2+building_length_1+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2+overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width-overhang_thickness,building_height/2+overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2+overhang_height},
        {building_length_2/2+building_length_1+overhang_width-overhang_thickness,building_width_1/2+overhang_width-overhang_thickness,building_height/2-overhang_height},
        {building_length_2/2+building_length_1+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2-overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width-overhang_thickness,building_height/2-overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2-overhang_height}
    };
    double coord_overhang_2[8][3] ={
        {-building_length_2/2-overhang_width,-building_width_1/2-overhang_width,building_height/2+overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width,building_height/2+overhang_height/2},
        {-building_length_2/2-overhang_width,-building_width_1/2-overhang_width,building_height/2-overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width,building_height/2-overhang_height/2}
    };
    double coord_overhang_subtract_2[8][3] ={
        {-building_length_2/2-overhang_width+overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2+overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/2+overhang_width,building_height/2+overhang_height},
        {building_length_2+overhang_width-overhang_thickness,building_width_1+overhang_width,building_height/2+overhang_height},
        {building_length_2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2+overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2-overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/2+overhang_width,building_height/2-overhang_height},
        {building_length_2+overhang_width-overhang_thickness,building_width_1+overhang_width,building_height/2-overhang_height},
        {building_length_2+overhang_width-overhang_thickness,-building_width_1/2-overhang_width+overhang_thickness,building_height/2-overhang_height}
    };
    double coord_overhang_3[8][3] = {
        {building_length_2/2+overhang_width,building_width_1/2+building_width_2+overhang_width,building_height/2+overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+building_width_2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+overhang_width,building_height/2+overhang_height/2},
        {building_length_2/2+overhang_width,building_width_1/2+building_width_2+overhang_width,building_height/2-overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+building_width_2+overhang_width,building_height/2-overhang_height/2},
        {building_length_2/2+overhang_width,building_width_1/2+overhang_width,building_height/2-overhang_height/2},
        {-building_length_2/2-overhang_width,building_width_1/2+overhang_width,building_height/2-overhang_height/2}
    };
    double coord_overhang_subtract_3[8][3] = {
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+building_width_2+overhang_width-overhang_thickness,building_height/2+overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/2+building_width_2+overhang_width-overhang_thickness,building_height/2+overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2+overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/4+overhang_width,building_height/2+overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+building_width_2+overhang_width-overhang_thickness,building_height/2-overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/2+building_width_2+overhang_width-overhang_thickness,building_height/2-overhang_height},
        {building_length_2/2+overhang_width-overhang_thickness,building_width_1/2+overhang_width,building_height/2-overhang_height},
        {-building_length_2/2-overhang_width+overhang_thickness,building_width_1/4+overhang_width,building_height/2-overhang_height}
    };
    //Converting the convex poyhedron hulls into nefs.
    Polyhedron_3 polygon_overhang_1;
    hull(polygon_overhang_1, coord_overhang_1, 8);
    Nef_polyhedron nef_overhang_1(polygon_overhang_1);
    
    Polyhedron_3 polygon_overhang_subtract_1;
    hull(polygon_overhang_subtract_1, coord_overhang_subtract_1, 8);
    Nef_polyhedron nef_overhang_subtract_1(polygon_overhang_subtract_1);
    
    Polyhedron_3 polygon_overhang_2;
    hull(polygon_overhang_2, coord_overhang_2, 8);
    Nef_polyhedron nef_overhang_2(polygon_overhang_2);
    
    Polyhedron_3 polygon_overhang_subtract_2;
    hull(polygon_overhang_subtract_2, coord_overhang_subtract_2, 8);
    Nef_polyhedron nef_overhang_subtract_2(polygon_overhang_subtract_2);
    
    Polyhedron_3 polygon_overhang_3;
    hull(polygon_overhang_3, coord_overhang_3, 8);
    Nef_polyhedron nef_overhang_3(polygon_overhang_3);
    
    Polyhedron_3 polygon_overhang_subtract_3;
    hull(polygon_overhang_subtract_3, coord_overhang_subtract_3, 8);
    Nef_polyhedron nef_overhang_subtract_3(polygon_overhang_subtract_3);
    
    //Each of the three parts comprises two hulls being differenced.
    Nef_polyhedron overhang_1 =nef_overhang_1-nef_overhang_subtract_1;
    Nef_polyhedron overhang_2 =nef_overhang_2-nef_overhang_subtract_2;
    Nef_polyhedron overhang_3 =nef_overhang_3-nef_overhang_subtract_3;
    
    return building_L+overhang_1+overhang_2+overhang_3;
    
}

//------------------------------MAIN FUNCTIONS--------------------------

//These functions form the basis for the program, and don't involve any
//actual polyhedron generation.

//This functions takes the entire parameter set and outputs it to an
//output text file. This file can later have its contents copied and used
//to generate the same building with the "input_parameters.txt" file.

void output_paramaters_txt(std::vector<std::vector <int> > remove_windows, int sides, int shape_type, double building_parameters[],std::vector <std::vector <int> > railing_parameters, double grid_bottom_parameters[], double grid_center_parameters[], double grid_top_parameters[], double window_scale_parameters[], double  window_design_parameters[], double door_parameters[],double vertical_offset, double overhang_parameters[], int num){
    
    std::ofstream output_file ("output_parameters_"+std::to_string(num)+".txt");
    if (output_file.is_open())
    {
        
        output_file << "Shape Type = "+std::to_string(shape_type)+"\n";
        output_file << "Sides = "+std::to_string(sides)+"\n";
        
        output_file << "Building Width 1 = "+std::to_string(building_parameters[0])+"\n";
        output_file << "Building Length 1 = "+std::to_string(building_parameters[1])+"\n";
        output_file << "Building Width 2 = "+std::to_string(building_parameters[2])+"\n";
        output_file << "Building Length 2 = "+std::to_string(building_parameters[3])+"\n";
        output_file << "Building Height = "+std::to_string(building_parameters[4])+"\n";
        output_file << "Overhang Width = "+std::to_string(overhang_parameters[0])+"\n";
        output_file << "Overhang Thickness = "+std::to_string(overhang_parameters[1])+"\n";
        output_file << "Overhang Height = "+std::to_string(overhang_parameters[2])+"\n";
        
        output_file << "Window Bottom Width Scale = "+std::to_string(window_scale_parameters[0])+"\n";
        output_file << "Window Bottom Width Scale = "+std::to_string(window_scale_parameters[1])+"\n";
        output_file << "Window Centre Width Scale = "+std::to_string(window_scale_parameters[2])+"\n";
        output_file << "Window Centre Width Scale = "+std::to_string(window_scale_parameters[3])+"\n";
        output_file << "Window Top Width Scale = "+std::to_string(window_scale_parameters[4])+"\n";
        output_file << "Window Top Width Scale = "+std::to_string(window_scale_parameters[5])+"\n";
        
        output_file << "Grid Bottom Width = "+std::to_string(grid_bottom_parameters[0])+"\n";
        output_file << "Grid Bottom Length = "+std::to_string(grid_bottom_parameters[1])+"\n";
        output_file << "Grid Bottom Side 3 = "+std::to_string(grid_bottom_parameters[2])+"\n";
        output_file << "Grid Bottom Side 4 = "+std::to_string(grid_bottom_parameters[3])+"\n";
        output_file << "Grid Bottom Side 5 = "+std::to_string(grid_bottom_parameters[4])+"\n";
        output_file << "Grid Bottom Side 6 = "+std::to_string(grid_bottom_parameters[5])+"\n";
        output_file << "Grid Bottom Tile Height = "+std::to_string(grid_bottom_parameters[6])+"\n";
        
        output_file << "Grid Centre Width = "+std::to_string(grid_center_parameters[0])+"\n";
        output_file << "Grid Centre Length = "+std::to_string(grid_center_parameters[1])+"\n";
        output_file << "Grid Centre Side 3 = "+std::to_string(grid_center_parameters[2])+"\n";
        output_file << "Grid Centre Side 4 = "+std::to_string(grid_center_parameters[3])+"\n";
        output_file << "Grid Centre Side 5 = "+std::to_string(grid_center_parameters[4])+"\n";
        output_file << "Grid Centre Side 6 = "+std::to_string(grid_center_parameters[5])+"\n";
        
        output_file << "Grid Top Width = "+std::to_string(grid_top_parameters[0])+"\n";
        output_file << "Grid Top Length = "+std::to_string(grid_top_parameters[1])+"\n";
        output_file << "Grid Top Side 3 = "+std::to_string(grid_top_parameters[2])+"\n";
        output_file << "Grid Top Side 4 = "+std::to_string(grid_top_parameters[3])+"\n";
        output_file << "Grid Top Side 5 = "+std::to_string(grid_top_parameters[4])+"\n";
        output_file << "Grid Top Side 6 = "+std::to_string(grid_top_parameters[5])+"\n";
        output_file << "Grid Height = "+std::to_string(grid_top_parameters[6])+"\n";
        
        output_file << "Vertical Offset = "+std::to_string(vertical_offset)+"\n";
        
        output_file << "Window Bottom Design = "+std::to_string(window_design_parameters[0])+"\n";
        output_file << "Window Center Design = "+std::to_string(window_design_parameters[1])+"\n";
        output_file << "Window Top Design = "+std::to_string(window_design_parameters[2])+"\n";
        
        output_file << "Door Location = "+std::to_string(door_parameters[0])+"\n";
        output_file << "Door Width Scale = "+std::to_string(door_parameters[1])+"\n";
        output_file << "Door Height Scale = "+std::to_string(door_parameters[2])+"\n";
        
        if(remove_windows.size() >0){
            std::string remove_windows_line = "Remove Window = ";
            
            for(int i =0;i< remove_windows.size(); i++){
                remove_windows_line+="(";
                
                
                for(int j=0; j<3; j++){
                    remove_windows_line+=std::to_string(remove_windows[i][j]);
                    if(j<2) remove_windows_line+=",";
                }
                remove_windows_line+=")";
                if(i<remove_windows.size()-1) remove_windows_line+=",";
            }
            output_file << remove_windows_line+"\n";
        }
        
        if(railing_parameters.size()>0){
            std::string railing_line = "Railings = ";
            for(int i =0;i< railing_parameters.size(); i++){
                railing_line+="(";
                
                for(int j=0; j<3; j++){
                    railing_line+=std::to_string(railing_parameters[i][j]);
                    if(j<2) railing_line+=",";
                }
                railing_line+=")";
                if(i<railing_parameters.size()-1) railing_line+=",";
            }
            output_file << railing_line+"\n";
        }
        
        output_file.close();
    }
    else std::cout << "Unable to open file";
    
    
}
//This function handles the actual grammar/pipeline creation for the building.
//Here, it checks the shape type and calls the corresponding functions as well as
//rename the file_name variable.

Nef_polyhedron grammar(std::vector < std::vector<int>> remove_windows, int sides, int shape_type, double building_parameters[],std::vector <std::vector <int> > railing_parameters, double grid_bottom_parameters[], double grid_center_parameters[], double grid_top_parameters[], double window_scale_parameters[], double  window_design_parameters[], double door_parameters[], double vertical_offset, double overhang_parameters[], int num){
    Nef_polyhedron nef;
    
    if(shape_type==1){
        std::cout <<"Rectangular Cube, sides: " << 2 << ", grid height: " << grid_top_parameters[6] << std::endl;
        file_name = "DSRI_CGAL_mesh_rectangle_"+std::to_string(num)+".off";
        Nef_polyhedron nef_building = gen_rectangle(building_parameters, overhang_parameters);
        Nef_polyhedron nef_window = facade_grammar(remove_windows, sides, shape_type, building_parameters, railing_parameters, grid_bottom_parameters,grid_center_parameters, grid_top_parameters, window_scale_parameters, window_design_parameters, door_parameters, vertical_offset);
        nef= nef_building+nef_window;
        
    }
    else if(shape_type==2){
        std::cout <<"Polygon, sides " << sides << ", grid height: " << grid_top_parameters[6] << std::endl;
        file_name = "DSRI_CGAL_mesh_polygon_"+std::to_string(num)+".off";
        Nef_polyhedron nef_building = gen_polygon(building_parameters, overhang_parameters, sides);
        Nef_polyhedron nef_window =facade_grammar(remove_windows, sides, shape_type, building_parameters, railing_parameters, grid_bottom_parameters,grid_center_parameters, grid_top_parameters, window_scale_parameters, window_design_parameters, door_parameters, vertical_offset);
        nef= nef_building+nef_window;
    }
    
    else if(shape_type ==3){
        std::cout <<"L Shape, sides " << 6 << ", grid height: " << grid_top_parameters[6] << std::endl;
        file_name = "DSRI_CGAL_mesh_L.off_"+std::to_string(num)+".off";
        Nef_polyhedron nef_building = gen_L(building_parameters, overhang_parameters);
        Nef_polyhedron nef_window =facade_grammar(remove_windows, sides, shape_type, building_parameters, railing_parameters, grid_bottom_parameters,grid_center_parameters, grid_top_parameters, window_scale_parameters, window_design_parameters, door_parameters, vertical_offset);
        nef= nef_building+nef_window;
    }
    
    //NOTE: Use this if you are testing the program.
    else if(shape_type ==4){//TESTING
        file_name = "DSRI_CGAL_mesh_TEST.off_"+std::to_string(num)+".off";
        
    }
    //This centers the building so it can be placed on a ground without any other resource.
    //Remove if you wish the building is to be centered on (0,0,0).
    translate(0,0, building_parameters[4]/2, nef);
    return nef;
}

//This function handles the parameter mangement of the building,
//and creates the parameters to be used as well as read from the input file,
//and finally calls the buidling generation.
Nef_polyhedron parameter_management(int num, std::string input_file){
    
    //These values are the default ranges used (If the input file is empty).
    int shape_range[2] ={2,2};
    int side_range[2] = {3,6};
    double building_range[10] = {200,1000, 200,1000,200,1000, 200,1000, 200,1000};
    double overhang_range[6] = {-1,-1, -1,-1,-1,-1};
    double window_scale_range[12] = {2,2, 2,2, 2,2, 2,2, 2,2, 2,2};
    double grid_top_range[14] = {2,4, 2,4, 2,4, 2,4, 2,4, 2,4, 2,5};
    double grid_center_range[14] = {2,4, 2,4, 2,4, 2,4, 2,4, 2,4, 2,5};
    double grid_bottom_range[14] = {3,5, 3,5, 3,5, 3,5, 3,5, 3,5, 100,250};
    int window_design_range[6] ={-1,-1, -1,-1, -1,-1};
    double door_range[6] ={-1,-1, 2,2, 2,2};
    
    //The below code generates a random number for each parameter.
    int shape_type;
    if(shape_range[0] ==-1)shape_type = randINum(1, 3); else shape_type = randINum(shape_range[0], shape_range[1]);
    
    int sides=4;
    while(sides==4)
        if(side_range[0]==-1)sides = randINum(3, 6); else sides=randINum(side_range[0], side_range[1]);
    
    double building_parameters[5];//{width1, length1, width2, length2, height}
    if(building_range[0] ==-1)building_parameters[0] = randDNum(100, 1000);else building_parameters[0] = randDNum(building_range[0], building_range[1]);
    if(building_range[2] ==-1)building_parameters[1] = randDNum(100, 1000);else building_parameters[1] = randDNum(building_range[2], building_range[3]);
    if(building_range[4] ==-1)building_parameters[2] = randDNum(100, 1000);else building_parameters[2] = randDNum(building_range[4], building_range[5]);
    if(building_range[6] ==-1)building_parameters[3] = randDNum(100, 1000);else building_parameters[3] = randDNum(building_range[6], building_range[7]);
    if(building_range[8] ==-1)building_parameters[4] = randDNum(100, 1000);else building_parameters[4] = randDNum(building_range[8], building_range[9]);
    
    double overhang_parameters[3];//[width, thickness, height]
    if(overhang_range[0]==-1)overhang_parameters[0] = randDNum(building_parameters[4]/30,building_parameters[4]/20); else overhang_parameters[0] = randDNum(overhang_range[0],overhang_range[1]);
    if(overhang_range[2]==-1)overhang_parameters[1] = randDNum(building_parameters[4]/30,building_parameters[4]/20); else overhang_parameters[1] = randDNum(overhang_range[2], overhang_range[3]);
    if(overhang_range[4]==-1)overhang_parameters[2] = randDNum(building_parameters[4]/30,building_parameters[4]/20); else overhang_parameters[2] = randDNum(overhang_range[4],overhang_range[5]);
    
    double window_scale_parameters[6];//{bottom_width, bottom_height, center_width, center_height, top_width, top_height}
    if(window_scale_range[0]==-1)window_scale_parameters[0] = randINum(1, 3); else window_scale_parameters[0] = randINum(window_scale_range[0], window_scale_range[1]);
    if(window_scale_range[0]==-1)window_scale_parameters[1] = randINum(1, 2); else window_scale_parameters[1] = randINum(window_scale_range[2], window_scale_range[3]);
    if(window_scale_range[0]==-1)window_scale_parameters[2] = randINum(1, 3); else window_scale_parameters[2] = randINum(window_scale_range[4], window_scale_range[5]);
    if(window_scale_range[0]==-1)window_scale_parameters[3] = randINum(1, 2); else window_scale_parameters[3] = randINum(window_scale_range[6], window_scale_range[7]);
    if(window_scale_range[0]==-1)window_scale_parameters[4] = randINum(1, 3); else window_scale_parameters[4] = randINum(window_scale_range[8], window_scale_range[9]);
    if(window_scale_range[0]==-1)window_scale_parameters[5] = randINum(1, 2); else window_scale_parameters[5] = randINum(window_scale_range[10], window_scale_range[11]);
    
    double grid_top_parameters[8];//{width, length, side3, side4, side5, side6, height, style}
    if(grid_top_range[0]==-1) grid_top_parameters[0] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_top_parameters[0] = randINum(grid_top_range[0], grid_top_range[1]);
    if(grid_top_range[2]==-1) grid_top_parameters[1] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_top_parameters[1] = randINum(grid_top_range[2], grid_top_range[3]);
    if(grid_top_range[4]==-1) grid_top_parameters[2] = randINum((building_parameters[1]+building_parameters[3])/80,(building_parameters[1]+building_parameters[3])/50); else grid_top_parameters[2] = randINum(grid_top_range[4], grid_top_range[5]);
    if(grid_top_range[6]==-1) grid_top_parameters[3] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_top_parameters[3] = randINum(grid_top_range[6], grid_top_range[7]);
    if(grid_top_range[8]==-1) grid_top_parameters[4] = randINum(building_parameters[2]/80,building_parameters[2]/50); else grid_top_parameters[4] = randINum(grid_top_range[8], grid_top_range[9]);
    if(grid_top_range[10]==-1)grid_top_parameters[5] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_top_parameters[5] = randINum(grid_top_range[10], grid_top_range[11]);
    if(grid_top_range[12]==-1)grid_top_parameters[6] = randINum(grid_top_parameters[0]/2,grid_top_parameters[0]*2); else grid_top_parameters[6] = randINum(grid_top_range[12], grid_top_range[13]);
    grid_top_parameters[7] = 1;
    
    double grid_center_parameters[8];//{width, length, side3, side4, side5, side6, height, style}
    if(grid_center_range[0]==-1)grid_center_parameters[0] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_center_parameters[0] = randINum(grid_center_range[0], grid_center_range[1]);
    if(grid_center_range[2]==-1)grid_center_parameters[1] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_center_parameters[1] = randINum(grid_center_range[2], grid_center_range[3]);
    if(grid_center_range[4]==-1)grid_center_parameters[2] = randINum((building_parameters[1]+building_parameters[3])/80,(building_parameters[1]+building_parameters[3])/50); else grid_center_parameters[2] = randINum(grid_center_range[4], grid_center_range[5]);
    if(grid_center_range[6]==-1)grid_center_parameters[3] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_center_parameters[3] = randINum(grid_center_range[6], grid_center_range[7]);
    if(grid_center_range[8]==-1)grid_center_parameters[4] = randINum(building_parameters[2]/80,building_parameters[2]/50); else grid_center_parameters[4] = randINum(grid_center_range[8], grid_center_range[9]);
    if(grid_center_range[10]==-1)grid_center_parameters[5] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_center_parameters[5] = randINum(grid_center_range[10], grid_center_range[11]);
    if(grid_center_range[12]==-1)grid_center_parameters[6] = randINum(grid_top_parameters[0]/2,grid_top_parameters[0]*2); else grid_center_parameters[6] = randINum(grid_center_range[12], grid_center_range[13]);
    grid_center_parameters[7] = 1;
    
    double grid_bottom_parameters[8];//{width, length, side3, side4, side5, side6, height, style}
    if(grid_bottom_range[0]==-1)grid_bottom_parameters[0] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_bottom_parameters[0] = randINum(grid_bottom_range[0], grid_bottom_range[1]);
    if(grid_bottom_range[2]==-1)grid_bottom_parameters[1] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_bottom_parameters[1] = randINum(grid_bottom_range[2], grid_bottom_range[3]);
    if(grid_bottom_range[4]==-1)grid_bottom_parameters[2] = randINum((building_parameters[1]+building_parameters[3])/80,(building_parameters[1]+building_parameters[3])/50); else grid_bottom_parameters[2] = randINum(grid_bottom_range[4], grid_bottom_range[5]);
    if(grid_bottom_range[6]==-1)grid_bottom_parameters[3] = randINum(building_parameters[0]/80,building_parameters[0]/50); else grid_bottom_parameters[3] = randINum(grid_bottom_range[6], grid_bottom_range[7]);
    if(grid_bottom_range[8]==-1)grid_bottom_parameters[4] = randINum(building_parameters[2]/80,building_parameters[2]/50); else grid_bottom_parameters[4] = randINum(grid_bottom_range[8], grid_bottom_range[9]);
    if(grid_bottom_range[10]==-1)grid_bottom_parameters[5] = randINum(building_parameters[1]/80,building_parameters[1]/50); else grid_bottom_parameters[5] = randINum(grid_bottom_range[10], grid_bottom_range[11]);
    if(grid_bottom_range[12]==-1)grid_bottom_parameters[6] = randDNum(building_parameters[4]/grid_top_parameters[6],building_parameters[4]/10); else grid_bottom_parameters[6] = randDNum(grid_bottom_range[12], grid_bottom_range[13]);
    grid_bottom_parameters[7] = 1;
    
    double window_design_parameters[3];//{bottom, center, top}
    if(window_design_range[0]==-1)window_design_parameters[0] = randINum(1, 5); else window_design_parameters[0] = randINum(window_design_range[0], window_design_range[1]);
    if(window_design_range[2]==-1)window_design_parameters[1] = randINum(1, 5); else window_design_parameters[1] = randINum(window_design_range[2], window_design_range[3]);
    if(window_design_range[4]==-1)window_design_parameters[2] = randINum(1, 5); else window_design_parameters[2] = randINum(window_design_range[4], window_design_range[5]);
    
    double door_parameters[3]; //{position, width_scale, height_scale}
    if(door_range[0] ==-1) door_parameters[0]=1;else door_parameters[0] = randDNum(door_range[0], door_range[1]);
    if(door_range[2] ==-1)door_parameters[1] =2;else door_parameters[1] = randDNum(door_range[2], door_range[3]);
    if(door_range[4] ==-1)door_parameters[2] =2;else door_parameters[2] = randDNum(door_range[4], door_range[5]);
    
    std::vector <std::vector <int> > railing_parameters;
    std::vector<std::vector <int> > remove_windows;
    
    double vertical_offset =-(grid_bottom_parameters[6]/2);
    
    Nef_polyhedron nef;
    
    //The below segment reads the input file. If a number is found, it will replace the number
    //generated above with the inputed number.
    
    std::string word = ",";
    std::string line;
    
    std::ifstream myfile (input_file);
    if (myfile.is_open()){
        
        while ( getline (myfile,line) ){
            
            
            std::string data = line.substr(0, line.find(" = ", 0));
            std::string numbers = line.substr(line.find("= ") + 1);
            
            if(line.substr(0, 2).compare("//")){
                
                //Each of the many if statements first check if the number is a range or not, and
                //handles it depending on that.
                if (!data.compare("Shape Type")){
                    if(line.find(",") != std::string::npos)shape_type=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else shape_type=atof(numbers.c_str());
                }
                else if (!data.compare("Sides")){
                    if(line.find(",") != std::string::npos)sides=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else sides=atof(numbers.c_str());
                }
                else if (!data.compare("Building Width 1")){
                    if(line.find(",") != std::string::npos)building_parameters[0]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else building_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Building Length 1")){
                    if(line.find(",") != std::string::npos)building_parameters[1]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else building_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Building Width 2")){
                    if(line.find(",") != std::string::npos)building_parameters[2]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else building_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Building Length 2")){
                    if(line.find(",") != std::string::npos)building_parameters[3]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else building_parameters[3]=atof(numbers.c_str());
                }
                else if (!data.compare("Building Height")){
                    if(line.find(",") != std::string::npos)building_parameters[4]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else building_parameters[4]=atof(numbers.c_str());
                }
                else if (!data.compare("Overhang Width")){
                    if(line.find(",") != std::string::npos)overhang_parameters[0]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else overhang_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Overhang Thickness")){
                    if(line.find(",") != std::string::npos)overhang_parameters[1]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else overhang_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Overhang Height")){
                    if(line.find(",") != std::string::npos)overhang_parameters[2]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else overhang_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Bottom Width Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[0]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Bottom Height Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[1]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Centre Width Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[2]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Centre Height Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[3]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[3]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Top Width Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[4]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[4]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Top Height Scale")){
                    if(line.find(",") != std::string::npos)window_scale_parameters[5]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_scale_parameters[5]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Width")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[0]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Length")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[1]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Side 3")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[2]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Side 4")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[3]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[3]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Side 5")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[4]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[4]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Bottom Side 6")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[5]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[5]=atof(numbers.c_str());
                }
                else if (!data.compare("Bottom Tile Height")){
                    if(line.find(",") != std::string::npos)grid_bottom_parameters[6]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_bottom_parameters[6]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Width")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[0]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Length")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[1]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Side 3")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[2]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Side 4")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[3]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[3]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Side 5")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[4]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[4]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Centre Side 6")){
                    if(line.find(",") != std::string::npos)grid_center_parameters[5]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_center_parameters[5]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Width")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[0]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Length")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[1]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Side 3")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[2]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Side 4")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[3]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[3]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Side 5")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[4]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[4]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Top Side 6")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[5]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[5]=atof(numbers.c_str());
                }
                else if (!data.compare("Grid Height")){
                    if(line.find(",") != std::string::npos)grid_top_parameters[6]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else grid_top_parameters[6]=atof(numbers.c_str());
                }
                
                else if (!data.compare("Vertical Offset")){
                    if(line.find(",") != std::string::npos)vertical_offset=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else vertical_offset=atof(numbers.c_str());
                }
                else if (!data.compare("Window Bottom Design")){
                    if(line.find(",") != std::string::npos)window_design_parameters[0]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_design_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Centre Design")){
                    if(line.find(",") != std::string::npos)window_design_parameters[1]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_design_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Window Top Design")){
                    if(line.find(",") != std::string::npos)window_design_parameters[2]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else window_design_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Door Location")){
                    if(line.find(",") != std::string::npos)door_parameters[0]=randINum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else door_parameters[0]=atof(numbers.c_str());
                }
                else if (!data.compare("Door Width Scale")){
                    if(line.find(",") != std::string::npos)door_parameters[1]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else door_parameters[1]=atof(numbers.c_str());
                }
                else if (!data.compare("Door Height Scale")){
                    if(line.find(",") != std::string::npos)door_parameters[2]=randDNum(atof(numbers.substr(0, numbers.find(",", 0)).c_str()), atof(numbers.substr(numbers.find(",") + 1).c_str()));
                    else door_parameters[2]=atof(numbers.c_str());
                }
                else if (!data.compare("Remove Window")){
                    numbers = numbers.substr(numbers.find(" ") + 1);
                    int window_num=std::count(numbers.begin(),numbers.end(),'(');
                    
                    for(int i =0; i<window_num; i++){
                        
                        std::string window_coord = numbers.substr(0, numbers.find(",(", 0));
                        numbers = numbers.substr(numbers.find(",(") + 1);
                        
                        window_coord = window_coord.substr(0, window_coord.find(")", 0));
                        window_coord = window_coord.substr(window_coord.find("(") + 1);
                        
                        std::vector <int> coord_vector;
                        for(int j =0; j<3; j++){
                            
                            coord_vector.push_back(atof(window_coord.substr(0, window_coord.find(",", 0)).c_str()));
                            
                            window_coord = window_coord.substr(window_coord.find(",") + 1);
                            
                        }
                        remove_windows.push_back(coord_vector);
                        
                    }
                }
                
                else if (!data.compare("Railings")){
                    numbers = numbers.substr(numbers.find(" ") + 1);
                    int railing_num=std::count(numbers.begin(),numbers.end(),'(');
                    
                    for(int i =0; i<railing_num; i++){
                        
                        std::string railing = numbers.substr(0, numbers.find(",(", 0));
                        numbers = numbers.substr(numbers.find(",(") + 1);
                        
                        railing = railing.substr(0, railing.find(")", 0));
                        railing = railing.substr(railing.find("(") + 1);
                        
                        std::vector <int> coord_vector;
                        for(int j =0; j<3; j++){
                            
                            coord_vector.push_back(atof(railing.substr(0, railing.find(",", 0)).c_str()));
                            
                            railing = railing.substr(railing.find(",") + 1);
                            
                        }
                        railing_parameters.push_back(coord_vector);
                        
                    }
                    
                }
            }
        }
        myfile.close();
    }
    
    else std::cout << "Unable to open file";
    
    //Due to the 2nd shape type being a polygon of n sides, all the sides in the grid parameters
    //need to be set to the first one.
    if(shape_type==2){
        for(int i =1; i<6; i++){
            grid_top_parameters[i] = grid_top_parameters[0];
            grid_center_parameters[i] = grid_center_parameters[0];
            grid_bottom_parameters[i] = grid_bottom_parameters[0];
        }
    }
    
    
    output_paramaters_txt(remove_windows, sides, shape_type, building_parameters, railing_parameters, grid_bottom_parameters,grid_center_parameters, grid_top_parameters, window_scale_parameters, window_design_parameters, door_parameters, vertical_offset, overhang_parameters, num);
    nef += grammar(remove_windows, sides, shape_type, building_parameters, railing_parameters, grid_bottom_parameters,grid_center_parameters, grid_top_parameters, window_scale_parameters, window_design_parameters, door_parameters, vertical_offset, overhang_parameters, num);
    
    return nef;
}
//------------------------------MAIN--------------------------

int main(){
    srand(time(NULL));
    gen_poly();
    gen_cylinder(1, fn);
    
    Nef_polyhedron nef;
    
    nef +=parameter_management(1, "input_parameters.txt");
    
    //The below converts the nef_polyhedron into a surface mesh to be outputted.
    Surface_mesh output;
    CGAL::convert_nef_polyhedron_to_polygon_mesh(nef, output);
    std::ofstream out;
    out.open(file_name);
    out << output;
    out.close();
    
    std::cout << "Saved file as: " << file_name << std::endl;
    
}


