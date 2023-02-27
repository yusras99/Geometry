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
#include "Point.hpp"
#include "Segment.hpp"

using namespace std;
using namespace geometry;

//Static variables redeclared in source code
set<shared_ptr<Point> > Point::pointSet_;
unsigned int Point::count_ = 0;
float Point::pointDiskRadius_;
GLuint Point::diskList_ = 0;
GLuint Point::circleList_ = 0;
bool Point::listsInitialized_ = false;
 
#define NUM_CIRCLE_PTS    12

const GLfloat POINT_COLOR[][4] = {
                                {0.f, 0.8f, 0.8f, 1.f},    //    SINGLE_POINT,
                                {1.f, 0.5f, 0.f, 1.f},    //    ENDPOINT,
                                {1.f, 1.f, 0.f, 1.f},    //    FIRST_ENDPOINT,
                                {1.f, 0.f, 1.f, 1.f}    //    EDIT_POINT
};


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

shared_ptr<Point> Point::makeNewPointPtr(float xCoord,float yCoord){
    bool found = false;
    shared_ptr<Point> p;
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

Point& Point::makeNewPoint(float xCoord,float yCoord){
    return *(makeNewPointPtr(xCoord, yCoord));
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//-----------------------------------------------------------------
#endif

void Point::render(PointType type) const{
    if (!Point::listsInitialized_){
        Point::initLists_();
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
    if (!Point::listsInitialized_){
        Point::initLists_();
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

void Point::initLists_(void){
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
    
    Point::listsInitialized_ = true;
}

float Point::distance(const PointStruct& pt1, const PointStruct& pt2){
    float dx = pt2.x - pt1.x;
    float dy = pt2.y - pt1.y;
    return sqrtf(dx*dx + dy*dy);
}

float Point::distance(const PointStruct& pt) const{
    float dx = pt.x - x_;
    float dy = pt.y - y_;
    return sqrtf(dx*dx + dy*dy);
}

float Point::distance(const Point& pt) const{
    float dx = pt.x_ - x_;
    float dy = pt.y_ - y_;
    return sqrtf(dx*dx + dy*dy);
}

