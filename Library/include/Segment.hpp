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

	class Point;

	class Segment{
		private:
			//std::pair< std::shared_ptr<Point>,std::shared_ptr<Point> > endpts;
			Point& p1;
			Point& p2;
						
			Segment(Point& pt1, Point& pt2);
			static std::set<std::shared_ptr<Segment> > segSet;
			static unsigned int count;
			unsigned int idx;
		public:
			static std::shared_ptr<Segment> makeNewSeg(Point& pt1, Point& pt2);
			
	};
}
#endif /* Segment_hpp */
