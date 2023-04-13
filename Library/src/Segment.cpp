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

#include "Point.hpp"
#include "Segment.hpp"


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
Segment::Segment(SegmentToken token, Point& pt1, Point& pt2)
    :
		p1_((pt1.y_ < pt2.y_) || ((pt1.y_ < pt2.y_) && (pt1.x_ < pt2.x_)) ? pt1 :  pt2),
		p2_((pt1.y_ < pt2.y_) || ((pt1.y_ < pt2.y_) && (pt1.x_ < pt2.x_)) ? pt2 :  pt1),
        idx_(count_++)
{
	(void) token;
	
	/**	add myself to segList of p1 and p2*/
	p1_.segList_.insert(idx_);
	p2_.segList_.insert(idx_);
}
Segment::Segment(Point& pt1, Point& pt2)
    :
		p1_((pt1.y_ < pt2.y_) || ((pt1.y_ < pt2.y_) && (pt1.x_ < pt2.x_)) ? pt1 :  pt2),
		p2_((pt1.y_ < pt2.y_) || ((pt1.y_ < pt2.y_) && (pt1.x_ < pt2.x_)) ? pt2 :  pt1),
        idx_(UINT_MAX)
{
}

/**Destructor function that clears the segment */
Segment::~Segment(void){
	/**	remove myself from segList of p1 and p2*/
	p1_.segList_.erase(idx_);
	p2_.segList_.erase(idx_);
}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif

