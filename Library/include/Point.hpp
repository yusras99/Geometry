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
#include "glPlatform.h"

namespace geometry {

	/** Enum type only used for rendering.  I am wodering whether it should be
	 *	renamed PointRenderMode
	 */
	enum class PointType{
		DEDUCED_TYPE,
		//
		SINGLE_POINT,
		ENDPOINT,
		FIRST_ENDPOINT,
		EDIT_POINT,
		INTERSECTION_POINT
	};

	/** Struct used to stort the coordinates of a "potential Point," by which
	 *	we mean a geometric point that hasn't been confirmed as a Point object.
	 */
	struct PointStruct{
		float x, y;
		
		PointStruct() {};
		
		PointStruct(float theX, float theY)
			: x(theX), y(theY) {};
	};

	/**	Class created for passkey purposes (so that make_shared can make calls to
	 *	a public constructor, but nobody else can.
	 */
	class PointToken{
		private:
			PointToken(void) {}
		
		friend class Point;
	};


	class Point{

		friend class Segment;
		
		private:

			float x_;
			float y_;
			// Seglist of all the segments these endpoints belong to
			std::set<unsigned int> segList_;
			unsigned int idx_;

			static unsigned int count_;
			static std::set<std::shared_ptr<Point> > pointSet_;
			static std::vector<std::shared_ptr<Point> > pointList_;
			static float pointDiskRadius_;
			static GLuint diskList_;
			static GLuint circleList_;
			static bool displayListsInitialized_;
			
			
			static void initDisplayLists_(void);
			

		public:

			Point(void) = delete;
			Point(const Point& pt) = delete;
			Point(Point&& pt) = delete;
			Point& operator = (const Point& pt) = delete;
			Point& operator = (Point&& pt) = delete;

			Point(PointToken token, float xCoord, float yCoord);

			~Point(void) = default;

			inline float getX(void) const{
				return x_;
			}
			inline float getY(void) const{
				return y_;
			}
	 
			inline unsigned int getIndex(void) const{
				return idx_;
			}
			
			inline void setCoordinates(float x, float y){
				x_ = x; y_ = y;
			}
			
			bool isSingle(void) const{
				return segList_.size() == 0;
			}
			
			size_t getConnectivityDegree(void) const{
				return segList_.size();
			}
			
			void render(PointType type = PointType::DEDUCED_TYPE) const;

			float distance(const PointStruct& pt) const;
			float distance(const Point& pt) const;
			float distanceSq(const PointStruct& pt) const;
			float distanceSq(const Point& pt) const;

			static void render(const PointStruct& pt,
							   PointType type = PointType::FIRST_ENDPOINT);


			static std::shared_ptr<Point> makeNewPointPtr(float xCoord,float yCoord);

			static Point& makeNewPoint(float xCoord,float yCoord);
			
			static const std::set<std::shared_ptr<Point> >& getAllPoints(void){
				return pointSet_;
			}
			
			static shared_ptr<Point> getPointAtIndex(size_t index);

			static void clearAllPoints(void) {
				pointSet_.clear();
				count_ = 0;
			}

			static void setPointDiskRadius(float radius);

			static void renderAllSinglePoints(void);
			
			static float distance(const PointStruct& pt1, const PointStruct& pt2);
			static float distanceSq(const PointStruct& pt1, const PointStruct& pt2);

	};
	
	using PointPtr = std::shared_ptr<Point>;
	
}	//	end of namespace
#endif /* Point_hpp */
