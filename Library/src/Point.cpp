//
//  Point.cpp
//  
//
//  Created by Yusra Suhail on 2/11/23.
//

#include "Point.hpp"
#include <iostream>
#include <algorithm>
#include <memory>
#include <set>

using namespace std;
using namespace geometry;

//Static variables redeclared in source code
set<shared_ptr<Point> > Point::pointSet;
unsigned int Point::count = 0;
set<int> Poinnt::segList;
unsigned int Segment::count = 0;


Point::Point(float xCoord, float yCoord)
    :
        x(xCoord),
        y(yCoord),
        idx(count++)
{}

Point& Point::makeNewPt(float xCoord,float yCoord){
    bool found = false;
    shared_ptr<Point> p;
    for(auto iter = pointSet.begin(); iter!=pointSet.end();iter++){
        if((*iter)->x == xCoord &&  (*iter)->y == yCoord){
            found = true;
            p = *iter;
            break;
        }
    }
    if (found){
        return *p;
    }
    else{
        shared_ptr<Point> currPt = make_shared<Point>(xCoord,yCoord);
        pointSet.insert(currPt);
        return *currPt;
    }
}
