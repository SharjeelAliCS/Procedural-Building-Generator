/*
|**********************************************************************;
* Project           : Procedural Building Generation
* Program name      : DSRI_OPENSCAD_BUILDING_GENERATION.scad
* Author            : Ali Sharjeel
* Date created      : 2018/07/20
* Purpose           : This program allows the user to procedurally generate a 
                      random building of different sizes, different shapes,
                      different amount of windows, etc... The program also allows
                      the user to manually type and choose their own parameters
                      instead of having the program generate them randomly. 
|**********************************************************************;
*/

//------------------------------VARIABLES--------------------------

//COLOR PARAMETERS: [window, style, tile, detail, door, roof]
color_0 = ["lightblue", "Maroon", "tan", "black", "black", "grey"];
color_1 = ["lightblue", "lightgrey", "grey", "lightgrey", "black", "grey"];
color_2 = ["DimGray", "DarkGray", "white", "black", "black", "grey"];
color_3 = [[50/255, 50/255, 50/255, 255/255], [140/255, 115/255, 85/255, 255/255], "Wheat", "Ivory", "black", "grey"];
color_4 = ["SteelBlue", "Gainsboro", "FloralWhite", "black", "black", "grey"];
color_parameters = [color_0, color_1, color_2, color_3, color_4];

//------------------------------PARAMETERS--------------------------
RANDOM_ON = false; //Set to true if you wish to use the randomizer. 

shape_type = 1; //1 = recangle, 2 = polygon, 3 = L shape. 
sides = 5; //Number of sides for the polygon shape. 
color_style = 2; //Choose a color style from the color parameters list above, and set the value here. 

building_parameters = [300,500,500,200,500];//[width1, length1, width2,length2,height]
overhang_parameters = [30, 5, 20];//[width, thickness, height]
railing_parameters = [50, [1,2], [0,2,4]];//[scale, grid#, side#] Note: -1 in the grid# applies the railing to all y rows. 

grid_top_parameters=[3, 2, 6, 6, 6, 6, 0, 0];//[width, length, side3, side4, side5, side6, height, style]
grid_center_parameters = [3, 2, 6, 6, 6, 6, 0, 0];//[width, length, side3, side4, side5, side6, height, style]
grid_bottom_parameters=[5, 3, 6, 6, 6, 6, 70, 1];//[width, length, side3, side4, side5, side6, height, style]
grid_parameters = [grid_bottom_parameters, grid_center_parameters, grid_top_parameters, 3, -50];//[bottom, center, top, height, vertical_offset]

window_scale_parameters =[2,1,2,2,2,2];//[bottom_width, bottom_height, center_width, center_height, top_width, top_height]
window_design_parameters = [2, 4, 2];//[bottom, center, top]
door_parameters=[(2), 2, 1.5];//[position, width_scale, height_scale]

top = 0; //The # of stacking (buildings ontop of one another) that can be done. 
top_scale = 0.5; //The scale of each building being stacked relative to the one below it. 

coord = 0; //Where the entire building should be placed. 
angle = 0; //The angle of the entire building. 
grid_scale1 = 100; //The scaling for the random grid number. (building_width/grid_scale1)
grid_scale2 = 70;  //The scaling for the random grid number. (building_width/grid_scale2)

//The following ranges is relativly simple to understand. The left value is the min value, right is the max value. 
//Then the program chooses a random number based upon the range. 
top_scale_range = [1.2, 2]; 
color_range =[0,4];
shape_range =[1,3];
building_range =[100,1000];
window_width_scale_range = [4,4];
window_height_scale_range = [1,2];
window_design_range = [1,1];
door_scale_amount = [0];
door_location_range = [0]; 

        


//----------------------------MAIN------------------------------


//This module generates the random parameters to be used for the building, then calls the shape_grammar module. 
module main_randomizer(top, top_scale_range, color_range, shape_range, building_range, window_width_scale_range, window_height_scale_range, window_design_range, railing_list,  door_scale_amount, door_location_range, railing_parameters, coord, angle, grid_scale1, grid_scale2){    
        
    //------------------------------RANDOM NUMBER GENERATOR --------------------------

    shape_type = round(rands(shape_range[0],shape_range[1],1)[0]);

    sides = round(rands(3,8,1)[0]);
    color_style = round(rands(color_range[0], color_range[1],1)[0]);

    building_parameters = rands(building_range[0], building_range[1],5);//[width1, length1, width2,length2,height]
    overhang_parameters = rands(building_parameters[4]/30,building_parameters[4]/20,3);//[width, thickness, height]

