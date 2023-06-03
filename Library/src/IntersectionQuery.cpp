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
	:	vect_(vect){
	
}

vector<shared_ptr<PointStruct> > IntersectionQuery::findAllIntersectionsBruteForce(){
    
    vector<shared_ptr<PointStruct> > intersectVect;
    for (size_t i=0; i<vect_.size(); i++){
        for (size_t j=i+1; j<vect_.size(); j++){
            shared_ptr<PointStruct> pt = vect_[i]->findIntersection(*(vect_[j]));
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
    set<shared_ptr<InterQueueEvent>, compareEvent> eventQueue;
        for (auto itr = vect_.begin(); itr != vect_.end(); itr++){
            
            const std::shared_ptr<InterQueueEvent> endPt1 = make_shared<InterQueueEvent>();
            const std::shared_ptr<InterQueueEvent> endPt2 = make_shared<InterQueueEvent>();
            
            endPt1->endpt = (*itr)->getP1();
            endPt1->isIntersection = false;
            eventQueue.insert(endPt1);
            
            endPt2->endpt = (*itr)->getP2();
            endPt2->isIntersection = false;
            eventQueue.insert(endPt2);
        }
    return eventQueue;
}

void IntersectionQuery::addEvent(set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,shared_ptr<IntersectionPointStruct> currInterPt){
    const std::shared_ptr<InterQueueEvent> currPoint = make_shared<InterQueueEvent>();
    currPoint->interPt = currInterPt;
    currPoint->isIntersection = true;
    eventQueue.insert(currPoint);
}
void IntersectionQuery::compareSegments(shared_ptr<Segment> currentSeg,shared_ptr<Segment> compareSeg,vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >& segIntersectionCheckedVec,set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,  vector<shared_ptr<PointStruct> >& smartIntersectVect,set< shared_ptr<Segment>, compareSegment>& prioritySegSet){
    auto checkSegPair = currentSeg->getIndex() < compareSeg->getIndex() ?
							make_pair(currentSeg,compareSeg) : make_pair(compareSeg,currentSeg);
    //call vector's iterator to check if the currentSeg and prevSeg are already compared or not
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >::iterator findPairItr = std::find(segIntersectionCheckedVec.begin(), segIntersectionCheckedVec.end(), checkSegPair );
	//if the segs are not compared before, find intersection and put the point in the vector and in the eventQueue
    if (findPairItr == segIntersectionCheckedVec.end()){
        auto potentialIntersection = compareSeg->findIntersection(*currentSeg);
        if (potentialIntersection != nullptr){
            shared_ptr<IntersectionPointStruct> smartIntersectionPt = make_shared<IntersectionPointStruct>(
                                potentialIntersection,
                                checkSegPair);
            smartIntersectVect.push_back(smartIntersectionPt);
            addEvent(eventQueue, smartIntersectionPt);
            #if DEBUG_OUT
                cout << "After adding intersection between S_" << compareSeg->getIndex() << " and S_" << currentSeg->getIndex() << endl;
                cout << "Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
                //cout << "T list: " << getTListContent(prioritySegSet,segIntersectionCheckedVec, swappedSegmentVec) << endl;
            #endif
        }
    }
    //	In any case, push the segment pair on the "checked" list
	segIntersectionCheckedVec.push_back(checkSegPair);
}
void IntersectionQuery::postInsertSegCheck(set< shared_ptr<Segment>, compareSegment>& prioritySegSet,
            set< shared_ptr<Segment>, compareSegment>::iterator iteratorToCurrentSeg,
            vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >& segIntersectionCheckedVec,
			set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue, vector<shared_ptr<PointStruct> > &smartIntersectVect){
	auto currentSeg = *iteratorToCurrentSeg;
	if(prioritySegSet.size() > 1){
		bool isLastSeg = next(iteratorToCurrentSeg) == prioritySegSet.end();
		bool isFirstSeg = iteratorToCurrentSeg == prioritySegSet.begin();
		if(!isFirstSeg){//unless the current segment is first, compare the previous one
			auto compareSeg = *(prev(iteratorToCurrentSeg));
			compareSegments(compareSeg, currentSeg,segIntersectionCheckedVec,eventQueue,smartIntersectVect,prioritySegSet);
		}
		if (!isLastSeg){//unless the current segment is last, compare to the next one
			auto compareSeg = *(next(iteratorToCurrentSeg));
			compareSegments(currentSeg,compareSeg,segIntersectionCheckedVec,eventQueue,smartIntersectVect,prioritySegSet);
		}
	}
}
void IntersectionQuery::removeSeg(set< shared_ptr<Segment>, compareSegment>&prioritySegSet,
                                  set< shared_ptr<Segment>, compareSegment>::iterator iteratorToCurrentSeg,
                                  vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >&segIntersectionCheckedVec,
                                  set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,
                                  vector<shared_ptr<PointStruct> >& smartIntersectVect){
    
	auto currentSeg = *iteratorToCurrentSeg;
    if(prioritySegSet.size() > 1){
        bool isLastSeg = next(iteratorToCurrentSeg) == prioritySegSet.end();
        bool isFirstSeg = iteratorToCurrentSeg == prioritySegSet.begin();
        
        //Initialize the compare segment
        auto compareSeg = *(iteratorToCurrentSeg);
        if(!isLastSeg && !isFirstSeg){
            //if the current segment is last only compare the previous one
            compareSeg = *(prev(iteratorToCurrentSeg));
            currentSeg = *(next(iteratorToCurrentSeg));
            compareSegments(compareSeg,currentSeg,segIntersectionCheckedVec,eventQueue,smartIntersectVect,prioritySegSet);
        }
    }
    prioritySegSet.erase(iteratorToCurrentSeg);
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
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > > segIntersectionCheckedVec;
  
	#if DEBUG_OUT
		cout << "Initial Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
	#endif
	
    for(auto itr = eventQueue.begin(); itr != eventQueue.end(); itr++){
        shared_ptr<InterQueueEvent> currPoint = *itr;
        
        if(!currPoint->isIntersection){
            //Get the segmentlist of each point and push it in the priority seg set according to the order
            set<shared_ptr<Segment>> currPointSegList = (currPoint->endpt)->getSegList();
            //scanY is the current y value of the point of the eventQueue.
            scanY = (currPoint->endpt)->getY();
            //Execute this loop to add/remove each of the segments one by one in the segSet
            for(auto currSegPtr : currPointSegList){
				//this the iterator of priority seg set which we will be using for checking intersections
				decltype(prioritySegSet)::iterator iteratorToCurrentSeg;
				//If currPoint is the first point of the segment push it on the prioritySegSet else remove it
                if(currPoint->endpt == (currSegPtr)->getP1()){
                    auto insertItr = prioritySegSet.insert(currSegPtr);
                    iteratorToCurrentSeg = insertItr.first;
  
                    //	If inserted iterator is not begin, check with prev neighbor
                    //	If inserted iterator is not last, check with next neighbor
                    postInsertSegCheck(prioritySegSet,iteratorToCurrentSeg, segIntersectionCheckedVec, eventQueue,smartIntersectVect);
                }
                else{//If it is the second point of the segment, then find the segment to remove
//                    iteratorToCurrentSeg = prioritySegSet.find(currSegPtr);
                    iteratorToCurrentSeg = prioritySegSet.end();
                    for (auto iter = prioritySegSet.begin(); iter != prioritySegSet.end(); iter++){
						if (*iter == currSegPtr) {
							iteratorToCurrentSeg = iter;
							break;
						}
					}
                    if (iteratorToCurrentSeg == prioritySegSet.end()) {
						cout << "should not happen " << endl;
						exit(10);
					}
                    removeSeg(prioritySegSet, iteratorToCurrentSeg, segIntersectionCheckedVec, eventQueue,smartIntersectVect);
                }
            }
			#if DEBUG_OUT
				//cout << "After processing endpoint P_" << currPoint->endpt->getIndex() << endl;
				cout << "T list: " << getTListContent(prioritySegSet,segIntersectionCheckedVec, swappedSegmentVec) << endl;
			#endif
             
        }else{ //if currPoint is an intersection point then execute this code snippet
            //Get the segmentlist of each point and push it in the priority seg set according to the order
            pair<shared_ptr<Segment>,shared_ptr<Segment> > currSegPair = (currPoint->interPt)->segPair;

			auto currentSeg = currSegPair.first;  //this is current seg
			auto nextSeg = currSegPair.second;   //and this is next
			auto checkSegPair = currentSeg->getIndex() < nextSeg->getIndex() ?
									make_pair(currentSeg,nextSeg) : make_pair(nextSeg,currentSeg);
			//check if these 2 segments are swapped before or not
			vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >::iterator findPairItr2 = std::find(swappedSegmentVec.begin(),swappedSegmentVec.end(),checkSegPair );
			if (findPairItr2 == swappedSegmentVec.end()){
				//swap them
				//take them out and put it back with respect to new y coordinate
				prioritySegSet.erase(currentSeg);
				prioritySegSet.erase(nextSeg);
				scanY = (currPoint->interPt)->y;
				
				auto insertIter = prioritySegSet.insert(nextSeg).first;
				if (insertIter != prioritySegSet.begin()){
                    compareSegments(*(prev(insertIter)), *insertIter,segIntersectionCheckedVec,eventQueue,smartIntersectVect,prioritySegSet);
				}
				insertIter = prioritySegSet.insert(currentSeg).first;
				if (next(insertIter) != prioritySegSet.end()){
                    compareSegments(*insertIter, *(next(insertIter)),segIntersectionCheckedVec,eventQueue,smartIntersectVect,prioritySegSet);
				}
				
				swappedSegmentVec.push_back(checkSegPair);
				
				#if DEBUG_OUT
					cout << "After processing intersection between S_" << currentSeg->getIndex() << " and S_" << nextSeg->getIndex() << endl;
					cout << "Event Queue:\n" <<getEventQueueContent(eventQueue, true) << endl;
					cout << "T list: " << getTListContent(prioritySegSet,segIntersectionCheckedVec, swappedSegmentVec) << endl;
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
        shared_ptr<InterQueueEvent> currPoint = *itr;
        
        if(currPoint->isIntersection){
			shared_ptr<IntersectionPointStruct> pt = currPoint->interPt;
			outStream << "\tInt " << "(" << pt->x << ", " << pt->y << "), ";
			outStream << "intersection of Segments ";
			outStream << "S_" << pt->segPair.first->getIndex() << " and " <<
						 "S_" << pt->segPair.second->getIndex() << endl;
		}
		else{
			shared_ptr<Point> pt = currPoint->endpt;
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
										const vector<pair<shared_ptr<Segment>, shared_ptr<Segment> > >& segIntersectionCheckedVec,
										const vector<pair<shared_ptr<Segment>, shared_ptr<Segment> > >& swappedSegmentVec) const{
	ostringstream outStream;
	
	for(auto seg : prioritySegSet){
		outStream << "S_" << seg->getIndex() << " [P_" << seg->getP1()->getIndex() << ", " <<
					 "P_" << seg->getP2()->getIndex() << "] ";
	}
	outStream << endl << "Segment pair tested: ";
	for (const auto& segPair : segIntersectionCheckedVec){
		outStream << "(S_" << segPair.first->getIndex() << ", " << "S_" << segPair.second->getIndex() << ") ";
	}
	outStream << endl << "Segment pair swapped: ";
	for (const auto& segPair : swappedSegmentVec){
		outStream << "(S_" << segPair.first->getIndex() << ", " << "S_" << segPair.second->getIndex() << ") ";
	}
	
	return outStream.str();

}
