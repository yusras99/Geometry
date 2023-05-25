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
#include "Segment.hpp"
#include "Point.hpp"
#include "Geometry.hpp"
#include "IntersectionQuery.hpp"

using namespace std;
using namespace geometry;

IntersectionQuery(const std::vector<std::shared_ptr<Segment> >& vect)
	

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
set<shared_ptr<geometry::InterQueueEvent> , compareEvent> IntersectionQuery::buildEventSet(){
//    vector<shared_ptr<Point> > endpointVect = getAllEndPoints(vect_);
    set<shared_ptr<InterQueueEvent> , compareEvent> eventQueue;
        for (auto itr = vect_.begin(); itr != vect_.end(); itr++){
            const std::shared_ptr<InterQueueEvent> endPt1 = make_shared<InterQueueEvent>();
            const std::shared_ptr<InterQueueEvent> endPt2 = make_shared<InterQueueEvent>();
//            static const std::set<std::shared_ptr<Point> >& allPointSet = Point::getAllPoints();
//            std::shared_ptr<Point> currPoint = allPointSet.find(&(*itr)->getP1());
            endPt1->endpt = (*itr)->getP1();
            endPt1->isIntersection = false;
            eventQueue.insert(endPt1);
            
            endPt2->endpt = (*itr)->getP2();
            endPt2->isIntersection = false;
            eventQueue.insert(endPt2);
        }
    return eventQueue;
}

void IntersectionQuery::addEvent(set<shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,shared_ptr<PointStruct> currInterPt){
    const std::shared_ptr<InterQueueEvent> currPoint;
    currPoint->interPt = currInterPt;
    currPoint->isIntersection = true;
    eventQueue.insert(currPoint);
}
vector<shared_ptr<PointStruct> > IntersectionQuery::findAllIntersectionsSmart(){
    
    vector<shared_ptr<PointStruct> > smartIntersectVect;
    //    Call this function to populate the segmentPoints
    set<shared_ptr<InterQueueEvent> , compareEvent> eventQueue = buildEventSet();
    //   The T data structure is a set of shared pointers to segments and compare function that will sort the segments
//    set< pair < shared_ptr<Segment>,shared_ptr <Point> >, compareSegment> prioritySegSet;
    set< shared_ptr<Segment>, compareSegment> prioritySegSet;
    //The vector of segment pairs which have been swapped
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > > swappedSegs;
    //The vector segment pairs which have been checked for intersection
    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > > intersectionCheckSegs;
    
    for(auto itr = eventQueue.begin(); itr != eventQueue.end(); itr++){
        shared_ptr<InterQueueEvent> currPoint = *itr;
        //Get the segmentlist of each point and push it in the priority seg set according to the order
        set<shared_ptr<Segment>> currSegSet = (currPoint->endpt)->getSegList();
        if(!currPoint->isIntersection){
            //Execute this loop to add/remove each of the segments one by one in the segSet
            for(auto segItr : currSegSet){
                //If currPoint is the first point of the segment push it on the prioritySegSet else remove it
                if(currPoint->endpt == (segItr)->getP1()){
                    prioritySegSet.insert(segItr);
                }else{
//                    auto priorityItr = std::find_if(prioritySegSet.begin(), prioritySegSet.end(), pair< shared_ptr<Segment>,shared_ptr <Point> > p ){
//                        return p.first == segVect[segItr]; });
//                    if (priorityItr != prioritySegSet.end()){prioritySegSet.erase(priorityItr);}
                        prioritySegSet.erase(segItr);
                }
            }
            //Now that the segments relating to the currPoint are added
            //check if there is only one segment in the set then continue otherwise check for intersections
            if(prioritySegSet.size() == 1){
                continue;
            }else{
                //loop over the prioritySegSet to check for intersections
                for(auto it = prioritySegSet.begin(), previt = it++ ; it != prioritySegSet.end(); ++it, ++previt){
                    auto prevSeg = *previt;  //this is current seg
                    auto currentSeg = *it;   //and this is next
                    //check if the currentSeg and prevSeg are already compared or not
                    vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >::iterator findPairItr = std::find(intersectionCheckSegs.begin(),                        intersectionCheckSegs.end(),make_pair(prevSeg,currentSeg) );
                    if (findPairItr != intersectionCheckSegs.end()){
                        continue;
                    }else{//if the segs are not compared before, find intersection and put the point in the vector and in the eventQueue
                        shared_ptr<PointStruct> smartIntersectionPt = prevSeg->findIntersection(*(currentSeg));
                        if (smartIntersectionPt != nullptr){
                            smartIntersectVect.push_back(smartIntersectionPt);
                            addEvent(eventQueue, smartIntersectionPt);
                            intersectionCheckSegs.push_back(make_pair(prevSeg,currentSeg));
                        }
                    }
                }
            }
        }else{ //if currPoint is an intersection point then execute this code snippet
            for(auto it = currSegSet.begin(), previt = it++ ; it != currSegSet.end(); ++it, ++previt){
                auto prevSeg = *previt;  //this is current seg
                auto currentSeg = *it;   //and this is next
                //check if these 2 segments are swapped before or not
                vector< pair<shared_ptr<Segment>, shared_ptr<Segment> > >::iterator findPairItr2 = std::find(swappedSegs.begin(),swappedSegs.end(),make_pair(prevSeg,currentSeg) );
                if (findPairItr2 != swappedSegs.end()){
                    continue;
                }else{
                    //swap them
                    //take them out and put it back with respect to new y coordinate
                }
            }
        }
    }
    return smartIntersectVect;
}
