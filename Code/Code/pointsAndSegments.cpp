/*------------------------------------------------------------------------------+
|    This simple demo lets the user select one of two modes through a pop-up        |
|    menu (right click):    - point creation                                        |
|                        - segment creation                                        |
|                                                                                |
|    Points and segment endpoints are recorded at the location of mouse clicks.    |
|    In segment creation mode, two consecutive clicks define the segment's two    |
|    endpoints.  In that mode, pressing the SHIFT key allows to constrain the     |
|    segment to a horizontal, vertical, or diagonal direction (whichever is         |
|    closest to the one actually entered by the user.                            |
|                                                                                |
|    Hitting the 'ESC' button resets all current mode and editing selections.    |
|    For example, in segment creation mode, if the user creates a first endpoint    |
|    then hits the 'ESC' key, that first segment point is discarded (but the        |
|    mode remains segment creation).                                                |
|                                                                                |
|    The Application  can only be quit by selecting 'EXIT' in the popup menu        |
|    or hitting alt-c at the keyboard (sorry, but ctrl is not detected on my        |
|    Mac.                                                                        |                                            |
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
#include "Point.hpp"
#include "glPlatform.h"
#include "Common.h"

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
void displayFunc(void);
void resizeFunc(int w, int h);
void mouseHandlerFunc(int b, int s, int x, int y);
void passiveMotionHandlerFunc(int x, int y);
void keyboardHandlerFunc(unsigned char, int x, int y);
void myinit(void);
void zeEnd(void);
void myTimerFunc(int val);
void renderPoint(const Point& pt, PointType type);
void renderPoint(const PointStruct& pt, PointType type);
void renderSegment(const Point& pt1, const Point& pt2, SegmentType type);
void renderCreatedSegment(const PointStruct& pt1, const PointStruct& pt2);
const GLfloat*const* initCirclePt(size_t nbPts);

//-----------------------------------------------------------------
//  GUI constants
//-----------------------------------------------------------------
const GLint    EXIT_MENU_ITEM = 0,
            CLEAR_MENU_ITEM = 1,
            CLEAR_ALL_MENU_ITEM = 2,
            //
            SEPARATOR = -1;

const GLint    POINT_CREATION_CODE = 10,
            SEGMENT_CREATION_CODE = 11,
            POINT_EDIT_CODE = 20,
            SEGMENT_EDIT_CODE = 21;

const GLfloat POINT_COLOR[][4] = {
                                {0.f, 0.8f, 0.8f, 1.f},    //    SINGLE_POINT,
                                {1.f, 0.5f, 0.f, 1.f},    //    ENDPOINT,
                                {1.f, 1.f, 0.f, 1.f},    //    FIRST_ENDPOINT,
                                {1.f, 0.f, 1.f, 1.f}    //    EDIT_POINT
};

const GLfloat SEGMENT_COLOR[][4] = {
                                    {1.f, 1.f, 1.f, 1.f},    //    SEGMENT,
                                    {0.7f, 1.f, 0.7f, 1.f},    //    CREATED_SEGMENT,
                                    {0.f, 0.f, 1.f, 1.f}    //    EDITED_SEGMENT
};


//-----------------------------------------------------------------
//  file-level global variables
//-----------------------------------------------------------------

 //  Dimensions of the world in "world units"
const float X_MIN = -100, X_MAX = +100;
const float Y_MIN = -100, Y_MAX = +100;
const float WORLD_WIDTH = X_MAX - X_MIN;
const float WORLD_HEIGHT = Y_MAX - Y_MIN;

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
GLfloat POINT_DISK_RADIUS = 5.f * PIXEL_TO_WORLD;

//    Right now there is only a main window.  We may have subwindow indices.
int mainWindow;

// vectors to store isolated points and segments.  Since I haven't defined
// a Segment type. a segment is implcitly defined as two consecutive Point
// objects on the vector list:
//        Even index:  first endpoint, odd index:  second endpoint.
vector<shared_ptr<Point> > pointList;
vector<shared_ptr<Point> > segmentList;
bool isFirstClick = true;
PointStruct firstEndpoint;
int mouseX, mouseY;

const int NUM_CIRCLE_PTS = 12;
const GLfloat*const* CIRCLE_PT = initCirclePt(NUM_CIRCLE_PTS);

ApplicationMode mode = ApplicationMode::NO_MODE;


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Scaling and conversion functions
//-----------------------------------------------------------------
#endif

// a function to setup the window as to track the
// mouse to draw polygon starting position at click position.
PointStruct pixelToWorld(int ix, int iy)
{
    //    we need to transform the pixel coordinates into "world" coordinates
    //    Please note that my world y direction points "up" but the pixel
    //    direction points down
    PointStruct pt = {X_MIN + ix*PIXEL_TO_WORLD,
                      Y_MIN + (PANE_HEIGHT-iy)*PIXEL_TO_WORLD};

    return pt;
}

PointStruct worldToPixel(float x, float y)
{
    PointStruct pixelPt = {(x-X_MIN)*WORLD_TO_PIXEL,
                     PANE_HEIGHT - (y-Y_MIN)*WORLD_TO_PIXEL};

    return pixelPt;
}


float checkScaling(void)
{
    //    I just refuse to consider the possibility that I was given a zero
    //    pane width or height
    float widthRatio = WORLD_WIDTH / PANE_WIDTH;
    float heightRatio = WORLD_HEIGHT / PANE_HEIGHT;

    // If the two ratios differ by more than 5%, I reject the dimensions)
    if (fabsf(widthRatio-heightRatio)/max(widthRatio,heightRatio) > 0.05)
    {
        cout << "This application will not support different H & V scalings" << endl;
        exit(1);
    }

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
void resizeFunc(int w, int h)
{
    if (w != WIN_WIDTH || h != WIN_HEIGHT)
    {
        glutReshapeWindow(WIN_WIDTH, WIN_HEIGHT);
    }

    glViewport(0,0,WIN_WIDTH,WIN_HEIGHT);
    glutPostRedisplay();
}

void displayFunc(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Clear Screen And Depth Buffer
    
    glPushMatrix();

    //    right now edit mode is not implemented, so I draw all points the same
    for (auto pt : pointList)
        renderPoint(*pt, PointType::SINGLE_POINT);

    //    Again, no edit yet, so regardless of mode, all segments currently
    //    in the segment list are drawn the same
    for (size_t k=0; k<segmentList.size(); k+=2)
    {
        //    first draw the segment, then draw its endpoints
        renderSegment(*segmentList[k], *segmentList[k+1], SegmentType::SEGMENT);
        renderPoint(*segmentList[k], PointType::ENDPOINT);
        renderPoint(*segmentList[k+1], PointType::ENDPOINT);
    }
    
    //    if we are creating a segment, then draw the putative segment, based
    //    on current mouse pointer location
    if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick)
    {
        PointStruct nextPt = pixelToWorld(mouseX, mouseY);
        
        // If the shift key is down, we try to enforce closest among
        //    vertical/horizontal/diagonal
        if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
        {
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

        renderCreatedSegment(firstEndpoint, nextPt);
        renderPoint(firstEndpoint, PointType::FIRST_ENDPOINT);
        
    }
    
    glPopMatrix();
    glutSwapBuffers();
}

void myTimerFunc(int val)
{
    glutTimerFunc(15, myTimerFunc, val);
    
    // redraw the scene
    glutSetWindow(mainWindow);
    glutPostRedisplay();
}



void mouseHandlerFunc(int button, int state, int ix ,int iy)
{
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
    {
        PointStruct pt = pixelToWorld(ix, iy);
        
        switch (mode)
        {
            using enum ApplicationMode;
            
            case POINT_CREATION:
                pointList.push_back(Point::makeNewPt(pt.x, pt.y));
                break;
                
            case SEGMENT_CREATION:
                if (isFirstClick)
                {
                    firstEndpoint = pt;
                    mouseX = ix; mouseY = iy;
                    isFirstClick = false;
                }
                else
                {
                    segmentList.push_back(Point::makeNewPt(firstEndpoint.x, firstEndpoint.y));
                    segmentList.push_back(Point::makeNewPt(pt.x, pt.y));
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

void passiveMotionHandlerFunc(int x, int y)
{
    //    If we are creating a new segment and have already recorded
    //    the first endpoint, then we need to track the mouse position.
    if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick)
    {
        mouseX = x;
        mouseY = y;
    }
}

/*    Menu event handling        */
void menuHandlerFunc(int value)
{
    switch (value)
    {
        case EXIT_MENU_ITEM:
            zeEnd();
            break;
        
        case CLEAR_MENU_ITEM:
            switch (mode)
            {
                using enum ApplicationMode;
                case POINT_CREATION:
                    pointList.clear();
                    break;

                case SEGMENT_CREATION:
                    segmentList.clear();
                    isFirstClick = true;
                    break;
            
                default:
                break;
            }
            break;

        case CLEAR_ALL_MENU_ITEM:
            pointList.clear();
            segmentList.clear();
            isFirstClick = true;
            break;
            
        default:
            break;
    }
}

