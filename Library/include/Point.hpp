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
#include <cmath>
#include "Geometry.hpp"


namespace geometry {
    /** Forward segment decleration
     */
    class Segment;

	/** Enum type only used for rendering.  I am wodering whether it should be
	 *	renamed PointRenderMode
	 */
	enum class PointType{
		DEDUCED_TYPE,
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
		
		/** The point's horizontal coordinate.
		 */
		float x;

		/** The point's vertical coordinate.
		 */
		float y;

		/** Empty constructor */
		PointStruct() {};
		
		/** Intializes a PointStruct object.  This constructor (and therefore the
		 *	previous one, seem to be needed because make_unique doesn't play nice
		 *	with brace initialization [???]
		 *	@param theX	 our point's x coordinate
		 *	@param theY	 our point's y coordinate
		 */
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
        friend struct InterQueueEvent;
        
		private:

			float x_;
			float y_;
			// Seglist of all the segments these endpoints belong to
			std::set<std::shared_ptr<Segment> > segList_;
			unsigned int idx_;
        
			static unsigned int count_;
            //The set of all points which are unique and can autosort itself
			static std::set<std::shared_ptr<Point> > pointSet_;
        
			static std::vector<std::shared_ptr<Point> > pointVect_;
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
            
            /**Constructor function that can only be called using the class itself and not by user
             * @param xCoord  the x coordinate which will be used to create a point
             * @param yCoord  the y coordinate which will be used to create a point
             * @param token - the object of segmentToken class (which acts like passkey attribute) so that it is not accessible by anyone else
             */
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
            std::set<std::shared_ptr<Segment> > getSegList(void) const{
                return segList_;
            }
			void render(PointType type = PointType::DEDUCED_TYPE) const;


			static void render(const PointStruct& pt,
							   PointType type = PointType::FIRST_ENDPOINT);

            /**This is a maker function that returns a shared pointer to a point that can auto destruct itself when the point is no longer needed
             *@params xCoord - The x coordinate of the point
             *@params yCoord - The y coordinate of the point
             *@return a shared pointer to the point
             */
			static std::shared_ptr<Point> makeNewPointPtr(float xCoord,float yCoord);
            /**Maker function that calls the other constructor function and gets the point on that pointer
             * @params xCoord - The x coordinate of the point
             * @params yCoord - The y coordinate of the point
             * @return the reference to a point
             */
			static Point& makeNewPoint(float xCoord,float yCoord);
			
            static const std::set<std::shared_ptr<Point> >& getAllPoints(void){
                return pointSet_;
            }
			static float distanceSq(float x1, float y1, float x2, float y2);

			inline float distance(const PointStruct& pt) const{
				return sqrtf(distanceSq(x_, y_, pt.x, pt.y));
			}
			inline float distance(const Point& pt) const{
				return sqrtf(distanceSq(x_, y_, pt.x_, pt.y_));
			}
			inline float distance(float x, float y) const{
				return sqrtf(distanceSq(x_, y_, x, y));
			}
			inline float distanceSq(const PointStruct& pt) const{
				return distanceSq(x_, y_, pt.x, pt.y);
			}
			inline float distanceSq(const Point& pt) const{
				return distanceSq(x_, y_, pt.x_, pt.y_);
			}
			inline float distanceSq(float x, float y) const{
				return distanceSq(x_, y_, x, y);
			}
			inline bool isSame(const PointStruct& pt) const{
				return (distanceSq(x_, y_, pt.x, pt.y) == 0);
			}
			inline bool isSame(const Point& pt) const{
				return (distanceSq(x_, y_, pt.x_, pt.y_) == 0.f);
			}
			inline bool isSame(float x, float y) const{
				return (distanceSq(x_, y_, x, y) == 0);
			}
			inline static float distance(float x1, float y1, float x2, float y2){
				return sqrtf(distanceSq(x1, y1, x2, y2));
			}
			
			inline static float distanceSq(const PointStruct& pt1, const PointStruct& pt2){
				return distanceSq(pt1.x, pt1.y, pt2.x, pt2.y);
			}
			inline static float distance(const PointStruct& pt1, const PointStruct& pt2){
				return sqrtf(distanceSq(pt1.x, pt1.y, pt2.x, pt2.y));
			}

			inline static bool isSame(float x1, float y1, float x2, float y2){
				return (distanceSq(x1, y1, x2, y2) == 0.f);
			}
			
			inline static bool isSame(const PointStruct& pt1, const PointStruct& pt2){
				return (distanceSq(pt1.x, pt1.y, pt2.x, pt2.y) == 0.f);
			}

			static void clearAllPoints(void) {
				pointSet_.clear();
				count_ = 0;
			}

			static void setPointDiskRadius(float radius);

			static void renderAllSinglePoints(void);
			
	};
	using PointPtr = std::shared_ptr<Point>;

}	//	end of namespace
#endif /* Point_hpp */
