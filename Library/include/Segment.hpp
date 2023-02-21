//
//  Segment.hpp
//
//
//  Created by Yusra Suhail on 2/11/23.
//

#ifndef Segment_hpp
#define Segment_hpp

#include <utility>
#include <memory>
#include <set>

namespace geometry {

        /** Enum type only used for rendering.  I am wodering whether it should be
         *    renamed PointRenderMode
         */
    enum class SegmentType
    {
        SEGMENT,
        CREATED_SEGMENT,
        EDITED_SEGMENT
    };

    class Point;

    class Segment{
        private:
            Point& p1;
            Point& p2;
                        
            Segment(Point& pt1, Point& pt2);

            static std::set<std::shared_ptr<Segment> > segSet;
            static unsigned int count;
            unsigned int idx;
            
            //    We need to implement the destructor because, if a segment is deleted,
            //    then it must be removed from its endpoints' segList
            ~Segment(void);

            //    Disabled constructors and operators
            Segment(void) = delete;
            Segment(const Segment& ) = delete;
            Segment(Segment&& ) = delete;
            Segment& operator = (const Segment& ) = delete;
            Segment& operator = (Segment&& ) = delete;
            
        public:
        
            void render(void, ) const;
            
            static std::shared_ptr<Segment> makeNewSeg(Point& pt1, Point& pt2);
            
    };
}
#endif /* Segment_hpp */
