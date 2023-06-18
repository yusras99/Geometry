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
 * @param endpt1Ptr point that will be used to create a segment
 * @param endpt2Ptr point that will be usedto create a segment
 * @param token - the object of segmentToken class (which acts like passkey attribute) so that it is not accessible by anyone else
 */
Segment::Segment(SegmentToken token, shared_ptr<Point> endpt1Ptr, shared_ptr<Point> endpt2Ptr,
				bool endpointsOrdered)
    :
		p1_(endpointsOrdered ? endpt1Ptr : (endpt1Ptr->getY() > endpt2Ptr->getY()) || ((endpt1Ptr->getY() == endpt2Ptr->getY()) && (endpt1Ptr->getX() < endpt2Ptr->getX())) ? endpt1Ptr :  endpt2Ptr),
		p2_(endpointsOrdered ? endpt2Ptr : (endpt1Ptr->getY() > endpt2Ptr->getY()) || ((endpt1Ptr->getY() == endpt2Ptr->getY()) && (endpt1Ptr->getX() < endpt2Ptr->getX())) ? endpt2Ptr :  endpt1Ptr),
        idx_(count_++)
{
	(void) token;
}
Segment::Segment(shared_ptr<Point>  endpt1Ptr, shared_ptr<Point>  endpt2Ptr)
    :
		p1_((endpt1Ptr->getY() > endpt2Ptr->getY()) || ((endpt1Ptr->getY() == endpt2Ptr->getY()) && (endpt1Ptr->getX() < endpt2Ptr->getX())) ? endpt1Ptr :  endpt2Ptr),
		p2_((endpt1Ptr->getY() > endpt2Ptr->getY()) || ((endpt1Ptr->getY() == endpt2Ptr->getY()) && (endpt1Ptr->getX() < endpt2Ptr->getX())) ? endpt2Ptr :  endpt1Ptr),
        idx_(UINT_MAX)
{
}

