$fn = 200;//количество полигонов

height = 15;
ext_diameter = 6;
int_diameter = 4.5;


    difference()
    {
        cylinder(h = height, d = ext_diameter);
        translate([0,0,2])
            cylinder(h = height, d = int_diameter);
    }
    
        

