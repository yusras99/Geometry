//
//  IntersectionQuery.cpp
//  Point Input - source
//
//  Created by Yusra Suhail on 5/4/23.
//  Copyright © 2023 URI 3D Group. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <sstream>
#include "Segment.hpp"
#include "Point.hpp"
#include "Geometry.hpp"
#include "IntersectionQuery.hpp"

using namespace std;
using namespace geometry;

#define DEBUG_OUT	1

IntersectionQuery::IntersectionQuery(const vector<shared_ptr<Segment> >& vect)
	:	potentialInterSegVect_(vect){
	
}

vector<shared_ptr<PointStruct> > IntersectionQuery::findAllIntersectionsBruteForce(){
    
    vector<shared_ptr<PointStruct> > intersectVect;
    for (size_t i=0; i<potentialInterSegVect_.size(); i++){
        for (size_t j=i+1; j<potentialInterSegVect_.size(); j++){
            shared_ptr<PointStruct> pt = potentialInterSegVect_[i]->findIntersection(*(potentialInterSegVect_[j]));
            if (pt != nullptr){
                intersectVect.push_back(pt);
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
set<shared_ptr<InterQueueEvent>, compareEvent> IntersectionQuery::buildEventSet(){
    //check the potentialInterSegVect_ and grab endpoints of each segment in the vector and add it in the queue, change the type of these points to be an InterQueueEvent.
    set<shared_ptr<InterQueueEvent>, compareEvent> eventQueue;
        for (auto itr = potentialInterSegVect_.begin(); itr != potentialInterSegVect_.end(); itr++){
            
            const std::shared_ptr<InterQueueEvent> endPt1 = make_shared<InterQueueEvent>();
            const std::shared_ptr<InterQueueEvent> endPt2 = make_shared<InterQueueEvent>();
            
            endPt1->endptPtr = (*itr)->getP1();
            endPt1->isIntersection = false;
            eventQueue.insert(endPt1);
            
            endPt2->endptPtr = (*itr)->getP2();
            endPt2->isIntersection = false;
            eventQueue.insert(endPt2);
        }
    return eventQueue;
}

void IntersectionQuery::addEvent(set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,shared_ptr<IntersectionPointStruct> currInterPt){
    //for each intersection point found, add it in the eventQueue
    const std::shared_ptr<InterQueueEvent> currEventPt = make_shared<InterQueueEvent>();
    currEventPt->interPtPtr = currInterPt;
    currEventPt->isIntersection = true;
    eventQueue.insert(currEventPt);
}
void IntersectionQuery::checkSegIntersection(shared_ptr<Segment> currSegOfEvent,shared_ptr<Segment> compareSeg,vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >& comparedSegForInterVec,set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,  vector<shared_ptr<PointStruct> >& smartIntersectVect,set< shared_ptr<Segment>, compareSegment>& prioritySegSet){
    
    auto checkSegPair = currSegOfEvent->getIndex() < compareSeg->getIndex() ?
							make_pair(currSegOfEvent,compareSeg) : make_pair(compareSeg,currSegOfEvent);
    //call vector's iterator to check if the currSegOfEvent and prevSeg are already compared or not
    auto findComparedSegPairItr = std::find(comparedSegForInterVec.begin(), comparedSegForInterVec.end(), checkSegPair );
	//if the segs are not compared before, find intersection and put the point in the vector and in the eventQueue
    if (findComparedSegPairItr == comparedSegForInterVec.end()){
        auto potentialInterPtPtr = compareSeg->findIntersection(*currSegOfEvent);
        if (potentialInterPtPtr != nullptr){
            shared_ptr<IntersectionPointStruct> smartInterPtPtr = make_shared<IntersectionPointStruct>(
                                potentialInterPtPtr,
                                checkSegPair);
            smartIntersectVect.push_back(smartInterPtPtr);
            addEvent(eventQueue, smartInterPtPtr);
            #if DEBUG_OUT
                cout << "After adding intersection between S_" << compareSeg->getIndex() << " and S_" << currSegOfEvent->getIndex() << endl;
                cout << "Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
            #endif
        }
    }
    //	In any case, push the segment pair on the "checked" list
	comparedSegForInterVec.push_back(checkSegPair);
}
void IntersectionQuery::postInsertSegCheck(set< shared_ptr<Segment>, compareSegment>& prioritySegSet,
            set< shared_ptr<Segment>, compareSegment>::iterator currSegOfEventItr,
            vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >& comparedSegForInterVec,
			set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue, vector<shared_ptr<PointStruct> > &smartIntersectVect){
    
	auto currSegOfEvent = *currSegOfEventItr;
	if(prioritySegSet.size() > 1){
		bool isLastSeg = next(currSegOfEventItr) == prioritySegSet.end();
		bool isFirstSeg = currSegOfEventItr == prioritySegSet.begin();
		if(!isFirstSeg){//unless the current segment is first, compare the previous one, since currSegOfEvent is on right make it second argument
			auto compareSeg = *(prev(currSegOfEventItr));
			checkSegIntersection(compareSeg, currSegOfEvent,comparedSegForInterVec,eventQueue,smartIntersectVect,prioritySegSet);
		}
		if (!isLastSeg){//unless the current segment is last, compare to the next one, since currSegOfEvent is on left make it first argument
			auto compareSeg = *(next(currSegOfEventItr));
			checkSegIntersection(currSegOfEvent,compareSeg,comparedSegForInterVec,eventQueue,smartIntersectVect,prioritySegSet);
		}
	}
}
void IntersectionQuery::removeSeg(set< shared_ptr<Segment>, compareSegment>&prioritySegSet,
                                  set< shared_ptr<Segment>, compareSegment>::iterator currSegOfEventItr,
                                  vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >&comparedSegForInterVec,
                                  set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,
                                  vector<shared_ptr<PointStruct> >& smartIntersectVect){
    
	auto currSegOfEvent = *currSegOfEventItr;
    if(prioritySegSet.size() > 1){
        bool isLastSeg = next(currSegOfEventItr) == prioritySegSet.end();
        bool isFirstSeg = currSegOfEventItr == prioritySegSet.begin();
        
        //Initialize the compare segment
        auto compareSeg = *(currSegOfEventItr);
        //if the current segment has neighbors on both left and right side then compare the neighbors for intersection
        if(!isLastSeg && !isFirstSeg){
            compareSeg = *(prev(currSegOfEventItr));
            currSegOfEvent = *(next(currSegOfEventItr));
            checkSegIntersection(compareSeg,currSegOfEvent,comparedSegForInterVec,eventQueue,smartIntersectVect,prioritySegSet);
        }
    }
    prioritySegSet.erase(currSegOfEventItr);
}
vector<shared_ptr<PointStruct> > IntersectionQuery::findAllIntersectionsSmart(){
    
    vector<shared_ptr<PointStruct> > smartIntersectVect;
    //    Call this function to populate the segmentPoints
    set<shared_ptr<InterQueueEvent> , compareEvent> eventQueue = buildEventSet();
    //   The T data structure is a set of shared pointers to segments and compare function that will sort the segments
    set< shared_ptr<Segment>, compareSegment> prioritySegSet;
    //The vector of segment pairs which have been swapped
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > > swappedSegmentVec;
    //The vector segment pairs which have been checked for intersection
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > > comparedSegForInterVec;
  
	#if DEBUG_OUT
		cout << "Initial Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
	#endif
	
    for(auto eventQueueItr = eventQueue.begin(); eventQueueItr != eventQueue.end(); eventQueueItr++){
        shared_ptr<InterQueueEvent> currEventPt = *eventQueueItr;
        
        if(!currEventPt->isIntersection){
            //Get the segmentlist of each point and push it in the priority seg set according to the order
            set<shared_ptr<Segment>> eventPtSegList = (currEventPt->endptPtr)->getSegList();
            //scanY is the current y value of the point of the eventQueue.
            scanY = (currEventPt->endptPtr)->getY();
            //Execute this loop to add/remove each of the segments one by one in the segSet
            for(auto currSegOfEvent : eventPtSegList){
				//this the iterator of priority seg set which we will be using for checking intersections
				decltype(prioritySegSet)::iterator currSegOfEventItr;
				//If currEventPt is the first point of the segment push it on the prioritySegSet else remove it
                if(currEventPt->endptPtr == (currSegOfEvent)->getP1()){
                    auto insertSegItr = prioritySegSet.insert(currSegOfEvent);
                    currSegOfEventItr = insertSegItr.first;
  
                    //	If inserted iterator is not begin, check with prev neighbor
                    //	If inserted iterator is not last, check with next neighbor
                    postInsertSegCheck(prioritySegSet,currSegOfEventItr, comparedSegForInterVec, eventQueue,smartIntersectVect);
                }
                else{//If it is the second point of the segment, then find the segment to remove
//                    currSegOfEventItr = prioritySegSet.find(currSegOfEvent);
                    currSegOfEventItr = prioritySegSet.end();
                    for (auto prioritySegItr = prioritySegSet.begin(); prioritySegItr != prioritySegSet.end(); prioritySegItr++){
						if (*prioritySegItr == currSegOfEvent) {
							currSegOfEventItr = prioritySegItr;
							break;
						}
					}
                    if (currSegOfEventItr == prioritySegSet.end()) {
						cout << "should not happen " << endl;
						exit(10);
					}
                    removeSeg(prioritySegSet, currSegOfEventItr, comparedSegForInterVec, eventQueue,smartIntersectVect);
                }
            }
			#if DEBUG_OUT
				cout << "After processing endpoint P_" << currEventPt->endptPtr->getIndex() << endl;
				cout << "T list: " << getTListContent(prioritySegSet,comparedSegForInterVec, swappedSegmentVec) << endl;
			#endif
             
        }else{ //if currEventPt is an intersection point then execute this code snippet
            //Get the segmentlist of each point and push it on the priority seg set according to the order
            pair<shared_ptr<Segment>,shared_ptr<Segment> > interPtSegPair = (currEventPt->interPtPtr)->segPair;

			auto currSegOfEvent = interPtSegPair.first;  //this is current seg
			auto nextSeg = interPtSegPair.second;   //and this is next
			auto checkSegPair = currSegOfEvent->getIndex() < nextSeg->getIndex() ?
									make_pair(currSegOfEvent,nextSeg) : make_pair(nextSeg,currSegOfEvent);
			//check if these 2 segments are swapped before or not
			auto findSwappedSegPairItr = std::find(swappedSegmentVec.begin(),swappedSegmentVec.end(),checkSegPair );
			if (findSwappedSegPairItr == swappedSegmentVec.end()){
				//swap them
				//take them out and put it back with respect to new y coordinate
				prioritySegSet.erase(currSegOfEvent);
				prioritySegSet.erase(nextSeg);
				scanY = (currEventPt->interPtPtr)->y;
				//reinsertion of the seg that was the next(on right side of current) and check it for intersection with the seg on left of it
				auto reInsertSegItr = prioritySegSet.insert(nextSeg).first;
				if (reInsertSegItr != prioritySegSet.begin()){
                    checkSegIntersection(*(prev(reInsertSegItr)), *reInsertSegItr,comparedSegForInterVec,eventQueue,smartIntersectVect,prioritySegSet);
				}
                //reinsertion of the seg that was current(left of next seg) and check it for intersection with the seg on right of it
				reInsertSegItr = prioritySegSet.insert(currSegOfEvent).first;
				if (next(reInsertSegItr) != prioritySegSet.end()){
                    checkSegIntersection(*reInsertSegItr, *(next(reInsertSegItr)),comparedSegForInterVec,eventQueue,smartIntersectVect,prioritySegSet);
				}
				
				swappedSegmentVec.push_back(checkSegPair);
				
				#if DEBUG_OUT
					cout << "After processing intersection between S_" << currSegOfEvent->getIndex() << " and S_" << nextSeg->getIndex() << endl;
					cout << "Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
					cout << "T list: " << getTListContent(prioritySegSet,comparedSegForInterVec, swappedSegmentVec) << endl;
				#endif
			}
			//	should eventually be removed
			else{
				cout << "segments should't have already been swapped" << endl;
				exit(12);
			}
        }
    }
    return smartIntersectVect;
}


string IntersectionQuery::getEventQueueContent(const set<shared_ptr<InterQueueEvent>, compareEvent >& eventQueue,
												bool detailed) const{
	
	// not used yet
	(void) detailed;
	
	ostringstream outStream;
    for(auto itr = eventQueue.begin(); itr != eventQueue.end(); itr++){
        shared_ptr<InterQueueEvent> currEventPt = *itr;
        
        if(currEventPt->isIntersection){
			shared_ptr<IntersectionPointStruct> pt = currEventPt->interPtPtr;
			outStream << "\tInt " << "(" << pt->x << ", " << pt->y << "), ";
			outStream << "intersection of Segments ";
			outStream << "S_" << pt->segPair.first->getIndex() << " and " <<
						 "S_" << pt->segPair.second->getIndex() << endl;
		}
		else{
			shared_ptr<Point> pt = currEventPt->endptPtr;
			set<std::shared_ptr<Segment> > segList = pt->getSegList();
			outStream << "\tP_" << pt->getIndex() <<  " (" << pt->getX() << ", " << pt->getY() << "), " <<
						 "endpoint of Segment";
			if (segList.size() > 1){
				outStream << "s";
			}
			for (auto segPtr : segList){
				outStream << " S_" << segPtr->getIndex();
				if (pt->getIndex() == segPtr->getP1()->getIndex()){
					outStream << " (start) ";
				}
				else{
					outStream << " (end) ";
				}
				outStream << endl;
			}		
		}
	}
	
	return outStream.str();
}
			
string IntersectionQuery::getTListContent(const set<shared_ptr<Segment>, compareSegment>& prioritySegSet,
										const vector<pair<shared_ptr<Segment>, shared_ptr<Segment> > >& comparedSegForInterVec,
										const vector<pair<shared_ptr<Segment>, shared_ptr<Segment> > >& swappedSegmentVec) const{
	ostringstream outStream;
	
	for(auto seg : prioritySegSet){
		outStream << "S_" << seg->getIndex() << " [P_" << seg->getP1()->getIndex() << ", " <<
					 "P_" << seg->getP2()->getIndex() << "] ";
	}
	outStream << endl << "Segment pair tested: ";
	for (const auto& segPair : comparedSegForInterVec){
		outStream << "(S_" << segPair.first->getIndex() << ", " << "S_" << segPair.second->getIndex() << ") ";
	}
	outStream << endl << "Segment pair swapped: ";
	for (const auto& segPair : swappedSegmentVec){
		outStream << "(S_" << segPair.first->getIndex() << ", " << "S_" << segPair.second->getIndex() << ") ";
	}
	
	return outStream.str();

}
