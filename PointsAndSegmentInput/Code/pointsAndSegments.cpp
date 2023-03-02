/*------------------------------------------------------------------------------+
|    This simple demo lets the user select one of two modes through a pop-up        |
|    menu (right click):    - point creation                                        |
|                        - segment creation                                        |
|                                                                                |
|    If the program ccan be launched with the path to a data file containing        |
|    lists of points and segments in the app's custom format presented in the    |
|    attached Data File Format.md                                                |
|                                                                                |
|    Points and segment endpoints are recorded at the location of mouse clicks.    |
|    In segment creation mode, two consecutive clicks define the segment's two    |
|    endpoints.                                                                  |
|                                                                                |
|    Initially I wanted to use pressing the SHIFT key to constrain the segment     |
|    to a horizontal, vertical, or diagonal direction (whichever is closest to     |
|    the one actually entered by the user). However, glut only checks the SHIFT    |
|    key in the mouse and keyboard callback functions, NOT the display or mouse     |
|    motion callback functions.                                                     |
|    So, instead, I added "snap to" modes that can be toggled on/off via a menu.    |
|    I am starting with:                                                            |
|        o horizontal,                                                            |
|        o vertical,                                                                |
|        o diagonal.                                                                |
|    If the segment being created (first endpoint clicked in, second not yet),    |
|    is within 5º (0.0872665 rad.) of a snap direction, the closest point on        |
|    that line will be chosen as endpoint.                                        |
|    Later on, I would like to add possibility to snap to:                        |
|        o an existing point,                                                     |
|        o an existing segment.                                                    |
|    In both cases if the click is within 1.5 * disk radius of the target.        |
|                                                                                |
|    Hitting the 'ESC' button resets all current mode and editing selections.    |
|    For example, in segment creation mode, if the user creates a first endpoint    |
|    then hits the 'ESC' key, that first segment point is discarded (but the        |
|    mode remains segment creation).                                                |
|                                                                                |
|    The Application  can only be quit by selecting 'EXIT' in the popup menu        |
|    or hitting SHIFT-ESC at the keyboard.                                        |                                                            |                                            |
|                                                                                |
|    jyh, 2023-02-04.                                                            |
+------------------------------------------------------------------------------*/

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <cstdlib>
#include <cmath>
//
#include "glPlatform.h"
#include "World.hpp"
#include "Point.hpp"
#include "Segment.hpp"
#include "dataFileIO.hpp"

using namespace std;
using namespace geometry;


//-----------------------------------------------------------------
//  function prototypes
//-----------------------------------------------------------------
float checkScaling(void);
PointStruct pixelToWorld(int ix, int iy);
PointStruct worldToPixel(float x, float y);
void menuHandlerFunc(int value);
void modeMenuHandlerFunc(int value);
void snapMenuHandlerFunc(int value);
void displayFunc(void);
void resizeFunc(int w, int h);
void mouseHandlerFunc(int b, int s, int x, int y);
void passiveMotionHandlerFunc(int x, int y);
void keyboardHandlerFunc(unsigned char, int x, int y);
void interfaceInit(void);
void applicationInit(int argc, char* argv[]);
void zeEnd(void);
void myTimerFunc(int val);

//-----------------------------------------------------------------
//  Custom data types
//-----------------------------------------------------------------
enum class ApplicationMode{
    NO_MODE = -1,
    //
    POINT_CREATION,
    SEGMENT_CREATION,
    //
    NUM_MODES
};

//  Dimensions of the world in "world units"
float World::X_MIN = -100;
float World::X_MAX = +100;
float World::Y_MIN = -100;
float World::Y_MAX = +100;
float World::WORLD_WIDTH = World::X_MAX - World::X_MIN;
float World::WORLD_HEIGHT = World::Y_MAX - World::Y_MIN;


//-----------------------------------------------------------------
//  GUI constants
//-----------------------------------------------------------------
const GLint    EXIT_MENU_ITEM = 0,
            CLEAR_MENU_ITEM = 1,
            CLEAR_ALL_MENU_ITEM = 2,
            //
            SEPARATOR = -1;

const GLint    POINT_CREATION_CODE = 10,
            SEGMENT_CREATION_CODE = 11;