    top_grid_width = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);
    top_grid_length = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    top_side3 = round(rands((building_parameters[1]+building_parameters[3])/grid_scale1,(building_parameters[1]+building_parameters[3])/grid_scale2,1)[0]);
    top_side4 = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    top_side5 = round(rands(building_parameters[2]/grid_scale1,building_parameters[2]/grid_scale2,1)[0]);
    top_side6 = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);

    grid_top_parameters=[top_grid_width, top_grid_length, top_side3, top_side4, top_side5, top_side6, 0, 0];//[width, length, side3, side4, side5, side6, height, style]

    center_grid_width = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);
    center_grid_length = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    center_side3 = round(rands((building_parameters[1]+building_parameters[3])/grid_scale1,(building_parameters[1]+building_parameters[3])/grid_scale2,1)[0]);
    center_side4 = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    center_side5 = round(rands(building_parameters[2]/grid_scale1,building_parameters[2]/grid_scale2,1)[0]);
    center_side6 = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);
    grid_center_parameters = [center_grid_width, center_grid_length, center_side3, center_side4, center_side5, center_side6, 0, 0];//[width, length, side3, side4, side5, side6, height, style]

    bottom_grid_width = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);
    bottom_grid_length = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    bottom_side3 = round(rands((building_parameters[1]+building_parameters[3])/grid_scale1,(building_parameters[1]+building_parameters[3])/grid_scale2,1)[0]);
    bottom_side4 = round(rands(building_parameters[0]/grid_scale1,building_parameters[0]/grid_scale2,1)[0]);
    bottom_side5 = round(rands(building_parameters[2]/grid_scale1,building_parameters[2]/grid_scale2,1)[0]);
    bottom_side6 = round(rands(building_parameters[1]/grid_scale1,building_parameters[1]/grid_scale2,1)[0]);
    grid_height = round(rands(center_grid_width/2,center_grid_width*2,1)[0]);
    bottom_height = round(rands(building_parameters[4]/grid_height,building_parameters[4]/10,1)[0]);
    grid_bottom_parameters=[bottom_grid_width, bottom_grid_length, bottom_side3, bottom_side4, bottom_side5, bottom_side6, bottom_height, 1];//[width, length, side3, side4, side5, side6, height, style]

    bottom_offset =-round(bottom_height/2);
    
    grid_parameters = [grid_bottom_parameters, grid_center_parameters, grid_top_parameters, grid_height, bottom_offset];//[bottom, center, top, height, vertical_offset]
    
    window_scale_parameters =[round(rands(window_width_scale_range[0],window_width_scale_range[1],1)[0]),rands(window_height_scale_range[0],window_height_scale_range[1],1)[0],round(rands(window_width_scale_range[0],window_width_scale_range[1],1)[0]),rands(window_height_scale_range[0],window_height_scale_range[1],1)[0],round(rands(window_width_scale_range[0],window_width_scale_range[1],1)[0]),rands(window_height_scale_range[0],window_height_scale_range[1],1)[0]];//[bottom_width, bottom_height, center_width, center_height, top_width, top_height]
    
    window_design_parameters = [round(rands(window_design_range[0],window_design_range[1],1)[0]), round(rands(window_design_range[0],window_design_range[1],1)[0]), round(rands(window_design_range[0],window_design_range[1],1)[0])];//[bottom, center, top]
 
    door_parameters=[(round(rands(0,grid_parameters[0][0]-1,1)[0])   ), 2, 0.9];//[position, width_scale, height_scale]
 
    door_height = ((building_parameters[4]-grid_parameters[0][6]*2)/grid_parameters[3])/door_parameters[2];
    top_scale = rands(top_scale_range[0], top_scale_range[1], 1)[0];
    
    
    //This calls main_inputs module if the stacking is on (See main_inputs for more info). 
    top_building_parameters = [building_parameters[0]/top_scale, building_parameters[1]/top_scale,building_parameters[2]/top_scale,building_parameters[3]/top_scale,building_parameters[4]/top_scale];
    translate(coord)rotate(angle)translate([0,0,building_parameters[4]/2])shape_grammar(sides, shape_type,building_parameters , overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);           
    if(top>0)translate([0,0,building_parameters[4]-top_building_parameters[4]/2])main_inputs(top-1, top_scale,1, [0,0,building_parameters[4]/2], 0, sides, shape_type, top_building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);

}

