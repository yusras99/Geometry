//
//  main.cpp
//  PolygonPartitioning
//
//  Created by Jean-Yves Hervé on 2023-06-02.
//

#include <vector>
#include <iostream>
#include "World.hpp"
#include "Triangle.hpp"
#include "Polygon.hpp"
#include "dataFileIO.hpp"

using namespace std;
using namespace geometry;

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Custom data types
//-----------------------------------------------------------------
#endif

enum class ApplicationMode{
	NO_MODE = -1,
	//
	POLYGON_CREATION,		//	0
	POLYGON_EDITING,		//	1
	//
	NUM_MODES
};

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Functions prototypes
//-----------------------------------------------------------------
#endif

PointStruct pixelToWorld(float ix, float iy);
PointStruct worldToPixel(float x, float y);
void menuHandlerFunc(int value);
void modeMenuHandlerFunc(int value);
void snapMenuHandlerFunc(int value);
void displayFunc();
void myTimerFunc(int val);
void resizeFunc(int w, int h);
void mouseHandlerFunc(int b, int s, int x, int y);
void passiveMotionHandlerFunc(int x, int y);
void keyboardHandlerFunc(unsigned char, int x, int y);

/**	This function modifies the current pointer position according to the
 *	snap-to-point and snap-to-segment settings.
 *	@return true if the current pointer position has been modified, flase otherwise
 */
bool snapPoint();

/**	This function modifies the current pointer position according to the
 *	all the snap-to settings.
 *	@return true if the current pointer position has been modified, flase otherwise
 */
bool snapAll();

void interfaceInit();
void applicationInit(int argc, char* argv[]);
void zeEnd();

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Library and application constants
//-----------------------------------------------------------------
#endif


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


const float Geometry::DISTANCE_ABS_TOL = 3E-4f;
const float Geometry::DISTANCE_REL_TOL = 1E-6f;
const float Geometry::DISTANCE_ABS_SQ_TOL = 1E-6f;
const float Geometry::DISTANCE_REL_SQ_TOL = 1E-7f;
const int Geometry::SMALL_FONT_HEIGHT = 12;
const int Geometry::LARGE_FONT_HEIGHT = 18;
const int Geometry::TEXT_PADDING = 0;
const float Geometry::TextColor[4] = {1.f, 1.f, 1.f, 1.f};
bool Geometry::renderLabels = true;

//	Dimensions of the drawing pane (might be different from dimensions of window
//	if we add a GUI pane on the side)
//	I won't authorize window resizing.
int PANE_WIDTH=800, PANE_HEIGHT=800;
int WIN_WIDTH=PANE_WIDTH, WIN_HEIGHT=PANE_HEIGHT;

const GLint	EXIT_MENU_ITEM = 0,
			TOGGLE_RENDER_LABELS = 1,
            CLEAR_CURRENT_MENU_ITEM = 2,
			CLEAR_ALL_MENU_ITEM = 3,
			//
			MONOTONE_PARTITION = 4,
			CONVEX_PARTITION = 5,
			TRIANGULATION = 6,
			//
			SAVE_TO_FILE = 7,
			RESTORE_FROM_FILE = 8,
            //
            SEPARATOR = -1;

const GLint	POLYGON_CREATION_CODE = 10,
			POLYGON_EDITING_CODE = 11;

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

const string rootFilePath = "../../../../DataFiles/savedPolygon";

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark file-level global & mode-related variables
//-----------------------------------------------------------------
#endif

//	Right now there is only a main window.  We may have subwindow indices.
int mainWindow;

ApplicationMode mode = ApplicationMode::NO_MODE;

int mainMenu = -1, modeSubMenu = -1, snapSubMenu = -1;
bool snapToVertical = false;
bool snapToHorizontal = false;
bool snapToDiagonal = false;
bool snapToPoint = false;
bool snapToSegment = false;

string dataFilePath = "";
bool isFirstClick = true;
PointStruct currPixelPt;
vector<shared_ptr<Polygon> > polygonList;
vector<shared_ptr<PointStruct> > currentPolygon;

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

bool snapPoint(){
	return false;
}

bool snapAll(){
	return false;
}