void modeMenuHandlerFunc(int value)
{
    switch (value)
    {
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
// Keyboard event handling
void keyboardHandlerFunc(unsigned char key, int x, int y)
{
    (void) x;
    (void) y;
printf("key: %c\n", key);
    switch (key)
    {
        case '\27':
            //    At this point, this is about the only effect
            if (mode == ApplicationMode::SEGMENT_CREATION)
            {
                //    If we had already recorded a first endpoint, forget it
                if (!isFirstClick)
                {
                    isFirstClick = true;
                }
            }
            break;
        
        //    ctrl-c should exit the program but doesn’t [??]
        case 'c':
            if (glutGetModifiers() & GLUT_ACTIVE_ALT)
                zeEnd();
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
#pragma mark Rendering code
//-----------------------------------------------------------------
#endif

void renderPoint(const Point& pt, PointType type)
{
    switch (type)
    {
        using enum PointType;
        
        case SINGLE_POINT:
            glColor4fv(POINT_COLOR[0]);
            break;

        case ENDPOINT:
            glColor4fv(POINT_COLOR[1]);
            break;
        
        case FIRST_ENDPOINT:
            glColor4fv(POINT_COLOR[2]);
            break;
        
        case EDIT_POINT:
            glColor4fv(POINT_COLOR[3]);
            break;
        
        default:
            break;
    }
    
    glPushMatrix();
    glTranslatef(pt.getX(), pt.getY(), 0.f);
    glScalef(POINT_DISK_RADIUS, POINT_DISK_RADIUS, 1.f);
    glBegin(GL_POLYGON);
    for (int k=0; k<NUM_CIRCLE_PTS; k++)
    {
        glVertex2f(CIRCLE_PT[k][0], CIRCLE_PT[k][1]);
    }
    glEnd();
    
    glPopMatrix();
}

void renderPoint(const PointStruct& pt, PointType type)
{
    switch (type)
    {
        using enum PointType;
        
        case SINGLE_POINT:
            glColor4fv(POINT_COLOR[0]);
            break;

        case FIRST_ENDPOINT:
            glColor4fv(POINT_COLOR[2]);
            break;
        
        case EDIT_POINT:
            glColor4fv(POINT_COLOR[3]);
            break;
        
        default:
            break;
    }
    
    glPushMatrix();
    glTranslatef(pt.x, pt.y, 0.f);
    glScalef(POINT_DISK_RADIUS, POINT_DISK_RADIUS, 1.f);
    glBegin(GL_POLYGON);
    for (int k=0; k<NUM_CIRCLE_PTS; k++)
    {
        glVertex2f(CIRCLE_PT[k][0], CIRCLE_PT[k][1]);
    }
    glEnd();
    
    glPopMatrix();
}


void renderSegment(const Point& pt1, const Point& pt2, SegmentType type)
{
    switch (type)
    {
        using enum SegmentType;
        
        case SEGMENT:
            glColor4fv(POINT_COLOR[0]);
            break;

        case EDITED_SEGMENT:
            glColor4fv(POINT_COLOR[2]);
            break;
        
        default:
            break;
    }
    
    glBegin(GL_LINES);
        glVertex2f(pt1.getX(), pt1.getY());
        glVertex2f(pt2.getX(), pt2.getY());
    glEnd();

}

void renderCreatedSegment(const PointStruct& pt1, const PointStruct& pt2)
{
    glColor4fv(POINT_COLOR[1]);
    glBegin(GL_LINES);
        glVertex2f(pt1.x, pt1.y);
        glVertex2f(pt2.x, pt2.y);
    glEnd();

}


const GLfloat*const* initCirclePt(size_t nbPts)
{
    GLfloat **circlePt = new GLfloat*[nbPts];
    for (size_t k=0; k<nbPts; k++)
    {
        circlePt[k] = new GLfloat[2];
        circlePt[k][0] = static_cast<GLfloat>(cos(2.f*M_PI*k/nbPts));
        circlePt[k][1] = static_cast<GLfloat>(sin(2.f*M_PI*k/nbPts));
    }

    return circlePt;
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Main, init, and exit
//-----------------------------------------------------------------
#endif

void myinit(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(X_MIN, X_MAX, Y_MIN, Y_MAX);
    
    // Create Menus
    int menu, modeSubMenu;
    
    // submenu for mode selection choices
    //  Note how I use my arrays defined in ApplConstants.h to automatically generate my menus
    modeSubMenu = glutCreateMenu(modeMenuHandlerFunc);
    glutAddMenuEntry("Point Creation", POINT_CREATION_CODE);
    glutAddMenuEntry("Segment Creation", SEGMENT_CREATION_CODE);

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


void zeEnd(void)
{
    pointList.clear();
    segmentList.clear();
    
    exit(0);
}

int main(int argc, char** argv)
{
    // initialize glut and create a new window
    glutInit(&argc,argv);
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

    myinit();

    glutMainLoop();
    
    return 0;
}
