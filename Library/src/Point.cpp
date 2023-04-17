//
//  Point.cpp
//  
//
//  Created by Yusra Suhail on 2/11/23.
//

#include <iostream>
#include <algorithm>
#include <memory>
#include <set>
#include <cmath>
#include "Geometry.hpp"
#include "Point.hpp"
#include "Segment.hpp"

using namespace std;
using namespace geometry;

//Static variables redeclared in source code
set<shared_ptr<Point> > Point::pointSet_;
vector<shared_ptr<Point> > Point::pointVect_;
unsigned int Point::count_ = 0;
float Point::pointDiskRadius_;
GLuint Point::diskList_ = 0;
GLuint Point::circleList_ = 0;
bool Point::displayListsInitialized_ = false;

#define NUM_CIRCLE_PTS	12

const GLfloat POINT_COLOR[][4] = {
								{0.f, 0.8f, 0.8f, 1.f},	//	SINGLE_POINT,
								{1.f, 0.5f, 0.f, 1.f},	//	ENDPOINT,
								{1.f, 1.f, 0.f, 1.f},	//	FIRST_ENDPOINT,
								{1.f, 0.f, 1.f, 1.f},	//	EDIT_POINT
								{0.f, 1.f, 0.f, 1.f}	//	INTERSECTION_POINT
};

/**Constructor function that can only be called using the class itself and not by user
 * @param xCoord  the x coordinate which will be used to create a point
 * @param yCoord  the y coordinate which will be used to create a point
 * @param token - the object of segmentToken class (which acts like passkey attribute) so that it is not accessible by anyone else
 */
Point::Point(PointToken token, float xCoord, float yCoord)
    :
        x_(xCoord),
        y_(yCoord),
        segList_(),
        idx_(count_++)
{
	(void) token;
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif
/**This is a maker function that returns a shared pointer to a point that can auto destruct itself when the point is no longer needed
 *@params xCoord - The x coordinate of the point
 *@params yCoord - The y coordinate of the point
 *@return a shared pointer to the point
 */
shared_ptr<Point> Point::makeNewPointPtr(float xCoord,float yCoord){
    bool found = false;
    shared_ptr<Point> p;
    /** First check if the point exists or not, if it is return the pointer otherwise make the new pointer*/
    for(auto iter = pointSet_.begin(); iter!=pointSet_.end();iter++){
        if((*iter)->x_ == xCoord &&  (*iter)->y_ == yCoord){
            found = true;
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }else{
        shared_ptr<Point> currPt = make_shared<Point>(PointToken{}, xCoord,yCoord);
        pointSet_.insert(currPt);
        return currPt;
    }
}
/**Maker function that calls the other constructor function and gets the point on that pointer
 * @params xCoord - The x coordinate of the point
 * @params yCoord - The y coordinate of the point
 * @return the reference to a point
 */
Point& Point::makeNewPoint(float xCoord,float yCoord){
    return *(makeNewPointPtr(xCoord, yCoord));
}
//Yusra: what is this function doing?
//I don't remember why I have this. Its definetly wrong
//shared_ptr<Point> Point::getPointAtIndex(size_t index){
//	shared_ptr<Point> ptr = nullptr;
//	if (index < pointSet_.size()){
//
//	}
//	return ptr;
//}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//-----------------------------------------------------------------
#endif

void Point::render(PointType type) const{
	if (!Point::displayListsInitialized_){
		Point::initDisplayLists_();
	}
	switch (type){
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
	glTranslatef(x_, y_, 0.f);
	glScalef(pointDiskRadius_, pointDiskRadius_, 1.f);
	glCallList(diskList_);
	glFlush();
	glPopMatrix();
}

void Point::render(const PointStruct& pt, PointType type){
	if (!Point::displayListsInitialized_){
		Point::initDisplayLists_();
	}
	
	switch (type){
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
		
		case INTERSECTION_POINT:
			glColor4fv(POINT_COLOR[4]);
			break;
		
		default:
			break;
	}
	
	glPushMatrix();
	glTranslatef(pt.x, pt.y, 0.f);
	glScalef(pointDiskRadius_, pointDiskRadius_, 1.f);
	glCallList(diskList_);
	glFlush();
	glPopMatrix();
}

void Point::renderAllSinglePoints(void){
	for (auto pt : pointSet_){
		if (pt->isSingle()){
			pt->render(PointType::SINGLE_POINT);
		}
	}
}

void Point::setPointDiskRadius(float radius){
	pointDiskRadius_ = radius;
}

void Point::initDisplayLists_(void){
	Point::diskList_ = glGenLists(1);
	glNewList(Point::diskList_, GL_COMPILE);
		glBegin(GL_POLYGON);
			for (size_t k=0; k<NUM_CIRCLE_PTS; k++){
				glVertex2f( static_cast<GLfloat>(cos(2.f*M_PI*k/NUM_CIRCLE_PTS)),
							static_cast<GLfloat>(sin(2.f*M_PI*k/NUM_CIRCLE_PTS)));
			}
		glEnd();
	glEndList();

	Point::circleList_ = glGenLists(1);
	glNewList(Point::circleList_, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			for (size_t k=0; k<NUM_CIRCLE_PTS; k++){
				glVertex2f( static_cast<GLfloat>(cos(2.f*M_PI*k/NUM_CIRCLE_PTS)),
							static_cast<GLfloat>(sin(2.f*M_PI*k/NUM_CIRCLE_PTS)));
			}
		glEnd();
	glEndList();
	
	Point::displayListsInitialized_ = true;
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Distance functions
//-----------------------------------------------------------------
#endif

float Point::distanceSq(float x1, float y1, float x2, float y2){
	float dx = x2 - x1;
	float dy = y2 - y1;
	float d2 = dx*dx + dy*dy;
	float r2 = x1*x1 + y1*y1;
	if ((r2 > Geometry::DISTANCE_ABS_SQ_TOL && d2/r2 < Geometry::DISTANCE_REL_SQ_TOL) ||
		(d2 < Geometry::DISTANCE_ABS_SQ_TOL)){
		d2 = 0.f;
	}
	return d2;

}