//This module calls the shape_grammar module, but it is also recursive for use with the stacking feature. 
module main_inputs(top, top_scale, scale_amount, coord, angle, sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style){    
    scale(scale_amount)translate(coord)rotate(angle)union(){ 
   
        shape_grammar(sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);  
        top_building_parameters = [building_parameters[0]/top_scale, building_parameters[1]/top_scale,building_parameters[2]/top_scale,building_parameters[3]/top_scale,building_parameters[4]/top_scale];
        if(top>0)translate([0,0,building_parameters[4]-top_building_parameters[4]/2])main_inputs(top-1, top_scale, 1, [0,0,0], 0, sides, shape_type, top_building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters,window_design_parameters, door_parameters, color_style);
   
    }
}

//The if statement below checks if the user selected the random option, and calls the modules based on that choice. 
if(RANDOM_ON){
    main_randomizer(top, top_scale_range, color_range, shape_range, building_range, window_width_scale_range, window_height_scale_range, window_design_range, railing_parameters,  door_scale_amount, door_location_range, railing_parameters, coord, angle, grid_scale1, grid_scale2);
}
else{
    
    door_height = ((building_parameters[4]-grid_parameters[0][6]*2)/grid_parameters[3])/door_parameters[2];
    top_scale = rands(top_scale_range[0], top_scale_range[1], 1)[0];
  
    top_building_parameters = [building_parameters[0]/top_scale, building_parameters[1]/top_scale,building_parameters[2]/top_scale,building_parameters[3]/top_scale,building_parameters[4]/top_scale];
    translate(coord)rotate(angle)translate([0,0,building_parameters[4]/2])shape_grammar(sides, shape_type,building_parameters , overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);           
    if(top>0)translate([0,0,building_parameters[4]-top_building_parameters[4]/2])main_inputs(top-1, top_scale,1, [0,0,building_parameters[4]/2], 0, sides, shape_type, top_building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);

    
    //translate([0,0,building_parameters[4]/2])main_inputs(stacking+1, top_scale, 1, 0,0, sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style);
}

//------------------------------BUIDLING GRAMMAR--------------------------

//The building grammar handles the generation of the actual building shape by first calling shape_grammar, then from
//there calling the various other modules (Rectangle, L, polygon, etc...) to create the unique shape as well as
//applying the overhang onto the building. Then it calls the facade grammar. 


//The shape_grammar module takes the entiriety of the parameters then checks what shape type was selected. 
//From there, it calls the specific module for generating each shape. 
module shape_grammar(sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style){
  
    //The building scales are used with the cube() function and the scale() function to scale the building
    //to its proper dimensions. 
    building_scale_h = building_parameters[4]/building_parameters[0];
    building_scale_l = building_parameters[1]/building_parameters[0];
    
    if(shape_type ==1)   
        gen_rectangle(shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l);   
    else if(shape_type ==2)
        gen_polygon(sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l);    
    else if(shape_type ==3)
        gen_L(shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l);    
}

//This module  generates the rectanglur cube shape, then calls the facade grammar. 
module gen_rectangle(shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l){
   
    union(){ 
        color(color_parameters[color_style][2]){
            //This line creates the building shape. 
            scale([1,building_scale_l,building_scale_h])cube(building_parameters[0], center=true);
            //The below code creates the overhang, excluding the actual roof.
            difference(){
                translate([0,0,building_parameters[4]/2])scale([1,1,overhang_parameters[2]])gen_square([building_parameters[0]+overhang_parameters[0]*2, building_parameters[1]+overhang_parameters[0]*2], center = true);
                translate([0,0,building_parameters[4]/2])scale([1,1,overhang_parameters[2]*2])gen_square([building_parameters[0]+overhang_parameters[0]*2-overhang_parameters[1]*2, building_parameters[1]+overhang_parameters[0]*2-overhang_parameters[1]*2], center = true);
            }
        }
        //This part creates the roof 
        color(color_parameters[color_style][5])translate([0,0,building_parameters[4]/2])gen_square([building_parameters[0]+overhang_parameters[0]*1.999, building_parameters[1]+overhang_parameters[0]*1.999], center = true);
    }
    
    facade_grammar(0, shape_type, building_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, 0, 0);
}

//This module generates a n sided polygon. It does so by calculating the circumradius and using that value to 
//generate a n amount of cylinders to be used with the hull function to fill the volume, generating a polygon. 
//The overhang works similarly. 

module gen_polygon(sides, shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l){
    