//            POINT_EDIT_CODE = 20,
//            SEGMENT_EDIT_CODE = 21;

const int SNAP_START_CODE = 20;
enum SnapToCode{
    SNAP_TO_VERTICAL = 0,
    SNAP_TO_HORIZONTAL,
    SNAP_TO_DIAGONAL,
    SNAP_TO_POINT,
    SNAP_TO_SEGMENT,
    //
    NUM_SNAP_MODES
};

const string SNAP_MENU_STR[] = {
    "Snap to vertical: ",
    "Snap to horizontal: ",
    "Snap to diagonal: ",
    "Snap to point: ",
    "Snap to segment: "
};
//const int NUM_SNAP_MODES = (int)(sizeof(SNAP_MENU_STR)/sizeof(SNAP_MENU_STR[0]));

//-----------------------------------------------------------------
//  file-level global variables
//-----------------------------------------------------------------

//    Dimensions of the drawing pane (might be different from dimensions of window
//    if we add a GUI pane on the side)
//    I won't authorize window resizing.
const int PANE_WIDTH=800, PANE_HEIGHT=800;
const int WIN_WIDTH=PANE_WIDTH, WIN_HEIGHT=PANE_HEIGHT;

//    World to pixel and pixel to world scale factors
//    I will check first that we have the same scaling vertically
const float PIXEL_TO_WORLD = checkScaling();
const float WORLD_TO_PIXEL = 1.f / PIXEL_TO_WORLD;
//
//    Size of a point disk:  I want a five-pixel radius on screen
//    This will get scaled to world units by a call to Point::setPointDiskRadius
GLfloat POINT_DISK_RADIUS = 5.f;

//    Right now there is only a main window.  We may have subwindow indices.
int mainWindow;

bool isFirstClick = true;
PointStruct firstEndpoint;
int mouseX, mouseY;

ApplicationMode mode = ApplicationMode::NO_MODE;

const string rootFilePath = "../../../savedScene";
string dataFilePath;

bool snapToVertical = false;
bool snapToHorizontal = false;
bool snapToDiagonal = false;
bool snapToPoint = false;
bool snapToSegment = false;

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Scaling and conversion functions
//-----------------------------------------------------------------
#endif

// a function to setup the window as to track the
// mouse to draw polygon starting position at click position.
PointStruct pixelToWorld(int ix, int iy){
    //    we need to transform the pixel coordinates into "world" coordinates
    //    Please note that my world y direction points "up" but the pixel
    //    direction points down
    PointStruct pt = {World::X_MIN + ix*PIXEL_TO_WORLD,
        World::Y_MIN + (PANE_HEIGHT-iy)*PIXEL_TO_WORLD};

    return pt;
}

PointStruct worldToPixel(float x, float y){
    PointStruct pixelPt = {(x-World::X_MIN)*WORLD_TO_PIXEL,
        PANE_HEIGHT - (y-World::Y_MIN)*WORLD_TO_PIXEL};

    return pixelPt;
}