void renderLabel(const string& prefix, unsigned int index, int xPos, int yPos, bool isLarge){
    //-----------------------------------------------
    //  0.  get current material properties
    //-----------------------------------------------
    float oldAmb[4], oldDif[4], oldSpec[4], oldShiny;
    glGetMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
    glGetMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
    glGetMaterialfv(GL_FRONT, GL_SHININESS, &oldShiny);

    glPushMatrix();
	
    //-----------------------------------------------
    //  1.  Build the string to display <-- parameter
    //-----------------------------------------------
    string label = prefix + to_string(index);
    const char* labelStr = label.c_str();
    size_t labelLn = label.length();

    //-----------------------------------------------
    //  2.  Determine the string's length (in pixels)
    //-----------------------------------------------
    int textWidth = 0;
    for (size_t k=0; k<labelLn; k++)
	{
		if (isLarge)
			textWidth += glutBitmapWidth(LARGE_DISPLAY_FONT, labelStr[k]);
		else
			textWidth += glutBitmapWidth(SMALL_DISPLAY_FONT, labelStr[k]);
		
    }
	//  add a few pixels of padding
	textWidth += 2*Geometry::TEXT_PADDING;
	
	// Finally, scale back
	textWidth = (int)(textWidth / World::PIXEL_TO_WORLD + 1);
		
    //-----------------------------------------------
    //  3.  Draw the string
    //-----------------------------------------------
    glColor4fv(Geometry::TextColor);
    int x = xPos;
    for (size_t k=0; k<labelLn; k++)
    {
        glRasterPos2i(x, yPos);
		if (isLarge)
		{
			glutBitmapCharacter(LARGE_DISPLAY_FONT, labelStr[k]);
			x += static_cast<int>(glutBitmapWidth(LARGE_DISPLAY_FONT, labelStr[k])*World::PIXEL_TO_WORLD);
		}
		else
		{
			glutBitmapCharacter(SMALL_DISPLAY_FONT, labelStr[k]);
			x += static_cast<int>(glutBitmapWidth(SMALL_DISPLAY_FONT, labelStr[k])*World::PIXEL_TO_WORLD);
		}
	}

    //-----------------------------------------------
    //  5.  Restore old material properties
    //-----------------------------------------------
	glMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, oldShiny);
    
    //-----------------------------------------------
    //  6.  Restore reference frame
    //-----------------------------------------------
    glPopMatrix();
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Callback functions
//-----------------------------------------------------------------
#endif

/*	This resize function ignores attempts by the user to resize the window and sets it back to the values st in the code*/
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
		
	//------------------------------------
	//	Render all existing polygons
	//------------------------------------
	for (auto poly : polygonList)
		poly->render();
		
	//----------------------------------------------------
	//	Render the "current" polygon if in creation mode
	//----------------------------------------------------
	/*	If we are creating a polygon, then draw the putative last edge, based on current mouse pointer location */
	if (mode == ApplicationMode::POLYGON_CREATION){
		if (!isFirstClick){
			PointStruct nextPt = pixelToWorld(currPixelPt.x, currPixelPt.y);
			
			/* If the shift key is down, we try to enforce closest among vertical/ horizontal/ diagonal */
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT){
				/*	compute deltaX and deltaY, and find out the closest direction to enforce*/
//				float dx = nextPt.x - firstEndpoint.x;
//				float dy = nextPt.y - firstEndpoint.y;
//
//				//	as much as I dislike (and advocate against) using angles
//				//	in geometrical applications, here it is by far the simplest
//				//	solution
//				float angle = atan2f(dy, dx);
//				float rad = sqrtf(dx*dx + dy*dy);
//
//				/* snap to the closes multiple of π/4 */
//				int mult = static_cast<int>(round(4.f*angle/M_PI));
//				angle =  static_cast<float>(mult * M_PI / 4.0);
//				nextPt.x = firstEndpoint.x + rad*cosf(angle);
//				nextPt.y = firstEndpoint.y - rad*sinf(angle);
			}
		}
		
//		Segment::renderCreated(firstEndpoint, nextPt);
	}

//	for (size_t k=0; k<intersectionPointList.size(); k++) {
//		Point::render(*(intersectionPointList[k]), PointType::INTERSECTION_POINT);
//	}