    //All of these variable declarations are used for the polygon generation. 
    circumradius = building_parameters[0]/(2*sin(180/sides)); //Distance to vertex of side. 
    sum_angle = (sides-2)*180; //Total angle of entire shape.
    side_angle = sum_angle/sides; //Angle of each side. 
    inradius = building_parameters[0]/(2*tan(180/sides)); //Distance to midpoint of side
    facade_angle = 180-(side_angle/2+90);
    
    //The below segment is used to generate a hull polygon. It first generates a thin cylinder, then it
    //translates it by the circumradius. Finally, it rotates it by the side angle. 
    color(color_parameters[color_style][2])hull(){
        for(side_num = [0:1:sides-1]){
            rotate([0,0,(180-side_angle)*side_num])translate([circumradius,0,0])cylinder(r=0.001, building_parameters[4], center = true);        
        }
    }
    //The below generates the roof
    color(color_parameters[color_style][5])hull(){
        for(side_num = [0:1:sides-1]){
            rotate([0,0,(180-side_angle)*side_num])translate([circumradius+overhang_parameters[0]*0.9,0,building_parameters[4]/2])cylinder(r=0.001, overhang_parameters[2]*0.1, center = true);        
        }
    }
    //The below generates the overhang
    color(color_parameters[color_style][2])difference(){
        hull(){
            for(side_num = [0:1:sides-1]){
                rotate([0,0,(180-side_angle)*side_num])translate([circumradius+overhang_parameters[0],0,building_parameters[4]/2+overhang_parameters[2]/2])cylinder(r=0.001, overhang_parameters[2], center = true);        
            }
        }
        
        hull(){
            for(side_num = [0:1:sides-1]){
                rotate([0,0,(180-side_angle)*side_num])translate([circumradius+overhang_parameters[0]-overhang_parameters[1],0,building_parameters[4]/2+overhang_parameters[2]/2*1.01])cylinder(r=0.001, overhang_parameters[2], center = true);        
            }
        }
    }
   
   facade_grammar(sides, shape_type, building_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, inradius, facade_angle); 
}



//This module generates an L shaped building. It does so by first generating two cubes of width, height 1, 2. 
//Then it translates them so that the two widths become one large one. The overhang is created using three 
//convex hulls, where each hull is comprised of four cylinders. 
module gen_L(shape_type, building_parameters, overhang_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, building_scale_h, building_scale_l){
     
    //This segment handles the actual L shape generation. Here, it splits the process into two parts,
    //one for each "cube" it generates. 
    translate([building_parameters[1]/2,0,0]){
        
        //The first part.
        color(color_parameters[color_style][2])scale([(building_parameters[1]+building_parameters[3])/building_parameters[0],1,building_parameters[4]/building_parameters[0]])cube(building_parameters[0], center = true);
        //This line creates a segment of the roof. 
        color(color_parameters[color_style][5])translate([0,0,building_parameters[4]/2])gen_square([(building_parameters[1]+building_parameters[3])+overhang_parameters[0]*1.8, building_parameters[0]+overhang_parameters[0]*1.8], center = true);  
         
        //The 2nd part. 
        translate([-building_parameters[1]/2,(building_parameters[0]+building_parameters[2])/2,0]){  
            color(color_parameters[color_style][2])scale([building_parameters[3]/building_parameters[2],1,building_parameters[4]/building_parameters[2]])cube(building_parameters[2], center = true);   
            //This part creates a segment of the roof. 
            color(color_parameters[color_style][5])translate([0,0,building_parameters[4]/2])gen_square([building_parameters[3]+overhang_parameters[0]*1.8, building_parameters[2]+overhang_parameters[0]*1.8], center = true);
        }
        
        
    }
    
