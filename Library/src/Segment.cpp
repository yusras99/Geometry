//
//  Segment.cpp
//
//
//  Created by Yusra Suhail on 2/11/23.
//

#include <iostream>
#include <algorithm>
#include <set>
#include <memory>
#include <utility>

#include "Segment.hpp"
#include "Point.hpp"


using namespace std;
using namespace geometry;

/*
 *Static variables redeclared in source code
 */
set<shared_ptr<Segment> > Segment::segSet_;
unsigned int Segment::count_ = 0;


const GLfloat SEGMENT_COLOR[][4] = {
                                    {1.f, 1.f, 1.f, 1.f},    //    SEGMENT,
                                    {0.7f, 1.f, 0.7f, 1.f},    //    CREATED_SEGMENT,
                                    {0.f, 0.f, 1.f, 1.f}    //    EDITED_SEGMENT
};

Segment::Segment(SegmentToken token, Point& pt1, Point& pt2)
    :
        p1_(pt1),
        p2_(pt2),
        idx_(count_++)
{
    (void) token;
    
    //    add myself to segList of p1 and p2
    p1_.segList_.insert(idx_);
    p2_.segList_.insert(idx_);
}

Segment::~Segment(void){
    //    remove myself from segList of p1 and p2
    p1_.segList_.erase(idx_);
    p2_.segList_.erase(idx_);
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif

/**
 * Constructuctor function for the segment that will create a shared pointer to the segment so it can be stored in the set
 * @param pt1 a reference to a point 1 which will be used to create a segment
 * @param pt2 a reference to a point 2 which will be used to create a segment
 * @return a shared pointer to the segment
 */
shared_ptr<Segment> Segment::makeNewSegPtr(Point& pt1, Point& pt2){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet_.begin(); iter!=segSet_.end();iter++){
        //segSet is a set of pointers to segments, which points to the endPt pointers, so we need to dereference the segment pointer and then dereference the endpt pointers to check the endpt pairs
        if( ((*iter)->p1_.idx_ == pt1.idx_ && (*iter)->p2_.idx_ == pt2.idx_) ||
            ((*iter)->p2_.idx_ == pt1.idx_ && (*iter)->p1_.idx_ == pt2.idx_)){
            found = true;
            //return pointer to the segment
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }else{
        shared_ptr<Segment> currSeg = make_shared<Segment>(SegmentToken{}, pt1, pt2);
        segSet_.insert(currSeg);
        //push endpoint and segment index to the segList
        pt1.segList_.insert(currSeg->idx_);
        pt2.segList_.insert(currSeg->idx_);
        return currSeg;
    }
}

Segment& Segment::makeNewSeg(Point& pt1, Point& pt2){
   return *(makeNewSegPtr(pt1, pt2));
}


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//-----------------------------------------------------------------
#endif

void Segment::render(SegmentType type) const{
    //    first draw the segment, then draw its endpoints
    render_(p1_, p2_, type);
    p1_.render(PointType::ENDPOINT);
    p2_.render(PointType::ENDPOINT);
}

void Segment::render_(const Point& pt1, const Point& pt2, SegmentType type){
    switch (type){
        using enum SegmentType;
        
        case SEGMENT:
            glColor4fv(SEGMENT_COLOR[0]);
            break;

        case EDITED_SEGMENT:
            glColor4fv(SEGMENT_COLOR[2]);
            break;
        
        default:
            break;
    }
    
    glBegin(GL_LINES);
        glVertex2f(pt1.getX(), pt1.getY());
        glVertex2f(pt2.getX(), pt2.getY());
    glEnd();
}

void Segment::renderCreated(const PointStruct& pt1, const PointStruct& pt2){
    glColor4fv(SEGMENT_COLOR[1]);
    
    glBegin(GL_LINES);
        glVertex2f(pt1.x, pt1.y);
        glVertex2f(pt2.x, pt2.y);
    glEnd();

    Point::render(pt1, PointType::FIRST_ENDPOINT);
}

void Segment::renderAllSegments(void){
    for (auto seg : segSet_) {
        seg->render(SegmentType::SEGMENT);
    }
}



//
//  Segment.cpp
//
//
//  Created by Yusra Suhail on 2/11/23.
//

#include <iostream>
#include <algorithm>
#include <set>
#include <memory>
#include <utility>

#include "Segment.hpp"
#include "Point.hpp"


using namespace std;
using namespace geometry;

//Static variables redeclared in source code
set<shared_ptr<Segment> > segSet;
unsigned int Segment::count = 0;

Segment::Segment(Point& pt1, Point& pt2)
    :
        p1(pt1),
        p2(pt2),
        idx(count++)
{
    //    add myself to segList of p1 and p2
    p1.segList.insert(idx);
    p2.segList.insert(idx);
}

shared_ptr<Segment> Segment::makeNewSeg(Point& pt1, Point& pt2){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet.begin(); iter!=segSet.end();iter++){
        //segSet is a set of pointers to segments, which points to the endPt pointers, so we need to dereference the segment pointer and then dereference the endpt pointers to check the endpt pairs
        if( ((*iter)->p1.idx == pt1.idx && (*iter)->p2.idx == pt2.idx) ||
            ((*iter)->p2.idx == pt1.idx && (*iter)->p1.idx == pt2.idx)){
            found = true;
            //return pointer to the segment
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }
    else{
        shared_ptr<Segment> currSeg = make_shared<Segment>(pt1,pt2);
        segSet.insert(currSeg);
        //push endpoint and segment index to the segList
        pt1.segList.insert(currSeg->idx);
        pt2.segList.insert(currSeg->idx);
        return currSeg;
    }
}
#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Intersection-related functions
//-----------------------------------------------------------------
#endif


bool Segment::isOnLeftSide(const Point& pt) const{
    /**The determinant of a pt to the segment is:
     *  det( p2 - p1, pt - p1)
     */
    float det  = ((pt.x_ - p1_.x_) * (p2_.y_ - p1_.y_)) - ((p2_.x_ - p1_.x_) * (pt.y_ - p1_.y_ ));
    if(det > 0){
        return true;
    }else{
        return false;
    }
}

bool Segment::isOnLeftSide(const PointStruct& pt) const{
    float det  = ((pt.x - p1_.x_) * (p2_.y_ - p1_.y_)) - ((p2_.x_ - p1_.x_) * (pt.y - p1_.y_ ));
    if(det > 0){
        return true;
    }else{
        return false;
    }
}

bool Segment::areOnOppositeSides(const Point& pt1, const Point& pt2) const{
    bool pt1IsLeft = isOnLeftSide(pt1);
    bool pt2IsLeft = isOnLeftSide(pt2);
    return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}

bool Segment::areOnOppositeSides(const PointStruct& pt1, const PointStruct& pt2) const{
    bool pt1IsLeft = isOnLeftSide(pt1);
    bool pt2IsLeft = isOnLeftSide(pt2);
    return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}
//    Intersection possible iff:
//    Seg1.pt1 and seg1.pt2 are on different sides of Seg2
//    Seg2.pt1 and seg2.pt2 are on different sides of Seg1
bool Segment::intersects(const Segment& seg) const{
    return areOnOppositeSides(seg.p1_, seg.p2_);
}

bool Segment::intersects(const Point& pt1, const Point& pt2) const{
    return areOnOppositeSides(pt1, pt2);
}

bool Segment::intersects(const PointStruct& pt1, const PointStruct& pt2) const{
        return areOnOppositeSides(pt1, pt2);
}
PointStruct* Segment::findIntersection(const Segment& interSeg){
    float interX = 0.0;
    float interY = 0.0;
    PointStruct interPoint = {interX,interY};
    PointStruct* ptr = &interPoint;
    if(intersects(interSeg)){
        /**variable that is essentially a divisor of the intersection point's x and y value - currSeg's x and y value**/
        float factor = 0.0;
        /**variables that are just difference of the x and y values for both segments **/
        float interSegX = interSeg.p1_.x_ - interSeg.p2_.x_;
        float currSegX = p1_.x_ - p2_.x_;
        float interSegY = interSeg.p1_.y_ - interSeg.p2_.y_;
        float currSegY = p1_.y_ - p2_.y_;
        /**Intersection point M belongs to currSeg (A, B) so there exists α  s.t. M = A + α(B-A)
         *The  interSeg(C, D) and the segment from (C, M) are colinear (determinant of the 2 vectors is 0):
         *      det(A + α(B-A) - C, D - C) = 0
         * Hence the equations below are solved for α using the above equation
         */
        float numerator = ((interSegX/interSegY)*(p1_.y_)) -        ((interSegX/interSegY)*(interSeg.p1_.x_)) - p1_.x_ + interSeg.p1_.x_;
        float denominator = currSegX - ((interSegX/interSegY)*currSegY);
        factor = numerator / denominator;
        
        interX = p1_.x_ + (factor * (p2_.x_ - p1_.x_));
        interY = p1_.y_ + (factor * (p2_.y_ - p1_.y_));
        
        interPoint = {interX,interY};
        return ptr;
    }else{
        /**if no intersection found return a null pointer*/
        return NULL;
    }
}