//	//	Possibly render the label, on the right side of the point
//	if (Geometry::renderLabels){
//		const float deltaX = 1.8f*Point::getPointDiskRadius(),
//					deltaY = 0.5f*Point::getPointDiskRadius();
//		const set<shared_ptr<Point> >& pointList = Point::getAllPoints();
//		for (const auto& pt : pointList){
//			renderLabel("P_", pt->getIndex(), static_cast<int>(pt->getX() + deltaX),
//								static_cast<int>(pt->getY() - deltaY), false);
//		}
//
//		const vector<shared_ptr<Segment> >& segList = Segment::getAllSegments();
//		for (const auto& seg : segList){
//			float x = seg->getP1()->getX() - Point::getPointDiskRadius(),
//				  y = seg->getP1()->getY() + 2.5f*Point::getPointDiskRadius();
//			renderLabel("S_", seg->getIndex(), static_cast<int>(x),
//								static_cast<int>(y), false);
//		}
//
//	}
//
	glPopMatrix();
	glutSwapBuffers();
}

void myTimerFunc(int val){
	glutTimerFunc(15, myTimerFunc, val);
	
	/* redraw the scene*/
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
			
			case POLYGON_CREATION:
				
				break;
				
			case POLYGON_EDITING:
//				if (isFirstClick){
//					firstEndpoint = pt;
//					isFirstClick = false;
//				}
//				else{
//					Segment::makeNewSegPtr(Point::makeNewPointPtr(firstEndpoint.x, firstEndpoint.y),
//										   Point::makeNewPointPtr(pt.x, pt.y));
//					isFirstClick = true;
//				}
				break;
				
			default:
				break;
		
		}
	}

	glutSetWindow(mainWindow);
	glutPostRedisplay();
}

void passiveMotionHandlerFunc(int x, int y){
}

/*	Menu event handling		*/
void menuHandlerFunc(int value){
	switch (value){
		case EXIT_MENU_ITEM:
			zeEnd();
			break;
		
		case CLEAR_ALL_MENU_ITEM:
			isFirstClick = true;
			break;
		
		case CLEAR_CURRENT_MENU_ITEM:
			isFirstClick = true;
			break;
		
		case TOGGLE_RENDER_LABELS:
			Geometry::renderLabels = !Geometry::renderLabels;
			break;
			
		case MONOTONE_PARTITION:
			cout  << "Monotone partition not implemented yet" << endl;
			break;
			
		case CONVEX_PARTITION:
			cout  << "Convexp partition not implemented yet" << endl;
			break;
			
		case TRIANGULATION:
			cout  << "Triangulation not implemented yet" << endl;
			break;

		case SAVE_TO_FILE:
			// preserve the path to last saved file
			dataFilePath = writeDataFile(rootFilePath);
			break;
			
		case RESTORE_FROM_FILE:
			readDataFile(dataFilePath, PANE_WIDTH, PANE_HEIGHT);
			break;
 
		default:
			break;
	}
}

