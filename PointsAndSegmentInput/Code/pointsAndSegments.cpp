/*------------------------------------------------------------------------------+
|	This simple demo lets the user select one of two modes through a pop-up		|
|	menu (right click):	- point creation										|
|						- segment creation										|
|																				|
|	If the program ccan be launched with the path to a data file containing		|
|	lists of points and segments in the app's custom format presented in the	|
|	attached Data File Format.md												|
|																				|
|	Points and segment endpoints are recorded at the location of mouse clicks.	|
|	In segment creation mode, two consecutive clicks define the segment's two	|
|	endpoints.  																|
|																				|
|	Initially I wanted to use pressing the SHIFT key to constrain the segment 	|
|	to a horizontal, vertical, or diagonal direction (whichever is closest to 	|
|	the one actually entered by the user). However, glut only checks the SHIFT	|
|	key in the mouse and keyboard callback functions, NOT the display or mouse 	|
|	motion callback functions. 													|
|	So, instead, I added "snap to" modes that can be toggled on/off via a menu.	|
|	I am starting with:															|
|		o horizontal,															|
|		o vertical,																|
|		o diagonal.																|
|	If the segment being created (first endpoint clicked in, second not yet),	|
|	is within 5º (0.0872665 rad.) of a snap direction, the closest point on		|
|	that line will be chosen as endpoint.										|
|	Later on, I would like to add possibility to snap to:						|
|		o an existing point, 													|
|		o an existing segment.													|
|	In both cases if the click is within 1.5 * disk radius of the target.		|
|	Snap precedence order:														|
|		(1) point, (2) segment, (3) vertical, horizontal, diagonal.				|
|																				|
|	Hitting the 'ESC' button resets all current mode and editing selections.	|
|	For example, in segment creation mode, if the user creates a first endpoint	|
|	then hits the 'ESC' key, that first segment point is discarded (but the		|
|	mode remains segment creation).												|
|																				|
|	The Application  can only be quit by selecting 'EXIT' in the popup menu		|
|	or hitting SHIFT-ESC at the keyboard.										|															|											|
|																				|
|	jyh, 2023-03-02.															|
+------------------------------------------------------------------------------*/

#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <cstdlib>
#include <cmath>
//
#include "glPlatform.hpp"
#include "World.hpp"
#include "Point.hpp"
#include "Segment.hpp"
#include "dataFileIO.hpp"

using namespace std;
using namespace geometry;


//-----------------------------------------------------------------
//  function prototypes
//-----------------------------------------------------------------
PointStruct pixelToWorld(float ix, float iy);
PointStruct worldToPixel(float x, float y);
void menuHandlerFunc(int value);
void modeMenuHandlerFunc(int value);
void snapMenuHandlerFunc(int value);
void displayFunc(void);
void resizeFunc(int w, int h);
void mouseHandlerFunc(int b, int s, int x, int y);
void passiveMotionHandlerFunc(int x, int y);
void keyboardHandlerFunc(unsigned char, int x, int y);

/**	This function modifies the current pointer position according to the
 *	snap-to-point and snap-to-segment settings.
 *	@return true if the current pointer position has been modified, flase otherwise
 */
bool snapPoint(void);

/**	This function modifies the current pointer position according to the
 *	all the snap-to settings.
 *	@return true if the current pointer position has been modified, flase otherwise
 */
bool snapAll(void);

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
float World::POINT_DISK_RADIUS;
float World::PIXEL_TO_WORLD;
float World::WORLD_TO_PIXEL;
float World::SNAP_TO_ANGLE_TOL;
float World::SNAP_TO_POINT_TOL;
float World::SNAP_TO_SEGMENT_TOL;
const float World::POINT_PIXEL_RADIUS = 5.f;
const float World::DISTANCE_TOL = (float) 1E-5;
const float World::DISTANCE_SQ_TOL = World::DISTANCE_TOL*World::DISTANCE_TOL;

//-----------------------------------------------------------------
//  GUI constants
//-----------------------------------------------------------------

//	Dimensions of the drawing pane (might be different from dimensions of window
//	if we add a GUI pane on the side)
//	I won't authorize window resizing.
int PANE_WIDTH=800, PANE_HEIGHT=800;
int WIN_WIDTH=PANE_WIDTH, WIN_HEIGHT=PANE_HEIGHT;


