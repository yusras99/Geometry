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
#include "Point.hpp"

namespace geometry {

    /** Enum type only used for rendering.
     */
    enum class SegmentType{
        SEGMENT,
        CREATED_SEGMENT,
        EDITED_SEGMENT
    };
    
    /**    Class created for passkey purposes (so that make_shared can make calls to
     *    a public constructor, but nobody else can.
     */
    class SegmentToken{
        private:
            SegmentToken(void) {}
        
        friend class Segment;
    };

    class Segment{

        private:

            Point& p1_;
            Point& p2_;
            unsigned int idx_;
                        
            static std::set<std::shared_ptr<Segment> > segSet_;
            static unsigned int count_;

            //    Disabled constructors and operators
            Segment(void) = delete;
            Segment(const Segment& ) = delete;
            Segment(Segment&& ) = delete;
            Segment& operator = (const Segment& ) = delete;
            Segment& operator = (Segment&& ) = delete;

            static void render_(const Point& pt1, const Point& pt2, SegmentType type);
            
        public:
        
            Segment(SegmentToken token, Point& pt1, Point& pt2);

            ~Segment(void);

            inline unsigned int getIndex(void) const {
                return idx_;
            }

            
            void render(SegmentType type = SegmentType::SEGMENT) const;

            bool isOnLeftSide(const Point& pt) const;
            bool isOnLeftSide(const PointStruct& pt) const;
            bool areOnOppositeSides(const Point& pt1, const Point& pt2) const;
            bool areOnOppositeSides(const PointStruct& pt1, const PointStruct& pt2) const;
            bool intersects(const Segment& seg) const;
            bool intersects(const Point& pt1, const Point& pt2) const;
            bool intersects(const PointStruct& pt1, const PointStruct& pt2) const;
                static std::shared_ptr<Segment> makeNewSegPtr(Point& pt1, Point& pt2);

            static Segment& makeNewSeg(Point& pt1, Point& pt2);

            static const std::set<std::shared_ptr<Segment> >& getAllSegments(void){
                return segSet_;
            }

            static void clearAllSegments(void){
                segSet_.clear();
                count_ = 0;
            }

            static void renderCreated(const PointStruct& pt1, const PointStruct& pt2);
            static void renderAllSegments(void);
    };
}
#endif /* Segment_hpp */