void modeMenuHandlerFunc(int value){
	switch (value){
		case  POLYGON_CREATION_CODE:
			mode = ApplicationMode::POLYGON_CREATION;
			isFirstClick = true;
			break;

		case POLYGON_EDITING_CODE:
			mode = ApplicationMode::POLYGON_EDITING;
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
		/*	Escape key*/
		case 27:
			/*	Alt-ESC makes us exit the application*/
			if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
				zeEnd();
			
			/*	At this point, this is about the only effect*/
			if (mode == ApplicationMode::POLYGON_CREATION){
				//	???
			}
			break;
		
		/*	Ctrl-S saves the current scene as a data file */
		case 's':
		case 'S':
			if (glutGetModifiers() & GLUT_ACTIVE_CTRL){
				dataFilePath = writeDataFile(rootFilePath);
			}
			break;

		/*Ctrl-R restores the scene to the state specified by the last saved file or the input data file if any.
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
#pragma mark Main and intialization
//-----------------------------------------------------------------
#endif

void interfaceInit(){
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowPosition(100,40);
	mainWindow = glutCreateWindow("Polygon Partitioning and Triangulation Demo");

	glutSetWindow(mainWindow);
	/*	at this point, all callback calls are for this window */
	/* make the background BLACK */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* setup callbacks*/
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(resizeFunc);
	glutMouseFunc(mouseHandlerFunc);
	glutPassiveMotionFunc(passiveMotionHandlerFunc);
	glutKeyboardFunc(keyboardHandlerFunc);
	glutTimerFunc(15, myTimerFunc, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(World::X_MIN, World::X_MAX, World::Y_MIN, World::Y_MAX);

const GLint	EXIT_MENU_ITEM = 0,
			TOGGLE_RENDER_LABELS = 1,
            CLEAR_CURRENT_MENU_ITEM = 2,
			CLEAR_ALL_MENU_ITEM = 3,
			//
			MONOTONE_PARTITION = 4,
			CONVEX_PARTITION = 5,
			TRIANGULATION = 6,
			//
			SAVE_TO_FILE = 7,
			RESTORE_FROM_FILE = 8,

	/* Submenu for mode selection choices
     * Note  how I use my arrays defined in ApplConstants.h to automatically generate my menus*/
	modeSubMenu = glutCreateMenu(modeMenuHandlerFunc);
	glutAddMenuEntry("Polygon Creation", POLYGON_CREATION_CODE);
	glutAddMenuEntry("Polygon Editing", POLYGON_EDITING_CODE);
	
	/* Submenu for toggling on/off "snap-to" modes**/
	snapSubMenu = glutCreateMenu(snapMenuHandlerFunc);
	for (int k=0; k< NUM_SNAP_MODES; k++){
		glutAddMenuEntry((SNAP_MENU_STR[k] + "off").c_str(), SNAP_START_CODE + k);
	}

	/* Main menu that the submenus are connected to */
	mainMenu = glutCreateMenu(menuHandlerFunc);
	glutAddSubMenu("Mode", modeSubMenu);
	glutAddSubMenu("Snap to", snapSubMenu);
	glutAddMenuEntry("Toggle render labels", TOGGLE_RENDER_LABELS);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Clear Current", CLEAR_CURRENT_MENU_ITEM);
	glutAddMenuEntry("Clear All", CLEAR_ALL_MENU_ITEM);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Monotone partition", MONOTONE_PARTITION);
	glutAddMenuEntry("Convex partition", CONVEX_PARTITION);
	glutAddMenuEntry("Triangulation", TRIANGULATION);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Save to File", SAVE_TO_FILE);
	glutAddMenuEntry("Restore from File", RESTORE_FROM_FILE);
	glutAddMenuEntry("-", SEPARATOR);
	glutAddMenuEntry("Exit", EXIT_MENU_ITEM);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void applicationInit(int argc, char* argv[]){
	/*	If the program has one argument, it should be the path to a data file */
	if (argc == 2){
		/*	save the path to be able to reload the scene */
		dataFilePath = argv[1];
		readDataFile(dataFilePath, PANE_WIDTH, PANE_HEIGHT);
	}else if (argc > 2){
		cout << "This program accepts only one argument (path to a data file " <<
				"or none at all." << endl;
		exit(3);
	}else{
		World::setWorldBounds(World::X_MIN, World::X_MAX, World::Y_MIN, World::Y_MAX,
								PANE_WIDTH, PANE_HEIGHT);
	}
	WIN_WIDTH=PANE_WIDTH;
	WIN_HEIGHT=PANE_HEIGHT;
	
}

void zeEnd(){
	polygonList.clear();
	currentPolygon.clear();
	Point::clearAllPoints();
	Segment::clearAllSegments();
	exit(0);
}

int main(int argc, char** argv){
	/* It's generally recommended to initialize glut early. */
	glutInit(&argc, argv);

	/*	This only checks if by any chance we have a file path as argument, in which case we will load lists of  predefined points and segments.*/
	applicationInit(argc, argv);
	
	/*	Initialize the GUI (create window, set up menus and callback functions*/
	interfaceInit();
	
	/*	And now we are ready to relinquish control to glut */
	glutMainLoop();
	
	//	not called
	zeEnd();
	
	return 0;
}