const GLint	EXIT_MENU_ITEM = 0,
            CLEAR_MENU_ITEM = 1,
			CLEAR_ALL_MENU_ITEM = 2,
			//
			FIND_INTERSECTION_BRUTE = 3,
			FIND_INTERSECTION_SMART = 4,
			//
			SAVE_TO_FILE = 5,
			RESTORE_FROM_FILE = 6,
            //
            SEPARATOR = -1;

const GLint	POINT_CREATION_CODE = 10,
			SEGMENT_CREATION_CODE = 11;
//			POINT_EDIT_CODE = 20,
//			SEGMENT_EDIT_CODE = 21;

const int SNAP_START_CODE = 20;
enum SnapToCode{
	SNAP_TO_VERTICAL = 0,
	SNAP_TO_HORIZONTAL,
	SNAP_TO_DIAGONAL,
	SNAP_TO_POINT,
	SNAP_TO_SEGMENT
};

const string SNAP_MENU_STR[] = {
	"Snap to vertical: ",
	"Snap to horizontal: ",
	"Snap to diagonal: ",
	"Snap to point: ",
	"Snap to segment: "
};
const int NUM_SNAP_MODES = (int)(sizeof(SNAP_MENU_STR)/sizeof(SNAP_MENU_STR[0]));

const string rootFilePath = "../../../savedScene";


//-----------------------------------------------------------------
//  file-level global mode-related variables
//-----------------------------------------------------------------

//	Right now there is only a main window.  We may have subwindow indices.
int mainWindow;

bool isFirstClick = true;
PointStruct firstEndpoint;
PointStruct currPixelPt;
//int mouseX, mouseY;

ApplicationMode mode = ApplicationMode::NO_MODE;

int mainMenu = -1, modeSubMenu = -1, snapSubMenu = -1;
bool snapToVertical = false;
bool snapToHorizontal = false;
bool snapToDiagonal = false;
bool snapToPoint = false;
bool snapToSegment = false;

string dataFilePath;

vector <unique_ptr<PointStruct> > intersectionPointList;

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Scaling, conversion, &  functions
//-----------------------------------------------------------------
#endif

/** Function to setup the window as to track the mouse to draw polygon starting position at click position
 * @param ix - the x coordinate of the mouseClick position
 * @param iy - the y coordinate of the mouseClick position
 * @return a pointStruct that will create the point at the mouse position
 */
PointStruct pixelToWorld(float ix, float iy){
	// The pixel coordinates are transformed into "world" coordinates
    // Note: The world's  y direction points "up" but the pixel direction points down
	PointStruct pt = {World::X_MIN + ix*World::PIXEL_TO_WORLD,
		World::Y_MIN + (PANE_HEIGHT-iy)*World::PIXEL_TO_WORLD};

	return pt;
}
/** Function to
 * @param x - the x coordinate
 * @param y - the y coordinate
 * @return a pointStruct
 */
PointStruct worldToPixel(float x, float y){
	PointStruct pixelPt = {(x-World::X_MIN)*World::WORLD_TO_PIXEL,
		PANE_HEIGHT - (y-World::Y_MIN)*World::WORLD_TO_PIXEL};

	return pixelPt;
}

bool snapPoint(void){
	return false;
}

bool snapAll(void){
	return false;
}


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Callback functions
//-----------------------------------------------------------------
#endif

/**	This resize function ignores attempts by the user to resize the window and sets it back to the values st in the code*/
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	
	glPushMatrix();

	Segment::renderAllSegments();
	Point::renderAllSinglePoints();
		
	/**	If we are creating a segment, then draw the putative segment, based on current mouse pointer location */
	if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick){
		PointStruct nextPt = pixelToWorld(currPixelPt.x, currPixelPt.y);
		
		/** If the shift key is down, we try to enforce closest among vertical/ horizontal/ diagonal */
		if (glutGetModifiers() & GLUT_ACTIVE_SHIFT){
			/**	compute deltaX and deltaY, and find out the closest direction to enforce*/
			float dx = nextPt.x - firstEndpoint.x;
			float dy = nextPt.y - firstEndpoint.y;
			
			//	as much as I dislike (and advocate against) using angles
			//	in geometrical applications, here it is by far the simplest
			//	solution
			float angle = atan2f(dy, dx);
			float rad = sqrtf(dx*dx + dy*dy);
			
			/** snap to the closes multiple of π/4 */
			int mult = static_cast<int>(round(4.f*angle/M_PI));
			angle =  static_cast<float>(mult * M_PI / 4.0);
			nextPt.x = firstEndpoint.x + rad*cosf(angle);
			nextPt.y = firstEndpoint.y - rad*sinf(angle);
		}

		Segment::renderCreated(firstEndpoint, nextPt);
	}

	for (size_t k=0; k<intersectionPointList.size(); k++) {
		Point::render(*(intersectionPointList[k]), PointType::INTERSECTION_POINT);
	}

	glPopMatrix();
	glutSwapBuffers();
}

