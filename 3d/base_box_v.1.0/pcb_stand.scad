$fn = 200;//количество полигонов

height = 12;
ext_diameter = 6;
int_diameter = 3.5;


difference()
{
    cylinder(h = height, d = ext_diameter, center = true);
    cylinder(h = height+1, d = int_diameter, center = true);
}