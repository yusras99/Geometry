//
//  Segment.cpp
//  
//
//  Created by Yusra Suhail on 2/11/23.
//

#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <memory>
#include <utility>
#include "Segment.hpp"
#include "Point.hpp"

using namespace std;
using namespace geometry;

/**
 *Static variables redeclared in source code
 */
set<shared_ptr<Segment> > Segment::segSet_;
vector<shared_ptr<Segment> > Segment::segVect_;
unsigned int Segment::count_ = 0;


const GLfloat SEGMENT_COLOR[][4] = {
									{1.f, 1.f, 1.f, 1.f},	//	SEGMENT,
									{0.7f, 1.f, 0.7f, 1.f},	//	CREATED_SEGMENT,
									{0.f, 0.f, 1.f, 1.f}	//	EDITED_SEGMENT
};
/**Constructor function that can only be called using the class itself and not by user
 * @param pt1 a reference to a point 1 which will be used to create a segment
 * @param pt2 a reference to a point 2 which will be used to create a segment
 * @param token - the object of segmentToken class (which acts like passkey attribute) so that it is not accessible by anyone else
 */
Segment::Segment(SegmentToken token, shared_ptr<Point> pt1, shared_ptr<Point> pt2,
				bool endpointsOrdered)
    :
		segP1_(endpointsOrdered ? pt1 : (pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt1 :  pt2),
		segP2_(endpointsOrdered ? pt2 : (pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt2 :  pt1),
        segIdx_(count_++)
{
	(void) token;
}
Segment::Segment(shared_ptr<Point>  pt1, shared_ptr<Point>  pt2)
    :
		segP1_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt1 :  pt2),
		segP2_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt2 :  pt1),
        segIdx_(UINT_MAX)
{
}

/**Destructor function that clears the segment */
Segment::~Segment(void){
	/**	remove myself from segList of p1 and p2*/
	segP1_->segList_.erase(segVect_[segIdx_]);
	segP2_->segList_.erase(segVect_[segIdx_]);
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif

shared_ptr<Segment> Segment::makeNewSegPtr(shared_ptr<Point> pt1, shared_ptr<Point> pt2){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet_.begin(); iter!=segSet_.end();iter++){
        /** segSet is a set of pointers to segments
         *  so we need to dereference the segment pointer  to check the point pairs
         */
        if( ((*iter)->segP1_->idx_ == pt1->idx_ && (*iter)->segP2_->idx_ == pt2->idx_) ||
			((*iter)->segP2_->idx_ == pt1->idx_ && (*iter)->segP1_->idx_ == pt2->idx_)){
            found = true;
            /**return pointer to the segment*/
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }else{
        shared_ptr<Segment> currSeg = make_shared<Segment>(SegmentToken{}, pt1, pt2);
        segSet_.insert(currSeg);
        segVect_.push_back(currSeg);
		pt1->segList_.insert(currSeg);
		pt2->segList_.insert(currSeg);
        return currSeg;
    }
}

Segment& Segment::makeNewSeg(shared_ptr<Point> pt1, shared_ptr<Point> pt2){
   return *(makeNewSegPtr(pt1, pt2));
}

Segment Segment::makeNewTempSeg(shared_ptr<Point> pt1, shared_ptr<Point> pt2){
    return Segment(pt1, pt2);
}


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//-----------------------------------------------------------------
#endif

void Segment::render(SegmentType type) const{
	/**	first draw the segment, then draw its endpoints*/
	render_(*segP1_, *segP2_, type);
	segP1_->render(PointType::ENDPOINT);
	segP2_->render(PointType::ENDPOINT);
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

void Segment::renderAllSegments(){
	for (auto seg : segSet_) {
		seg->render(SegmentType::SEGMENT);
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
    float det  = ((pt.x_ - segP1_->x_) * (segP2_->y_ - segP1_->y_)) -
                 ((segP2_->x_ - segP1_->x_) * (pt.y_ - segP1_->y_ ));
    return (det > 0);
}
bool Segment::isOnLeftSide(const shared_ptr<Point> pt) const{
    /**The determinant of a pt to the segment is:
     *  det( p2 - p1, pt - p1)
     */
    float det  = ((pt->x_ - segP1_->x_) * (segP2_->y_ - segP1_->y_)) -
                 ((segP2_->x_ - segP1_->x_) * (pt->y_ - segP1_->y_ ));
    return (det > 0);
}

bool Segment::isOnLeftSide(const PointStruct& pt) const{
    float det  = ((pt.x - segP1_->x_) * (segP2_->y_ - segP1_->y_)) - ((segP2_->x_ - segP1_->x_) * (pt.y - segP1_->y_ ));
    return det > 0;
}
bool Segment::areOnOppositeSides(const shared_ptr<Point> pt1, const shared_ptr<Point> pt2) const{
	bool pt1IsLeft = isOnLeftSide(pt1);
	bool pt2IsLeft = isOnLeftSide(pt2);
	return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}
bool Segment::areOnOppositeSides(const PointStruct& pt1, const PointStruct& pt2) const{
    bool pt1IsLeft = isOnLeftSide(pt1);
	bool pt2IsLeft = isOnLeftSide(pt2);
	return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}
bool Segment::intersects(const Segment& seg) const{
    return areOnOppositeSides(seg.segP1_, seg.segP2_) && seg.areOnOppositeSides(segP1_, segP2_);;
}
bool Segment::intersects(const shared_ptr<Point> pt1, const shared_ptr<Point> pt2) const{
		Segment seg(pt1, pt2);
        return areOnOppositeSides(pt1, pt2) && seg.areOnOppositeSides(segP1_, segP2_);
}
bool Segment::intersects(const PointStruct& pt1, const PointStruct& pt2) const{
		SegmentStruct segStruct(pt1, pt2);
		PointStruct ps1{segP1_->x_, segP1_->y_};
		PointStruct ps2{segP2_->x_, segP2_->y_};
        return areOnOppositeSides(pt1, pt2) && segStruct.areOnOppositeSides(ps1, ps2);
}
shared_ptr<PointStruct> Segment::findIntersection(const Segment& interSeg){
    if(intersects(interSeg)){
		const float interSegDX = interSeg.segP2_->x_ - interSeg.segP1_->x_;
		const float currSegDX = segP2_->x_ - segP1_->x_;
		const float interSegDY = interSeg.segP2_->y_ - interSeg.segP1_->y_;
		const float currSegDY = segP2_->y_ - segP1_->y_;

		float denom = interSegDX*currSegDY - interSegDY*currSegDX;
		float num = interSegDY*(segP1_->x_ - interSeg.segP1_->x_) -
					interSegDX*(segP1_->y_ - interSeg.segP1_->y_);
		float alpha = num/denom;
		float interX = segP1_->x_ + alpha*currSegDX;
		float interY = segP1_->y_ + alpha*currSegDY;
        return make_shared<PointStruct>(interX, interY);

    }else{
        //	if no intersection found return a null pointer
        return nullptr;
    }
}



