//
//  Edge.cpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/28/23.
//

#include "Edge.hpp"

using namespace geometry;
using namespace std;


Edge::Edge(shared_ptr<Vertex> v1, shared_ptr<Vertex> v2, unsigned int idx)
    :   Segment(v1->getPoint(), v2->getPoint(), true),
        idx_(idx)
{}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Rendering" functions
//-----------------------------------------------------------------
#endif

//static void render_(const Vertex& v1, const Vertex& v2){
//    glColor4fv({1.f, 1.f, 1.f, 1.f});
//    
//    glBegin(GL_LINES);
//        glVertex2f((v1.getPoint)->getX(), (v1.getPoint)->getY());
//        glVertex2f((v2.getPoint)->getX(), (v2.getPoint)->getY());
//    glEnd();
//}
