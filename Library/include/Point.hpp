//
//  Point.hpp
//
//
//  Created by Yusra Suhail on 2/11/23.
//

#ifndef Point_hpp
#define Point_hpp

#include <memory>
#include <set>

#include "Segment.hpp"

namespace geometry {
    class Point{

        /** Enum type only used for rendering.  I am wodering whether it should be
         *    renamed PointRenderMode
         */
        enum class PointType
        {
            SINGLE_POINT,
            ENDPOINT,
            FIRST_ENDPOINT,
            EDIT_POINT
        };

        friend class Segment;
        
        private:
            float x;
            float y;
            // Seglist of all the segments these endpoints belong to
            std::set<unsigned int> segList;
            unsigned int idx;
            
            static unsigned int count;
            static std::set<std::shared_ptr<Point> > pointSet;

            Point(float xCoord,float yCoord);
            //    Explicitly disabling the constructors and copy/move
            //    operators that we don't want to be called (implicitly);
            Point(void) = delete;
            Point(const Point& pt) = delete;
            Point(Point&& pt) = delete;
            Point& operator = (const Point& pt) = delete;
            Point& operator = (Point&& pt) = delete;
            
            // Since the constructor wouldn't do anything special here,
            //    we tell the compiler to take care of it
            ~Point(void) = default;
            
        public:
            //const getter functions, so they don't modifiy instance variables. This allows usage of getters in any function that takes as argument a const Point&
            inline float getX(void) const {
                return x;
            }
            inline float getY(void) const {
                return y;
            }
            static Point& makeNewPt(float xCoord,float yCoord);
            
    };
} //    end of namespace
#endif /* Point_hpp */
