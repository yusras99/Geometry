//
//  Vertex.cpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/27/23.
//

#include "Vertex.hpp"

using namespace geometry;
using namespace std;

const GLfloat VERTEX_COLOR[][4] = {
                                {0.f, 0.8f, 0.8f, 1.f},    //    START,
                                {1.f, 0.5f, 0.f, 1.f},    //    END,
                                {1.f, 1.f, 0.f, 1.f},    //    MERGE,
                                {1.f, 0.f, 1.f, 1.f},    //    SPLIT,
                                {0.f, 1.f, 0.f, 1.f} ,   //    REGULAR,
                                {1.f, 0.f, 0.f, 1.f}    //    NO_TYPE,
};

Vertex::Vertex(float x, float y)
	:	pt_(Point::makeNewPointPtr(x, y)){}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Maker" functions
//-----------------------------------------------------------------
#endif

//shared_ptr<Vertex> Vertex::makeNewVertexPtr(float xCoord,float yCoord){
//    bool found = false;
//    shared_ptr<Vertex> vert;
//    /** First check if the point exists or not, if it is return the pointer otherwise make the new pointer*/
//    for(auto iter = vertexVector_.begin(); iter!=vertexVector_.end();iter++){
//        if((*iter)->x_ == xCoord &&  (*iter)->y_ == yCoord){
//            found = true;
//            p = *iter;
//            break;
//        }
//    }
//    if (found){
//        return vert;
//    }else{
//        shared_ptr<Vertex> currVertex = make_shared<Vertex>(xCoord,yCoord);
//        vertexVector_.insert(currVertex);
//        return currVertex;
//    }
//}
//
//Vertex& Vertex::makeNewVertex(float xCoord,float yCoord){
//    return *(makeNewVertexPtr(xCoord, yCoord));
//}

#if 0
//-----------------------------------------------------------------
#pragma mark -
#pragma mark "Rendering" functions
//-----------------------------------------------------------------
#endif

//static void Vertex::render(VertexType type){
//    
////    if (!Point::displayListsInitialized_){
////        Point::initDisplayLists_();
////    }
//    switch (type){
//        using enum VertexType;
//        
//        case START:
//            glColor4fv(POINT_COLOR[0]);
//            break;
//
//        case END:
//            glColor4fv(POINT_COLOR[1]);
//            break;
//        
//        case MERGE:
//            glColor4fv(POINT_COLOR[2]);
//            break;
//        
//        case SPLIT:
//            glColor4fv(POINT_COLOR[3]);
//            break;
//        
//        case REGULAR:
//            glColor4fv(POINT_COLOR[4]);
//            break;
//        
//        case NO_TYPE:
//            glColor4fv(POINT_COLOR[5]);
//            break;
//        
//        default:
//            break;
//    }
//    
//    glPushMatrix();
//    glTranslatef(x_, y_, 0.f);
//    //glScalef(pointDiskRadius_, pointDiskRadius_, 1.f);
//    glCallList(diskList_);
//    glFlush();
//    glPopMatrix();
//}


    