shared_ptr<Segment> Segment::makeNewSegPtr(Point& pt1, Point& pt2){
    bool found = false;
    shared_ptr<Segment> p;
    for(auto iter = segSet_.begin(); iter!=segSet_.end();iter++){
        /** segSet is a set of pointers to segments
         *  so we need to dereference the segment pointer  to check the point pairs
         */
        if( ((*iter)->p1_.idx_ == pt1.idx_ && (*iter)->p2_.idx_ == pt2.idx_) ||
			((*iter)->p2_.idx_ == pt1.idx_ && (*iter)->p1_.idx_ == pt2.idx_)){
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
		pt1.segList_.insert(currSeg->idx_);
		pt2.segList_.insert(currSeg->idx_);
        return currSeg;
    }
}

Segment& Segment::makeNewSeg(Point& pt1, Point& pt2){
   return *(makeNewSegPtr(pt1, pt2));
}

Segment Segment::makeNewTempSeg(Point& pt1, Point& pt2){
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


#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Intersection-related functions
//-----------------------------------------------------------------
#endif

/**Function that checks if the point is on left of the segments or not
 *@param pt - a reference to a constant point which has to be checked on its direction to the segment
 *@return boolean that tells if the point is on left or not
 */
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
//
//    PointStruct pts = {pt.getX(), pt.getY()};
//    return isOnLeftSide(pts);
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
bool Segment::intersects(const Segment& seg) const{
    return areOnOppositeSides(seg.p1_, seg.p2_) && seg.areOnOppositeSides(p1_, p2_);;
}
bool Segment::intersects(const Point& pt1, const Point& pt2) const{
		Segment seg(const_cast<Point&>(pt1), const_cast<Point&>(pt2));
        return areOnOppositeSides(pt1, pt2) && seg.areOnOppositeSides(p1_, p2_);
}
bool Segment::intersects(const PointStruct& pt1, const PointStruct& pt2) const{
		SegmentStruct segStruct(pt1, pt2);
		PointStruct ps1{p1_.x_, p1_.y_};
		PointStruct ps2{p2_.x_, p2_.y_};
        return areOnOppositeSides(pt1, pt2) && segStruct.areOnOppositeSides(ps1, ps2);
}
unique_ptr<PointStruct> Segment::findIntersection(const Segment& interSeg){
    if(intersects(interSeg)){
		const float interSegDX = interSeg.p2_.x_ - interSeg.p1_.x_;
		const float currSegDX = p2_.x_ - p1_.x_;
		const float interSegDY = interSeg.p2_.y_ - interSeg.p1_.y_;
		const float currSegDY = p2_.y_ - p1_.y_;

		float denom = interSegDX*currSegDY - interSegDY*currSegDX;
		float num = interSegDY*(p1_.x_ - interSeg.p1_.x_) -
					interSegDX*(p1_.y_ - interSeg.p1_.y_);
		float alpha = num/denom;
		float interX = p1_.x_ + alpha*currSegDX;
		float interY = p1_.y_ + alpha*currSegDY;
        return make_unique<PointStruct>(interX, interY);

    }else{
        //	if no intersection found return a null pointer
        return nullptr;
    }
}
vector<unique_ptr<PointStruct> > geometry::findAllIntersectionsBruteForce(const vector<shared_ptr<Segment> >& vect){
	
	vector<unique_ptr<PointStruct> > intersectVect;
	for (size_t i=0; i<vect.size(); i++){
		for (size_t j=i+1; j<vect.size(); j++){
			unique_ptr<PointStruct> pt = vect[i]->findIntersection(*(vect[j]));
			if (pt != nullptr){
				intersectVect.push_back(std::move(pt));
			}
		}
	}
	return intersectVect;
}
#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark Smart Intersection-related functions
//-----------------------------------------------------------------
#endif
//vector<shared_ptr<Point> > geometry::getAllEndPoints(const vector<shared_ptr<Segment> >& vect){
//    vector<shared_ptr<Point> > endpointVect;
//    //   Get all points and see if they have a segList, if they do put them in the vector.
//    static const std::set<std::shared_ptr<Point> > allPointSet = Point::getAllPoints();
//    for (auto itr = allPointSet.begin(); itr != allPointSet.end(); itr++){
//        std::shared_ptr<Point> currPoint = *itr;
//        if(currPoint->getConnectivityDegree() != 0){
//            endpointVect.push_back(currPoint);
//        }
//    }
//    return endpointVect;
//}

std::set<std::shared_ptr<InterQueueEvent> , compare> geometry::buildEventSet(const std::vector<std::shared_ptr<Segment> >& vect){
//    vector<shared_ptr<Point> > endpointVect = getAllEndPoints(vect);
    std::set<std::shared_ptr<InterQueueEvent> , compare> eventQueue;
        for (auto itr = vect.begin(); itr != vect.end(); itr++){
            const std::shared_ptr<InterQueueEvent> endPt1 = make_shared<InterQueueEvent>();
            const std::shared_ptr<InterQueueEvent> endPt2 = make_shared<InterQueueEvent>();
//            static const std::set<std::shared_ptr<Point> >& allPointSet = Point::getAllPoints();
//            std::shared_ptr<Point> currPoint = allPointSet.find(&(*itr)->getP1());
            endPt1->endpt = (*itr)->getP1();
            endPt1->isIntersection = false;
            endPt1->isFirst = true;
            endPt1->segIdx = (*itr)->getIndex();
            eventQueue.insert(endPt1);
            
            endPt2->endpt = (*itr)->getP2();
            endPt2->isIntersection = false;
            endPt2->isFirst = false;
            endPt2->segIdx = (*itr)->getIndex();
            eventQueue.insert(endPt2);
        }
    return eventQueue;
}

void geometry::addEvent(set<shared_ptr<InterQueueEvent> , compare>& eventQueue,shared_ptr<PointStruct> currInterPt){
    const std::shared_ptr<InterQueueEvent> currPoint;
    currPoint->interPt = currInterPt;
    currPoint->isIntersection = true;
    eventQueue.insert(currPoint);
}
void geometry::orderMap(std::map<int,unsigned int> &prioritySegMap,const vector<shared_ptr<Segment> >& segVect,shared_ptr<InterQueueEvent> currPoint){
    
//  go through all the segments and see which if they are on the left or right of the segment that they are supposed to check
    for(auto itr = prioritySegMap.begin(); itr != prioritySegMap.end(); itr++){
        unsigned int currSeg = (itr)->second;
    }
}
//        auto itr2 = std::next(itr,1);
//        if(segVect[itr2].isOnLeftSide(currPoint->endpt)){
////                swap priorities
//                itr++
//        }else{
//
//        }
vector<unique_ptr<PointStruct> > geometry::findAllIntersectionsSmart(const vector<shared_ptr<Segment> >& segVect){
    
    vector<unique_ptr<PointStruct> > intersectVect;
    //    Call this function to populate the segmentPoints
    std::set<std::shared_ptr<InterQueueEvent> , compare> eventQueue = buildEventSet(segVect);
	//   The T data structure is an ordered map with key being segment priority number, and value is the segment number
	std::map<int,unsigned int> prioritySegMap;
	int segPriority = 0;
	for(auto itr = eventQueue.begin(); itr != eventQueue.end(); itr++){
		std::shared_ptr<InterQueueEvent> currPoint = *itr;
        std::set<unsigned int> currSegSet;
		//Get the segmentlist of each point and push it in the priority seg map according to the order
		if(!(*currPoint).isIntersection){
			currSegSet = (currPoint->endpt)->getSegList();
		}
        //if it is the first endpt, put it in the map
        if(currPoint->isFirst){
            //populate segMap
            //if segList size is larger than one then execute this loop to add them all
            if(currSegSet.size()>1){
                for(auto segItr = currSegSet.begin(); segItr != currSegSet.end(); segItr++){
                    prioritySegMap[segPriority] = *(segItr);
                    segPriority++;
                }
            }else{
                prioritySegMap[segPriority] = *(currSegSet.begin());
            }
            
            //if only one segment in map then continue
            if(prioritySegMap.size() == 1){
                continue;
            }else{
                // if there are more than one segments in the map then sort them
                orderMap(prioritySegMap,segVect,currPoint);
            }
        }else{
            //remove the segment from the map, if we are on the second endpt
            for (auto mapItr= prioritySegMap.begin(); mapItr!=prioritySegMap.end(); mapItr++){
                if(mapItr->second == currPoint->segIdx){
                    prioritySegMap.erase(mapItr->first);
                }
            }
        }
        segPriority++;
	}
    return intersectVect;
}
