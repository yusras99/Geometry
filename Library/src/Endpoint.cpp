//
//  Endpoint.cpp
//  
//
//  Created by Yusra Suhail on 2/11/23.
//

#include "Endpoint.hpp"


#include <iostream>
#include <algorithm>
#include <set>
#include <memory>

using namespace std;

//Static variables redeclared in source code

Endpoint::Endpoint(float xCoord, float yCoord)
    :
        x(xCoord),
        y(yCoord),
        idx(Point::count++)
{}
shared_ptr<Endpoint> Endpoint::makeNewEndpt(float xCoord, float yCoord){
    bool found = false;
    shared_ptr<Endpoint> p;
    for(auto iter = endptSet.begin(); iter!=endptSet.end();iter++){
        if(**iter == Endpoint(xCoord,yCoord)){
            found = true;
            p = *iter;
            break;
        }
    }
    if (found){
        return p;
    }
    else{
        shared_ptr<Endpoint> currPt = make_shared<Endpoint>(xCoord,yCoord);
        endptSet.insert(currPt);
        return currPt;
    }
}