void myTimerFunc(int val){
	glutTimerFunc(15, myTimerFunc, val);
	
	/** redraw the scene*/
	glutSetWindow(mainWindow);
	glutPostRedisplay();
}



void mouseHandlerFunc(int button, int state, int ix ,int iy){
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON){
		currPixelPt.x = ix;
		currPixelPt.y = iy;
		
		if (isFirstClick){
			snapPoint();
		}
		else{
			snapAll();
		}
		PointStruct pt = pixelToWorld(currPixelPt.x, currPixelPt.y);
		
		switch (mode){
			using enum ApplicationMode;
			
			case POINT_CREATION:
				Point::makeNewPointPtr(pt.x, pt.y);
				break;
				
			case SEGMENT_CREATION:
				if (isFirstClick){
					firstEndpoint = pt;
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
	// If we are creating a new segment and have already recorded the first
	//	endpoint, then we need to track the mouse position
	if (mode == ApplicationMode::SEGMENT_CREATION && !isFirstClick){
//		mouseX = x;
//		mouseY = y;
	}
}

/*	Menu event handling		*/
void menuHandlerFunc(int value){
	switch (value){
		case EXIT_MENU_ITEM:
			zeEnd();
			break;
		
		case CLEAR_MENU_ITEM:
			switch (mode){
				using enum ApplicationMode;
				case POINT_CREATION:
//					pointList.clear();
					break;

				case SEGMENT_CREATION:
//					segmentList.clear();
					isFirstClick = true;
					break;
		    
				default:
				break;
			}
			break;

		case CLEAR_ALL_MENU_ITEM:
//			segmentList.clear();
//			pointList.clear();
			isFirstClick = true;
			break;

		case FIND_INTERSECTION_BRUTE:
			intersectionPointList = geometry::findAllIntersectionsBruteForce(Segment::getAllSegments());
			break;
			
		case FIND_INTERSECTION_SMART:
			break;

		case SAVE_TO_FILE:
			break;
			
		case RESTORE_FROM_FILE:
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

void snapMenuHandlerFunc(int value){
	glutSetMenu(snapSubMenu);
	switch (value-SNAP_START_CODE){
		case SNAP_TO_VERTICAL:
			snapToVertical = !snapToVertical;
			{
				string valStr = snapToVertical ? "on" : "off";
				glutChangeToMenuEntry(SNAP_TO_VERTICAL+1,
									(SNAP_MENU_STR[SNAP_TO_VERTICAL]+valStr).c_str(),
									SNAP_TO_VERTICAL+SNAP_START_CODE);
			}
			break;
	
		case SNAP_TO_HORIZONTAL:
			snapToHorizontal = !snapToHorizontal;
			{
				string valStr = snapToHorizontal ? "on" : "off";
				glutChangeToMenuEntry(SNAP_TO_HORIZONTAL+1,
									(SNAP_MENU_STR[SNAP_TO_HORIZONTAL]+valStr).c_str(),
									SNAP_TO_HORIZONTAL+SNAP_START_CODE);
			}
			break;
	
		case SNAP_TO_DIAGONAL:
			snapToDiagonal = !snapToDiagonal;
			{
				string valStr = snapToDiagonal ? "on" : "off";
				glutChangeToMenuEntry(SNAP_TO_DIAGONAL+1,
									(SNAP_MENU_STR[SNAP_TO_DIAGONAL]+valStr).c_str(),
									SNAP_TO_DIAGONAL+SNAP_START_CODE);
			}
			break;
	
		case SNAP_TO_POINT:
			snapToPoint = !snapToPoint;
			{
				string valStr = snapToPoint ? "on" : "off";
				glutChangeToMenuEntry(SNAP_TO_POINT+1,
									(SNAP_MENU_STR[SNAP_TO_POINT]+valStr).c_str(),
									SNAP_TO_POINT+SNAP_START_CODE);
			}
			break;
	
		case SNAP_TO_SEGMENT:
			snapToSegment = !snapToSegment;
			{
				string valStr = snapToSegment ? "on" : "off";
				glutChangeToMenuEntry(SNAP_TO_SEGMENT+1,
									(SNAP_MENU_STR[SNAP_TO_SEGMENT]+valStr).c_str(),
									SNAP_TO_SEGMENT+SNAP_START_CODE);
			}
			break;
	
		default:
			break;
	}
	glutSetMenu(mainMenu);
}

// Keyboard event handling
void keyboardHandlerFunc(unsigned char key, int x, int y){
	(void) x;
	(void) y;
	switch (key){
		/**	Escape key*/
		case 27:
			/**	Alt-ESC makes us exit the application*/
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
				zeEnd();
			
			/**	At this point, this is about the only effect*/
			if (mode == ApplicationMode::SEGMENT_CREATION){
				/**	If the first endpoint of the segment is already recorded then forget it*/
				if (!isFirstClick){
					isFirstClick = true;
				}
			}
			break;
		
		/**	Ctrl-S saves the current scene as a data file */
		case 's':
		case 'S':
			if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
				writeDataFile(rootFilePath);
			}
			break;

		/**Ctrl-R restores the scene to the state specified by the last saved file or the input data file if any.
         *Does nothing otherwise
         */
		case 'r':
		case 'R':
			if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
				readDataFile(dataFilePath, PANE_WIDTH, PANE_HEIGHT);
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
	/**	at this point, all callback calls are for this window */
	/** make the background BLACK */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/** setup callbacks*/
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(resizeFunc);
	glutMouseFunc(mouseHandlerFunc);
	glutPassiveMotionFunc(passiveMotionHandlerFunc);
	glutKeyboardFunc(keyboardHandlerFunc);
	glutTimerFunc(15, myTimerFunc, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(World::X_MIN, World::X_MAX, World::Y_MIN, World::Y_MAX);
	
	/** Submenu for mode selection choices
     * Note  how I use my arrays defined in ApplConstants.h to automatically generate my menus*/
	modeSubMenu = glutCreateMenu(modeMenuHandlerFunc);
	glutAddMenuEntry("Point Creation", POINT_CREATION_CODE);
	glutAddMenuEntry("Segment Creation", SEGMENT_CREATION_CODE);

	/** Submenu for toggling on/off "snap-to" modes**/
	snapSubMenu = glutCreateMenu(snapMenuHandlerFunc);
	for (int k=0; k< NUM_SNAP_MODES; k++){
		glutAddMenuEntry((SNAP_MENU_STR[k] + "off").c_str(), SNAP_START_CODE + k);
	}

	/** Main menu that the submenus are connected to */
	mainMenu = glutCreateMenu(menuHandlerFunc);
	glutAddSubMenu("Mode", modeSubMenu);
	glutAddSubMenu("Snap to", snapSubMenu);
	glutAddMenuEntry("Clear for mode", CLEAR_MENU_ITEM);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Clear All", CLEAR_ALL_MENU_ITEM);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Find All Intersections (brute force)", FIND_INTERSECTION_BRUTE);
	glutAddMenuEntry("Find All Intersections (smart)", FIND_INTERSECTION_SMART);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Save to File", SAVE_TO_FILE);
	glutAddMenuEntry("Restore from File", RESTORE_FROM_FILE);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Exit", EXIT_MENU_ITEM);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void applicationInit(int argc, char* argv[]){
	/**	If the program has one argument, it should be the path to a data file */
	if (argc == 2){
		/**	save the path to be able to reload the scene */
		dataFilePath = argv[1];
		readDataFile(dataFilePath, PANE_WIDTH, PANE_HEIGHT);
	}else if (argc > 2){
		cout << "This program accets only one argument (path to a data file " <<
				"or none at all." << endl;
		exit(3);
	}else{
		World::setWorldBounds(World::X_MIN, World::X_MAX, World::Y_MIN, World::Y_MAX,
								PANE_WIDTH, PANE_HEIGHT);
	}
	WIN_WIDTH=PANE_WIDTH;
	WIN_HEIGHT=PANE_HEIGHT;
	
}

void zeEnd(void){
//	pointList.clear();
//	segmentList.clear();
	Point::clearAllPoints();
	Segment::clearAllSegments();
	exit(0);
}

int main(int argc, char** argv){
	/** It's generally recommended to initialize glut early. */
	glutInit(&argc, argv);

	/**	This only checcks if by any chance we have a file path as argument, in which case we will load lists of  predefined points and segments.*/
	applicationInit(argc, argv);
	
	/**	Initialize the GUI (create window, set up menus and callback functions*/
	interfaceInit();
	
	/**	And now we are ready to relinquish control to glut */
	glutMainLoop();
	
	return 0;
}