    //The convex hull generation for the overhang. 
    //Each of the three parts comprises two hulls being differenced. 
    color(color_parameters[color_style][2]){
        difference(){
            hull(){
                translate([building_parameters[3]/2+building_parameters[1]+overhang_parameters[0],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/2+building_parameters[1]+overhang_parameters[0],-building_parameters[0]/2-overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/2+overhang_parameters[0]-overhang_parameters[1],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
                translate([building_parameters[3]/2,-building_parameters[0]/2-overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
            }
            hull(){
                translate([building_parameters[3]/2+building_parameters[1]+overhang_parameters[0]-overhang_parameters[1],building_parameters[0]/2+overhang_parameters[0]-overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/2+building_parameters[1]+overhang_parameters[0]-overhang_parameters[1],-building_parameters[0]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/3,building_parameters[0]/2+overhang_parameters[0]-overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
                translate([building_parameters[3]/3,-building_parameters[0]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
            }
        }
        difference(){
            hull(){
                translate([-building_parameters[3]/2-overhang_parameters[0],-building_parameters[0]/2-overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([-building_parameters[3]/2-overhang_parameters[0],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/2,building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
                translate([building_parameters[3]/2,-building_parameters[0]/2-overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
            }  
           hull(){
                translate([-building_parameters[3]/2-overhang_parameters[0]+overhang_parameters[1],-building_parameters[0]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([-building_parameters[3]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3],building_parameters[0]+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
                translate([building_parameters[3],-building_parameters[0]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
            }
        }
        difference(){
            hull(){
                translate([building_parameters[3]/2+overhang_parameters[0],building_parameters[0]/2+building_parameters[2]+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([-building_parameters[3]/2-overhang_parameters[0],building_parameters[0]/2+building_parameters[2]+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);
                translate([building_parameters[3]/2+overhang_parameters[0],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
                translate([-building_parameters[3]/2-overhang_parameters[0],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]);   
            }    
            hull(){
                translate([building_parameters[3]/2+overhang_parameters[0]-overhang_parameters[1],building_parameters[0]/2+building_parameters[2]+overhang_parameters[0]-overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);
                translate([-building_parameters[3]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[0]/2+building_parameters[2]+overhang_parameters[0]-overhang_parameters[1],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);
                translate([building_parameters[3]/2+overhang_parameters[0]-overhang_parameters[1],building_parameters[0]/2+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
                translate([-building_parameters[3]/2-overhang_parameters[0]+overhang_parameters[1],building_parameters[0]/4+overhang_parameters[0],building_parameters[4]/2])cylinder(r=0.01, overhang_parameters[2]*2);   
            }
        }
    } 
    facade_grammar(0, shape_type, building_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, 0, 0);
}


//------------------------------FACADE GRAMMAR--------------------------

//This module is essentially a gateway into the actual facade generation. Here, it just checks the shape type and
//sends the coordinates based on the shape to the gen_facade module. 
module facade_grammar(sides, shape_type, building_parameters, railing_parameters, grid_parameters, window_scale_parameters, window_design_parameters, door_parameters, color_style, inradius, facade_angle){

    //The rectangle facade is split into two parts, one for each side 
    //(As the widths for the two side types are different)
    if(shape_type ==1){
       
        for(side=[0:1:1]){
            gen_facade(4, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[0]/2,-building_parameters[1]/2,-building_parameters[4]/2], [0,0,180*side], grid_parameters, building_parameters[1], building_parameters[4], [1,0,0], side, 0, color_style);
        }      
        for(side=[0:1:1]){
            
            gen_facade(4, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[1]/2,-building_parameters[0]/2,-building_parameters[4]/2], [0,0,90+180*side], grid_parameters, building_parameters[0], building_parameters[4], [1,0,0], 1, 1, color_style);
        }  
    }
 
    //Due to the polygon having the same side length, it is just repeated in a loop for n times
    else if (shape_type==2){
        //Note: It is sides*2, and being incremented by 2 instead of one due to the facade placement onto the shape.
        for(side_num = [1:2:(sides)*2]){ 
            gen_facade(sides, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [inradius,-building_parameters[0]/2,-building_parameters[4]/2], [0,0,(facade_angle)*side_num], grid_parameters, building_parameters[0], building_parameters[4],
            [0,0,0], (side_num-1), 0, color_style);
        }
    }
    
    //Due to the L shape having six different side lengths, gen_facade is called six times. 
    else if (shape_type==3){
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[3]/2,-building_parameters[0]/2,-building_parameters[4]/2], [0,0,0], grid_parameters, building_parameters[0]+building_parameters[2], building_parameters[4], [1,0,0], 0, 0, color_style);
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-(building_parameters[0]/2+building_parameters[2]),-building_parameters[3]/2,-building_parameters[4]/2], [0,0,270], grid_parameters, building_parameters[3], building_parameters[4], [1,0,0], 1, 1, color_style);
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[0]/2,-(building_parameters[3]/2+building_parameters[1]),-building_parameters[4]/2], [0,0,90], grid_parameters, building_parameters[1]+building_parameters[3], building_parameters[4], [1,0,0], 1, 2, color_style);
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-(building_parameters[3]/2+building_parameters[1]),-building_parameters[0]/2,-building_parameters[4]/2], [0,0,180], grid_parameters, building_parameters[0], building_parameters[4], [1,0,0], 1, 3, color_style);
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[3]/2,-(building_parameters[0]/2+building_parameters[2]),-building_parameters[4]/2], [0,0,180], grid_parameters, building_parameters[2], building_parameters[4], [1,0,0], 1, 4, color_style);
        gen_facade(6, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, [-building_parameters[0]/2,building_parameters[3]/2,-building_parameters[4]/2], [0,0,270], grid_parameters, building_parameters[1], building_parameters[4], [1,0,0], 1, 5, color_style);       
    }
}


//This module splits the facade generation into its y components, meaning it generates the bottom row first (y=0),
//then it moves to the center rows, then finally the top row. This allows the bottom, center and top to each have different
//styles of windows available. 
module gen_facade(max_sides, door_parameters, window_scale_parameters, window_design_parameters, railing_parameters, coord, angle, grid_parameters, width, height, flip, door, side, color_style){
    
    //This is actually where it rotates and translate the facade onto the shape. 
    rotate(angle)translate(coord)mirror(flip){
       
        gen_level(max_sides, side, window_scale_parameters, door_parameters, grid_parameters, window_design_parameters, railing_parameters, coord, angle, grid_parameters[0][side], grid_parameters[3], width, height, flip, door, 0, window_scale_parameters[0], window_scale_parameters[1],
 grid_parameters[4], color_style, grid_parameters[0][6], grid_parameters[0][7]);
        for(y=[1:1:grid_parameters[3]-2]){//Center rows
            gen_level(max_sides, side, window_scale_parameters, door_parameters, grid_parameters, window_design_parameters, railing_parameters, coord, angle, grid_parameters[1][side], grid_parameters[3], width, height, flip, door, y, window_scale_parameters[2], window_scale_parameters[3], grid_parameters[4], color_style, grid_parameters[0][6], grid_parameters[0][7]);
        }
        gen_level(max_sides, side, window_scale_parameters, door_parameters, grid_parameters, window_design_parameters, railing_parameters, coord, angle, grid_parameters[2][side], grid_parameters[3], width, height, flip, door, grid_parameters[3]-1, window_scale_parameters[4], window_scale_parameters[5], grid_parameters[4], color_style, grid_parameters[0][6], grid_parameters[0][7]);
        
    } 
    
    
}


//This module generates the facade row and tiles. It does so by looping through the row, and generating a 
//tile where a window is placed. 
module gen_level(max_sides, side, window_scale_parameters, door_parameters, grid_parameters, window_design_parameters, railing_parameters, coord, angle, grid_width, grid_height, width, height, flip, door, y, width_scale, height_scale, vertical_offset, color_style, bottom_height, bottom_style){
    
    //The tile variables is the height/width of each tile or segment of the facade grid. 
    tile_height = (height-bottom_height*2)/grid_height;
    tile_width = width/grid_width;
    
    //The window variables are the size of the window based upon the tile parameters. 
    window_height = tile_height/height_scale;
    window_width = tile_width/width_scale;
    
    //The bottom window height is the height of the bottom window
    bottom_window_height = bottom_height*2/height_scale;
    
     //The door variables are the size of the door based upon the tile parameters. 
    door_height = tile_height/door_parameters[2];
    door_width = tile_width/door_parameters[1];   
     
    //This segment handles the railing generation. It does so by first looping through the 
    //railing parameter list and checking if the values stored match the y row of the facade
    //(Or if the values are -1), then calls the railing module. 
    translate([0,(width/2),(tile_height/2+bottom_height*2+y*tile_height)])
        for(railing_y=[0:1:grid_height]){     
            for(railing_side=[0:1:max_sides]){
                if( (railing_parameters[2][railing_side]==side || railing_parameters[2][railing_side]==-1) && ( (y==railing_parameters[1][railing_y])|| railing_parameters[1][railing_y]==-1) ){
                    translate([0,0,vertical_offset])railing(tile_width, tile_height,railing_parameters[0], color_parameters[color_style][2], width);        
                }       
            }    
        }
            
    //This segment handles the actual tile generation. 
    for(x=[0:1:grid_width-1]){ 
        union(){
            translate([0,(tile_width/2+x*tile_width),(tile_height/2+bottom_height*2+y*tile_height)]){
                 
                //This segment checks if there is a door at the x, y value defined in the door_parameters list. 
                //If there is, it creates a window for the door and calls the door module. 
                if(door == 0 && y==0 && ( x==door_parameters[0]) ){
                    rotate([90,0,90])translate([0,-(tile_height/2-door_height/2)-bottom_height*2,0])color(color_parameters[color_style][4])gen_square([door_width, door_height], center=true);   
                    door_grammar(door_width, door_height, tile_width, tile_height, color_style, bottom_height*2);  
                }
                
                //This segment is identical to the actual tile creation (Further below) but is modifed
                //to generate the bottom row height. 
                else if(y==0 && bottom_height>0){                          
                    translate([0,0,-bottom_height*2+bottom_window_height/2])rotate([90,0,90])color(color_parameters[color_style][0])gen_square([window_width, bottom_window_height], center=true);  
                    translate([0,0,-bottom_height*2+bottom_window_height/2])window_grammar(window_scale_parameters, window_design_parameters, window_width, bottom_window_height, tile_width, bottom_height, y, grid_parameters[3], color_style);                                              
                }
                //This segment generates the tile and window. 
                else{
                    translate([0,0,vertical_offset])rotate([90,0,90])color(color_parameters[color_style][0])gen_square([window_width, window_height], center=true);  
                    translate([0,0,vertical_offset])window_grammar(window_scale_parameters, window_design_parameters, window_width, window_height, tile_width, tile_height, y, grid_parameters[3], color_style);
                }      
            }
        }
    } 
}
//------------------------------WINDOW GRAMMAR--------------------------


module railing(tile_width, tile_height, railing_scale, railing_color, width){
   
    rotate([0,0,180])color(railing_color){
        translate([1,0,tile_height/2-tile_height/5])scale([railing_scale,1,1])rotate([90,90,90])gen_square([tile_height/5, width], center = true);      
        translate([1,0,tile_height/2-tile_height/3])scale([railing_scale/2,1,1])rotate([90,90,90])gen_square([tile_height/5, width], center = true);                
    }  
}


//This module generates the window style based upon the y row, then calls the gen_windows module. 
module window_grammar(window_scale_parameters, window_design_parameters, window_width, window_height, tile_width, tile_height, grid_y, grid_height, color_style){  
    
    //This is a style rule. If the window scale is 1 (meaning it takes up the entire tile), the program 
    //sets the window style to 4.
    if( (window_scale_parameters[0] == 1 && grid_y == 0) || (window_scale_parameters[2] == 1 && grid_y >0 && grid_y<grid_height-1) || (window_scale_parameters[0] == 1 && grid_y == grid_height-1)){
        mirror([1,0,0])gen_windows(window_width, 4, tile_width, window_height, tile_height, color_style);
    }
    //For the actual window styles. 
    else{
        if(grid_y==0){
        mirror([1,0,0])gen_windows(window_width, window_design_parameters[0], tile_width, window_height, tile_height, color_style);
        }
        else if(grid_y==grid_height-1){
            mirror([1,0,0])gen_windows(window_width, window_design_parameters[2], tile_width, window_height, tile_height, color_style);
        }
        
        else{
            mirror([1,0,0])gen_windows(window_width, window_design_parameters[1], tile_width, window_height, tile_height, color_style);
        } 
    }
}



//this module calls the specific window generation module based on the window type chosen. 
module gen_windows(window_width, window_op, tile_width, window_height, tile_h, color_style){
    
    if(window_op==1)gen_window_1(window_width, window_height, color_style);     
    if(window_op==2)gen_window_2(window_width, window_height, color_style);     
    if(window_op==3)gen_window_3(window_width, window_height, color_style);
    if(window_op==4)gen_window_4(window_width, window_height, color_style); 
    if(window_op==5)gen_window_5(window_width, window_height, color_style);
        
}
    



//The following five window modules generate a different style of window. 

module gen_window_1(window_width, window_height, color_style){
    color(color_parameters[color_style][1]){
        translate([-1,0,-window_height/2])scale([20,1,5])gen_square([1,window_width*1.5], center = true);                
        translate([-1,0,window_height/2])scale([10,1,5])gen_square([1,window_width*1.5], center = true);
        
        for(pill = [0:1:1]){
            $fn = 20;
            translate([-1,window_width/2-window_width*pill,-window_height/2])scale([5,5,1])cylinder(window_height);                       
        }                    
        translate([-1,0,0])scale([5,1,3])gen_square([1,window_width], center = true);   
    }  
}

module gen_window_2(window_width, window_height, color_style){
    color(color_parameters[color_style][1]){
        translate([-1,0,-window_height/2])scale([2,1,0.5])gen_square([1,window_width*1.5], center = true);           
        translate([-1,0,window_height/2])scale([1,1,0.5])gen_square([1,window_width*1.5], center = true);
        
        for(pill = [0:1:1]){           
            mirror([0,1*pill,0])translate([-1,window_width/2*1.2,0])scale([0.1,0.15,1])cube(window_height, center = true);                       
        }               
        translate([-1,0,0])scale([0.1,0.1,1])cube(window_height, center = true);
    }
    
    color(color_parameters[color_style][3]){
        for(strut = [0:1:1]){
            mirror([0,0,1*strut])translate([-1,0,window_height/2*0.3])scale([0.5,1,2])gen_square([1,window_width], center = true);
        }  
    }
}  


module gen_window_3(window_width, window_height, color_style){
    color(color_parameters[color_style][1]){
        translate([-1,0,-window_height/2*1.15])scale([30,1,10])gen_square([1,window_width*1.2], center = true);
        translate([-1,0,-window_height/2])scale([50,1,10])gen_square([1,window_width*1.5], center = true);           
        translate([-1,0,window_height/2])scale([1,1,0.5])gen_square([1,window_width*1.3], center = true);
        translate([-1,0,window_height/2*1.1])rotate([0,-45,0])scale([0.5,1.6,10])gen_square(window_width, center = true);
        for(pill = [0:1:1]){           
            mirror([0,1*pill,0])translate([-1,window_width/2*1.2,0])scale([0.3,0.15,1])cube(window_height, center = true);                        
        }
       
    }
    
    color(color_parameters[color_style][3]){
        translate([-1,0,-window_height/2*0.5])scale([0.01,0.05,0.7])cube(window_height, center = true); 
        translate([-1,0,window_height/2*0.2])scale([0.5,1,1])gen_square([1,window_width], center = true);
    }   
}

module gen_window_4(window_width, window_height, color_style){
    color(color_parameters[color_style][1])for(pill = [0:1:1]){
        $fn = 20;
        mirror([0,pill,0])translate([-0.5,window_width/2,-window_height/2])scale([2.5,2.5,1])cylinder(window_height);    
        mirror([0,0,pill])translate([0,0,window_height/2])scale([1,1,4])gen_square([15,window_width], center = true);
    }   
}

module gen_window_5(window_width, window_height, color_style){
    
    color(color_parameters[color_style][1])union(){
        
        translate([-0.5,0,window_height/2])scale([10,1,3])gen_square([1,window_width], center = true);
        translate([-1,0,-window_height/2])scale([25,1,5])gen_square([1,window_width*1.2], center = true);
        
        translate([-1,0,window_height/2*1.7])scale([20,1,7])gen_square([1,window_width*1.2], center = true);
        
        for(pill = [0:1:1]){
        $fn = 20;
        translate([-0.5,window_width/2-window_width*pill,-window_height/2])scale([2.5,2.5,1.35])cylinder(window_height);
        
        mirror([0,pill,0])translate([-0.5,window_width*0.3,-window_height/2])scale([1,1,1])cylinder(window_height);    
            
            
        }
        translate([-0.3,0,window_height/4])scale([4,1,1])gen_square([1,window_width], center = true);
        translate([-0.3,0,-window_height/4])scale([4,1,1])gen_square([1,window_width], center = true);
    }    
}




//This module generates a simple door. 
module door_grammar(door_width, door_height, tile_width, tile_height, color_style, bottom_height){
    translate([0,0,-bottom_height])color(color_parameters[color_style][1])union(){   
        translate([1,0,door_height/2-(tile_height/2-door_height/2)*0.7])gen_square([1,door_width*1.2], center = true);
        translate([1,0,door_height/2-(tile_height/2-door_height/2)])scale([20,1,10])gen_square([1,door_width*1.5], center = true);
        
        for(pill = [0:1:1]){
            $fn = 20;
            translate([-1,(door_width/2-door_width*pill),-(tile_height/2-door_height/2)])scale([1,1,1])cylinder(door_height, center=true); 
        }    
    }
}
//This square module is a means to render the building by using all 3D shapes 
//(As rendering doesn't support both 3D and 2D shapes)
module gen_square(size, center){
    
    width = size[0];
    length = size[1];
    if(center)scale([1,length/width,1/width])cube(width, center = true);
    else scale([1,length/width,1/width])cube(width);
    
}