float checkScaling(void){
    //    I just refuse to consider the possibility that I was given a zero
    //    pane width or height
    float widthRatio = World::WORLD_WIDTH / PANE_WIDTH;
    float heightRatio = World::WORLD_HEIGHT / PANE_HEIGHT;

    // If the two ratios differ by more than 5%, I reject the dimensions)
    if (fabsf(widthRatio-heightRatio)/max(widthRatio,heightRatio) > 0.05){
        cout << "This application will not support different H & V scalings" << endl;
        exit(1);
    }
    
    Point::setPointDiskRadius(POINT_DISK_RADIUS * widthRatio);

    return widthRatio;
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Callback functions
//-----------------------------------------------------------------
#endif

//    This resize function ignores attempts by the user to resize the
//    window and sets it back to the values st in the code.
void resizeFunc(int w, int h){
    if (w != WIN_WIDTH || h != WIN_HEIGHT){
        glutReshapeWindow(WIN_WIDTH, WIN_HEIGHT);
    }

    glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glutPostRedisplay();
}

void displayFunc(void){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear Screen And Depth Buffer
    
    glPushMatrix();

    Segment::renderAllSegments();
    Point::renderAllSinglePoints();
        
    //    if we are creating a segment, then draw the putative segment, based
    //    on current mouse pointer location
    if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick){
        PointStruct nextPt = pixelToWorld(mouseX, mouseY);
        
        // If the shift key is down, we try to enforce closest among
        //    vertical/horizontal/diagonal
        if (glutGetModifiers() & GLUT_ACTIVE_SHIFT){
            //    compute deltaX and deltaY, and find out the closest
            //    direction to enforce
            float dx = nextPt.x - firstEndpoint.x;
            float dy = nextPt.y - firstEndpoint.y;
            
            //    as much as I dislike (and advocate against) using angles
            //    in geometrical applications, here it is by far the simplest
            //    solution
            float angle = atan2f(dy, dx);
            float rad = sqrtf(dx*dx + dy*dy);
            
            // snap to the closes multiple of π/4
            int mult = static_cast<int>(round(4.f*angle/M_PI));
            angle =  static_cast<float>(mult * M_PI / 4.0);
            nextPt.x = firstEndpoint.x + rad*cosf(angle);
            nextPt.y = firstEndpoint.y - rad*sinf(angle);
        }

        Segment::renderCreated(firstEndpoint, nextPt);
    }
    
    glPopMatrix();
    glutSwapBuffers();
}

void myTimerFunc(int val){
    glutTimerFunc(15, myTimerFunc, val);
    
    // redraw the scene
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}



void mouseHandlerFunc(int button, int state, int ix ,int iy){
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON){
        PointStruct pt = pixelToWorld(ix, iy);
        
        switch (mode){
            using enum ApplicationMode;
            
            case POINT_CREATION:
                Point::makeNewPointPtr(pt.x, pt.y);
                break;
                
            case SEGMENT_CREATION:
                if (isFirstClick){
                    firstEndpoint = pt;
                    mouseX = ix; mouseY = iy;
                    isFirstClick = false;
                }
                else{
                    Segment::makeNewSegPtr(Point::makeNewPoint(firstEndpoint.x, firstEndpoint.y),
                                           Point::makeNewPoint(pt.x, pt.y));
                    isFirstClick = true;
                }
                break;
                
            default:
                break;
        
        }
    }

    glutSetWindow(mainWindow);
    glutPostRedisplay();
}

void passiveMotionHandlerFunc(int x, int y){
    //    If we are creating a new segment and have already recorded
    //    the first endpoint, then we need to track the mouse position.
    if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick){
        mouseX = x;
        mouseY = y;
    }
}

/*    Menu event handling        */
void menuHandlerFunc(int value){
    switch (value){
        case EXIT_MENU_ITEM:
            zeEnd();
            break;
        
        case CLEAR_MENU_ITEM:
            switch (mode){
                using enum ApplicationMode;
                case POINT_CREATION:
//                    pointList.clear();
                    break;

                case SEGMENT_CREATION:
//                    segmentList.clear();
                    isFirstClick = true;
                    break;
            
                default:
                break;
            }
            break;

        case CLEAR_ALL_MENU_ITEM:
//            segmentList.clear();
//            pointList.clear();
            isFirstClick = true;
            break;
            
        default:
            break;
    }
}

void modeMenuHandlerFunc(int value){
    switch (value){
        case POINT_CREATION_CODE:
            mode = ApplicationMode::POINT_CREATION;
            isFirstClick = true;
            break;

        case SEGMENT_CREATION_CODE:
            mode = ApplicationMode::SEGMENT_CREATION;
            isFirstClick = true;
            break;
            
        default:
            break;
    }
}

//enum SnapToCode {
//    SNAP_TO_VERTICAL = 20,
//    SNAP_TO_HORIZONTAL,
//    SNAP_TO_DIAGONAL,
//    SNAP_TO_POINT,
//    SNAP_TO_SEGMENT
//};
//
//const string SNAP_MENU_STR[] = {
//    "Snap to vertical",
//    "Snap to horizontal",
//    "Snap to diagonal",
//    "Snap to point",
//    "Snap to segment"
//};


