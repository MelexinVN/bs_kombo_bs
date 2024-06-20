/***************************************************************
base_box.scad
Программа генерации 3д модели корпуса базы
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
wallThickness       = 2;//1.8;
basePlaneThickness  = 2;//1.2;
lidPlaneThickness   = 2;//1.2;

//-- Total height of box = basePlaneThickness + lidPlaneThickness 
//--                     + baseWallHeight + lidWallHeight
//-- space between pcb and lidPlane :=
//--      (baseWallHeight+lidWallHeight) - (standoffHeight+pcbThickness)
baseWallHeight      = 15;
lidWallHeight       = 15;

//-- ridge where base and lid off box can overlap
//-- Make sure this isn't less than lidWallHeight
ridgeHeight         = 3.5;
ridgeSlack          = 0.2;
roundRadius         = 1.0;

//-- pcb dimensions
pcbLength           = 99;
pcbWidth            = 99;
pcbThickness        = 1.5;

//-- How much the PCB needs to be raised from the base
//-- to leave room for solderings and whatnot
standoffHeight      = 10.0;
pinDiameter         = 2.0;
pinHoleSlack        = 0.5;
standoffDiameter    = 3;
standoffSupportHeight   = 10.0;
standoffSupportDiameter = 2.0;
                            
//-- padding between pcb and inside wall
paddingFront        = 2;
paddingBack         = 2;
paddingRight        = 2;
paddingLeft         = 2;


//-- D E B U G ----------------------------
showSideBySide      = 1;       //-> true
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
                ,[pcbLength-2.5,2.5,yappBaseOnly,yappHole]
                ,[2.5,pcbWidth-2.5,yappBaseOnly,yappHole]
                ,[pcbLength-2.5,pcbWidth-2.5,yappBaseOnly,yappHole]
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
                //Отверстия под дисплей и кнопки управления
                  [0.5, 43.5, 40, 98, 0, yappRectangle]
                 ,[62.7, 11.8, 5.5, 10, 0, yappCircle,yappCenter]
                 ,[62.7, 20.3, 5.5, 10, 0, yappCircle,yappCenter]
                 ,[62.7, 29, 5.5, 10, 0, yappCircle,yappCenter]
                 ,[72.7, 29, 5.5, 10, 0, yappCircle,yappCenter]
                 ,[14.4, 5.3, 7, 10, 0, yappCircle,yappCenter]
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
                    //Отверстие под аудио выход
                     [27.2, 2, 10, 2, 0, yappCircle, yappCenter]
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
                //Отверстия под выключатель подсветки дисплея, USB выводы
                     [48, -1, 10, 5, 0, yappRectangle]
                     ,[17, 0, 10, 7, 0, yappRectangle]
                     ,[64, -1, 10, 5, 0, yappRectangle]
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
                //Отверстие под SD-карту звукового модуля
                  [73, 9, 16, 6, 0, yappRectangle]
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
                //Отверстие под контроллер заряда аккумулятора
                    [66, -2, 13, 6, 0, yappRectangle]
               ];

//-- connectors -- origen = box[0,0,0]
// (0) = posx
// (1) = posy
// (2) = screwDiameter
// (3) = insertDiameter
// (4) = outsideDiameter
// (5) = { yappAllCorners }
connectors   = [

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
                    [10,  5, yappLeft, yappRight, yappSymmetric]
                   ,[10,  5, yappFront, yappBack, yappSymmetric]
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

                ];

module lidHook()
{
  %translate([-20, -20, 0]) cube(5);
}

//--- this is where the magic hapens ---
YAPPgenerate();

