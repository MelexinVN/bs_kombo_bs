/***************************************************************
button_box.scad
Программа генерации 3д модели корпуса игровой кнопки
Электронной системы для проведения интеллектуальных игр
("Брейн системы") проекта КомБО (Открытые системы беспроводной коммуникации)

Версия 1.0

Используется библиотека YAPP_Box 
Yet Another Parameterised Projectbox generator
https://mrwheel-docs.gitbook.io/yappgenerator_en/

Автор: MelexinVN (Мелехин В.Н.)
***************************************************************/

//---------------------------------------------------------
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------
include <library/YAPPgenerator_v16.scad>

// Note: length/lengte refers to X axis, 
//       width/breedte to Y, 
//       height/hoogte to Z

/*
      padding-back|<------pcb length --->|<padding-front
                            RIGHT
        0    X-as ---> 
        +----------------------------------------+   ---
        |                                        |    ^
        |                                        |   padding-right 
        |                                        |    v
        |    -5,y +----------------------+       |   ---              
 B    Y |         | 0,y              x,y |       |     ^              F
 A    - |         |                      |       |     |              R
 C    a |         |                      |       |     | pcb width    O
 K    s |         |                      |       |     |              N
        |         | 0,0              x,0 |       |     v              T
      ^ |   -5,0  +----------------------+       |   ---
      | |                                        |    padding-left
      0 +----------------------------------------+   ---
        0    X-as --->
                          LEFT
*/

//-- which half do you want to print?
printLidShell       = true;
printBaseShell      = true;

//-- Edit these parameters for your own board dimensions
wallThickness       = 2;
basePlaneThickness  = 2;
lidPlaneThickness   = 2;

//-- Total height of box = basePlaneThickness + lidPlaneThickness 
//--                     + baseWallHeight + lidWallHeight
//-- space between pcb and lidPlane :=
//--      (baseWallHeight+lidWallHeight) - (standoffHeight+pcbThickness)
baseWallHeight      = 3;
lidWallHeight       = 75;

//-- ridge where base and lid off box can overlap
//-- Make sure this isn't less than lidWallHeight
ridgeHeight         = 2;
ridgeSlack          = 0.2;
roundRadius         = 3.0;

//-- pcb dimensions
pcbLength           = 30.5;
pcbWidth            = 30.5;
pcbThickness        = 1.5;

//-- How much the PCB needs to be raised from the base
//-- to leave room for solderings and whatnot
standoffHeight      = 7.0;
pinDiameter         = 2.0;
pinHoleSlack        = 0.5;
standoffDiameter    = 7;
standoffSupportHeight   = 3.0;
standoffSupportDiameter = 0.0;
                            
//-- padding between pcb and inside wall
paddingFront        = 10;
paddingBack         = 49.5;
paddingRight        = 49.5;
paddingLeft         = 10;


//-- D E B U G ----------------------------
showSideBySide      = true;       //-> true
onLidGap            = 0;
shiftLid            = 1;
hideLidWalls        = false;       //-> false
colorLid            = "yellow";   
hideBaseWalls       = false;       //-> false
colorBase           = "white";
showPCB             = false;      //-> false
showMarkers         = false;      //-> false
inspectX            = 0;  //-> 0=none (>0 from front, <0 from back)
inspectY            = 0;
//-- D E B U G ----------------------------


//-- pcb_standoffs  -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = { yappBoth | yappLidOnly | yappBaseOnly }
// (3) = { yappHole, YappPin }
pcbStands = [
                 [2.54,2.54,yappBaseOnly,yappHole]
                ,[pcbLength-2.54,2.54,yappBaseOnly,yappHole]
                ,[2.54,pcbWidth-2.54,yappBaseOnly,yappHole]
                ,[pcbLength-2.54,pcbWidth-2.54,yappBaseOnly,yappHole]
             ];

//-- Lid plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLid =  [ 
               //Отверстия для аркадной кнопки     
                   [(paddingBack + pcbLength + paddingFront)/2 - paddingBack, (paddingLeft + pcbWidth + paddingRight)/2 - paddingLeft, 25, 0, 0, yappCircle, yappCenter]
                   ,[(paddingBack + pcbLength + paddingFront)/2 - paddingBack, (paddingLeft + pcbWidth + paddingRight)/2 - paddingLeft -  19.25, 5, 0, 0, yappCircle, yappCenter]
                   ,[(paddingBack + pcbLength + paddingFront)/2 - paddingBack, (paddingLeft + pcbWidth + paddingRight)/2 - paddingLeft + 19.25, 5, 0, 0, yappCircle, yappCenter]
                  //, [pcbLength/2, pcbWidth/2, 10, 15, 45, yappRectangle, yappCenter]
                 // , [pcbLength/2, pcbWidth/2, 5, 0, 0, yappCircle]
              ];

//-- base plane    -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posy
// (2) = width
// (3) = length
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBase = [

              ];

//-- front plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsFront =  [
                 //Отверстия для контроллера заряда аккумулятора
                     [(pcbWidth+paddingRight)/2 - 1, -7.5, 20, 2, 0, yappRectangle]
                    ,[(pcbWidth+paddingRight)/2+5, -standoffHeight, 8, 5, 0, yappRectangle]
                   , [(pcbWidth+paddingRight)/2+3, 0, 3, 0, 0, yappCircle]
                ];

//-- back plane  -- origin is pcb[0,0,0]
// (0) = posy
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsBack = [

              ];

//-- left plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsLeft = [

              ];

//-- right plane   -- origin is pcb[0,0,0]
// (0) = posx
// (1) = posz
// (2) = width
// (3) = height
// (4) = angle
// (5) = { yappRectangle | yappCircle }
// (6) = { yappCenter }
cutoutsRight = [
                //Отверстие под выключатель
                    [(paddingBack + pcbLength + paddingFront)/2 - paddingBack - 7, -7, 14, 9, 0, yappRectangle]

               ];

//-- connectors -- origen = box[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectors   = [
                   [7, 7, 4, 3, 10, yappAllCorners]
               ];


//-- base mounts -- origen = box[x0,y0]
// (0) = posx | posy
// (1) = screwDiameter
// (2) = width
// (3) = height
// (4..7) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (5) = { yappCenter }
baseMounts   = [

               ];

               
//-- snapJons -- origen = shell[x0,y0]
// (0) = posx | posy
// (1) = width
// (2..5) = yappLeft / yappRight / yappFront / yappBack (one or more)
// (n) = { yappSymmetric }
snapJoins   =   [

                ];

 
//-- origin of labels is box [0,0,0]
// (0) = posx
// (1) = posy/z
// (2) = orientation
// (3) = depth
// (4) = plane {lid | base | left | right | front | back }
// (5) = font
// (6) = size
// (7) = "label text"
labelsPlane =  [
            [22, 10, 0, 1, "back", "Liberation Mono:style=bold", 5, "bs_kombo MHXC" ]
                ];

module lidHook()
{
  %translate([-20, -20, 0]) cube(5);
}

//--- this is where the magic hapens ---
YAPPgenerate();

//лого
translate([6.5,84,100])
    rotate([270,0,90]) 
        import("logo_bs_kombo.stl");