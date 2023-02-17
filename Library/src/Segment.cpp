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
	//	add myself to segList of p1 and p2
	p1.segList.insert(idx);
	p2.segList.insert(idx);
}

shared_ptr<Segment> Segment::makeNewSeg(Point& pt1, Point& pt2){
    bool found = false;
    shared_ptr<Segment> p;
 //   pair<Endpoint,Endpoint> searchPair = make_pair(pt1,pt2);
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
