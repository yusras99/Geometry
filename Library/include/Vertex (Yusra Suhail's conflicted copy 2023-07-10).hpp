//
//  Vertex.hpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/27/23.
//

#ifndef Vertex_hpp
#define Vertex_hpp

#include <memory>

namespace geometry {
    class Vertex{
        //Vertex is a compostion of Point Class, implementing "has a" relationaship
        std::shared_ptr<Point> Point endPt_;
        //isFirst tells which endpoint is this, first or second
        bool isFirst_;
        Vertex(float x, float y, bool isFirst)
               : endPt_(x,y)
           {
               isFirst_ = isFirst;
           }
         
    };
}

#endif /* Vertex_hpp */