void snapMenuHandlerFunc(int value){
    switch (value){
        case SNAP_TO_VERTICAL:
        break;
    
        case SNAP_TO_HORIZONTAL:
        break;
    
        case SNAP_TO_DIAGONAL:
        break;
    
        case SNAP_TO_POINT:
        break;
    
        case SNAP_TO_SEGMENT:
        break;
    
        default:
        break;
    }

}

// Keyboard event handling
void keyboardHandlerFunc(unsigned char key, int x, int y){
    (void) x;
    (void) y;
    switch (key){
        //    Escape key
        case 27:
            //    Alt-ESC makes us exit the application
            if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
                zeEnd();
            
            //    At this point, this is about the only effect
            if (mode == ApplicationMode::SEGMENT_CREATION){
                //    If we had already recorded a first endpoint, forget it
                if (!isFirstClick){
                    isFirstClick = true;
                }
            }
            break;
        
        //    Ctrl-S saves the current scene as a data file
        case 's':
        case 'S':
            if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
                writeDataFile(rootFilePath);
            }
            break;

        //    Ctrl-R restores the scene to the state specified by the last saved file
        //    or the input data file if any.  Does nothing otherwise.
        //
        case 'r':
        case 'R':
            if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
                readDataFile(dataFilePath);
            }
            break;


        default:
            break;
    }
    
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Main, init, and exit
//-----------------------------------------------------------------
#endif

void interfaceInit(void){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutInitWindowPosition(100,40);
    mainWindow = glutCreateWindow("Point & Segment Creation Demo");

    glutSetWindow(mainWindow);
    //    at this point, all callback calls are for this window
    // make the background BLACK
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // setup callbacks
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(resizeFunc);
    glutMouseFunc(mouseHandlerFunc);
    glutPassiveMotionFunc(passiveMotionHandlerFunc);
    glutKeyboardFunc(keyboardHandlerFunc);
    glutTimerFunc(15, myTimerFunc, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(World::X_MIN, World::X_MAX, World::Y_MIN, World::Y_MAX);
    
    // Create Menus
    int menu, modeSubMenu, snapMenu;
    
    // Submenu for mode selection choices
    // Note how I use my arrays defined in ApplConstants.h to automatically generate my menus
    modeSubMenu = glutCreateMenu(modeMenuHandlerFunc);
    glutAddMenuEntry("Point Creation", POINT_CREATION_CODE);
    glutAddMenuEntry("Segment Creation", SEGMENT_CREATION_CODE);

    // Submenu for toggling on/off "snap-to" modes
    snapMenu = glutCreateMenu(snapMenuHandlerFunc);
    glutAddMenuEntry("Point Creation", POINT_CREATION_CODE);
    
    // Main menu that the submenus are connected to
    menu = glutCreateMenu(menuHandlerFunc);
    glutAddSubMenu("Mode", modeSubMenu);
    glutAddMenuEntry("Clear for mode", CLEAR_MENU_ITEM);
    glutAddMenuEntry("-", SEPARATOR);
    glutAddMenuEntry("Clear All", CLEAR_ALL_MENU_ITEM);
    glutAddMenuEntry("-", SEPARATOR);
    glutAddMenuEntry("Exit", EXIT_MENU_ITEM);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void applicationInit(int argc, char* argv[]){
    //    If the program has one argument, it should be the path to a data file
    if (argc == 2){
        //    save the path to be able to reload the scene
        dataFilePath = argv[1];
        readDataFile(dataFilePath);
    }else if (argc > 2){
        cout << "This program accets only one argument (path to a data file " <<
                "or none at all." << endl;
        exit(3);
    }
}

void zeEnd(void){
//    pointList.clear();
//    segmentList.clear();
    Point::clearAllPoints();
    Segment::clearAllSegments();
    exit(0);
}

int main(int argc, char** argv){
    // It's generally recommended to initialize glut early.
    glutInit(&argc, argv);

    //    This only checcks if by any chance we have a file path
    //    as argument, in which case we will load lists of predefined
    //    points and segments.
    applicationInit(argc, argv);
    
    //    Initialize the GUI (create window, set up menus and
    //    callback functions
    interfaceInit();
    
    //    And now we are ready to relinquish control to glut
    glutMainLoop();
    
    return 0;
}




