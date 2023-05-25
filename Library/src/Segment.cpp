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
Segment::Segment(SegmentToken token, shared_ptr<Point> pt1, shared_ptr<Point> pt2)
    :
		p1_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt1 :  pt2),
		p2_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt2 :  pt1),
        idx_(count_++)
{
	(void) token;
	
	/**	add myself to segList of p1 and p2*/
//    p1_->segList_.insert(this->makeNewSegPtr(pt1, pt2));
//    p2_->segList_.insert(this->makeNewSegPtr(pt1, pt2));
//	p1_->segList_.insert(segVect_[idx_]);
//	p2_->segList_.insert(segVect_[idx_]);
}
Segment::Segment(shared_ptr<Point>  pt1, shared_ptr<Point>  pt2)
    :
		p1_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt1 :  pt2),
		p2_((pt1->getY() > pt2->getY()) || ((pt1->getY() == pt2->getY()) && (pt1->getX() < pt2->getX())) ? pt2 :  pt1),
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

shared_ptr<Segment> Segment::makeNewSegPtr(shared_ptr<Point> pt1, shared_ptr<Point> pt2){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet_.begin(); iter!=segSet_.end();iter++){
        /** segSet is a set of pointers to segments
         *  so we need to dereference the segment pointer  to check the point pairs
         */
        if( ((*iter)->p1_->idx_ == pt1->idx_ && (*iter)->p2_->idx_ == pt2->idx_) ||
			((*iter)->p2_->idx_ == pt1->idx_ && (*iter)->p1_->idx_ == pt2->idx_)){
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
	render_(*p1_, *p2_, type);
	p1_->render(PointType::ENDPOINT);
	p2_->render(PointType::ENDPOINT);
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
    float det  = ((pt.x_ - p1_->x_) * (p2_->y_ - p1_->y_)) -
                 ((p2_->x_ - p1_->x_) * (pt.y_ - p1_->y_ ));
    return (det > 0);
}
bool Segment::isOnLeftSide(const shared_ptr<Point> pt) const{
    /**The determinant of a pt to the segment is:
     *  det( p2 - p1, pt - p1)
     */
    float det  = ((pt->x_ - p1_->x_) * (p2_->y_ - p1_->y_)) -
                 ((p2_->x_ - p1_->x_) * (pt->y_ - p1_->y_ ));
    return (det > 0);
}

bool Segment::isOnLeftSide(const PointStruct& pt) const{
    float det  = ((pt.x - p1_->x_) * (p2_->y_ - p1_->y_)) - ((p2_->x_ - p1_->x_) * (pt.y - p1_->y_ ));
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
    return areOnOppositeSides(seg.p1_, seg.p2_) && seg.areOnOppositeSides(p1_, p2_);;
}
bool Segment::intersects(const shared_ptr<Point> pt1, const shared_ptr<Point> pt2) const{
		Segment seg(pt1, pt2);
        return areOnOppositeSides(pt1, pt2) && seg.areOnOppositeSides(p1_, p2_);
}
bool Segment::intersects(const PointStruct& pt1, const PointStruct& pt2) const{
		SegmentStruct segStruct(pt1, pt2);
		PointStruct ps1{p1_->x_, p1_->y_};
		PointStruct ps2{p2_->x_, p2_->y_};
        return areOnOppositeSides(pt1, pt2) && segStruct.areOnOppositeSides(ps1, ps2);
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