/**Destructor function that clears the segment */
Segment::~Segment(void){
	/**	remove myself from segList of p1 and p2*/
	p1_->segList_.erase(segVect_[idx_]);
	p2_->segList_.erase(segVect_[idx_]);
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif

shared_ptr<Segment> Segment::makeNewSegPtr(shared_ptr<Point> endpt1Ptr, shared_ptr<Point> endpt2Ptr){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet_.begin(); iter!=segSet_.end();iter++){
        /** segSet is a set of pointers to segments
         *  so we need to dereference the segment pointer  to check the point pairs
         */
        if( ((*iter)->p1_->idx_ == endpt1Ptr->idx_ && (*iter)->p2_->idx_ == endpt2Ptr->idx_) ||
			((*iter)->p2_->idx_ == endpt1Ptr->idx_ && (*iter)->p1_->idx_ == endpt2Ptr->idx_)){
            found = true;
            /**return pointer to the segment*/
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }else{
        shared_ptr<Segment> currSeg = make_shared<Segment>(SegmentToken{}, endpt1Ptr, endpt2Ptr);
        segSet_.insert(currSeg);
        segVect_.push_back(currSeg);
		endpt1Ptr->segList_.insert(currSeg);
		endpt2Ptr->segList_.insert(currSeg);
        return currSeg;
    }
}

Segment& Segment::makeNewSeg(shared_ptr<Point> endpt1Ptr, shared_ptr<Point> endpt2Ptr){
   return *(makeNewSegPtr(endpt1Ptr, endpt2Ptr));
}

Segment Segment::makeNewTempSeg(shared_ptr<Point> endpt1Ptr, shared_ptr<Point> endpt2Ptr){
    return Segment(endpt1Ptr, endpt2Ptr);
}


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Rendering functions
//-----------------------------------------------------------------
#endif

void Segment::render(SegmentType type) const{
	/**	first draw the segment, then draw its endpoints*/
	render_(*p1_, *p2_, type);
	p1_->render(PointType::ENDPOINT);
	p2_->render(PointType::ENDPOINT);
}

void Segment::render_(const Point& endpt1, const Point& endpt2, SegmentType type){
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
		glVertex2f(endpt1.getX(), endpt1.getY());
		glVertex2f(endpt2.getX(), endpt2.getY());
	glEnd();
}

void Segment::renderCreated(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2){
	glColor4fv(SEGMENT_COLOR[1]);
	
	glBegin(GL_LINES);
		glVertex2f(potentialEndpt1.x, potentialEndpt1.y);
		glVertex2f(potentialEndpt2.x, potentialEndpt2.y);
	glEnd();

	Point::render(potentialEndpt1, PointType::FIRST_ENDPOINT);
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

bool Segment::isOnLeftSide(const Point& referencePt) const{
    /**The determinant of a pt to the segment is:
     *  det( p2 - p1, pt - p1)
     */
    float determinant  = ((referencePt.x_ - p1_->x_) * (p2_->y_ - p1_->y_)) -
                 ((p2_->x_ - p1_->x_) * (referencePt.y_ - p1_->y_ ));
    return (determinant > 0);
}
bool Segment::isOnLeftSide(const shared_ptr<Point> referencePtPtr) const{
    /**The determinant of a pt to the segment is:
     *  det( p2 - p1, pt - p1)
     */
    float determinant  = ((referencePtPtr->x_ - p1_->x_) * (p2_->y_ - p1_->y_)) -
                 ((p2_->x_ - p1_->x_) * (referencePtPtr->y_ - p1_->y_ ));
    return (determinant > 0);
}

bool Segment::isOnLeftSide(const PointStruct& potentialPt) const{
    float det  = ((potentialPt.x - p1_->x_) * (p2_->y_ - p1_->y_)) - ((p2_->x_ - p1_->x_) * (potentialPt.y - p1_->y_ ));
    return det > 0;
}
bool Segment::areOnOppositeSides(const shared_ptr<Point> endpt1Ptr, const shared_ptr<Point> endpt2Ptr) const{
	bool pt1IsLeft = isOnLeftSide(endpt1Ptr);
	bool pt2IsLeft = isOnLeftSide(endpt2Ptr);
	return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}
bool Segment::areOnOppositeSides(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2) const{
    bool pt1IsLeft = isOnLeftSide(potentialEndpt1);
	bool pt2IsLeft = isOnLeftSide(potentialEndpt2);
	return ((pt1IsLeft && !pt2IsLeft) || (!pt1IsLeft && pt2IsLeft));
}
bool Segment::intersects(const Segment& intersectingSeg) const{
    return areOnOppositeSides(intersectingSeg.p1_, intersectingSeg.p2_) && intersectingSeg.areOnOppositeSides(p1_, p2_);;
}
bool Segment::intersects(const shared_ptr<Point> endpt1Ptr, const shared_ptr<Point> endpt2Ptr) const{
		Segment seg(endpt1Ptr, endpt2Ptr);
        return areOnOppositeSides(endpt1Ptr, endpt2Ptr) && seg.areOnOppositeSides(p1_, p2_);
}
bool Segment::intersects(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2) const{
		SegmentStruct segStruct(potentialEndpt1, potentialEndpt2);
		PointStruct ps1{p1_->x_, p1_->y_};
		PointStruct ps2{p2_->x_, p2_->y_};
        return areOnOppositeSides(potentialEndpt1, potentialEndpt2) && segStruct.areOnOppositeSides(ps1, ps2);
}
shared_ptr<PointStruct> Segment::findIntersection(const Segment& interSeg){
    if(intersects(interSeg)){
		const float interSegDX = interSeg.p2_->x_ - interSeg.p1_->x_;
		const float currSegDX = p2_->x_ - p1_->x_;
		const float interSegDY = interSeg.p2_->y_ - interSeg.p1_->y_;
		const float currSegDY = p2_->y_ - p1_->y_;

		float denom = interSegDX*currSegDY - interSegDY*currSegDX;
		float num = interSegDY*(p1_->x_ - interSeg.p1_->x_) -
					interSegDX*(p1_->y_ - interSeg.p1_->y_);
		float alpha = num/denom;
		float interX = p1_->x_ + alpha*currSegDX;
		float interY = p1_->y_ + alpha*currSegDY;
        return make_shared<PointStruct>(interX, interY);

    }else{
        //	if no intersection found return a null pointer
        return nullptr;
    }
}



