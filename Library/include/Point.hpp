//
//  Point.hpp
//  
//
//  Created by Yusra Suhail on 2/11/23.
//

#ifndef Point_hpp
#define Point_hpp

#include <memory>
#include <utility>
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
		
		/**	Copy constructor
		 *	@parameter pt	the point to copy
		 */
		PointStruct(const PointStruct& pt) = default;
		
		/** Intializes a PointStruct object.  This constructor (and therefore the
		 *	previous one, seem to be needed because make_unique doesn't play nice
		 *	with brace initialization [???]
		 *	@param theX	 our point's x coordinate
		 *	@param theY	 our point's y coordinate
		 */
		PointStruct(float theX, float theY)
			: x(theX), y(theY) {};
	};
	/** Struct used to  store the intersection point of two (or more?) segments.
	 *	This type only adds to its parent class shared pointers to the segments
	 *	that intersect at this point.
	 */
	struct IntersectionPointStruct : public PointStruct{
	//IntersectionPointStruct is a subclass of PointStruct
		
//		~IntersectionPointStruct() = default;
		
		/** A list of the segments that intersect at this point
		 */
		std::pair<std::shared_ptr<Segment>,std::shared_ptr<Segment> > segPair;
		
		IntersectionPointStruct(std::shared_ptr<PointStruct> pt,
			std::pair<std::shared_ptr<Segment>,std::shared_ptr<Segment> > theSegPair);
		
	};

	/**	Class created for passkey purposes (so that make_shared can make calls to
	 *	a public constructor, but nobody else can.
	 */
	class PointToken{
		private:
			PointToken() {}
		
		friend class Point;
	};


	class Point{

		friend class Segment;
        friend struct InterQueueEvent;
        
		private:

			float x_;
			float y_;
			/** Seglist of all the segments these endpoints belong to
			*/
			std::set<std::shared_ptr<Segment> > segList_;
			unsigned int ptIdx_;
        
			static unsigned int count_;
            /**	The set of all points which are unique and can autosort itself
			 */
			static std::set<std::shared_ptr<Point> > pointSet_;
        
			static std::vector<std::shared_ptr<Point> > pointVect_;
			static float pointDiskRadius_;
			static GLuint diskList_;
			static GLuint circleList_;
			static bool displayListsInitialized_;
			
			
			static void initDisplayLists_();
			

		public:

        Point() = delete;
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
			/** Destructor
			 */
			~Point() = default;
			/** Getter for the x coordinate
			 * @return this point's x coordinate
			 */
			inline float getX() const{
				return x_;
			}
			/** Getter for the y coordinate
			 * @return this point's y coordinate
			 */
			inline float getY() const{
				return y_;
			}
			/** Getter for the index
			 * @return this point's index
			 */
			inline unsigned int getIndex() const{
				return ptIdx_;
			}
			/** Setter for the x and ycoordinates
			 * @param x  new x coordinate to assign to the point
			 * @param y  new y coordinate to assign to the point
			 */
			inline void setCoordinates(float x, float y){
				x_ = x; y_ = y;
			}
			/** Checks whether this point is free/single or assigned to one or more segments
			 *	@return true if the point is free/single (not a segment's endpoint), false otherwise
			 */
			bool isSingle() const{
				return segList_.size() == 0;
			}
			/**	Provides the coonectivity of the point (the number of segments that it is an
			 *	endpoint of).
			 * 	@return	the degree of connectivity of this segment
			 */
			size_t getConnectivityDegree() const{
				return segList_.size();
			}
            const std::set<std::shared_ptr<Segment> > getSegList() const{
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
			
            static const std::set<std::shared_ptr<Point> >& getAllPoints(){
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

			static void clearAllPoints() {
				pointSet_.clear();
				count_ = 0;
			}

			static void setPointDiskRadius(float radius);
			inline static float getPointDiskRadius(){
				return pointDiskRadius_;
			}
			static void renderAllSinglePoints();
			
	};
	using PointPtr = std::shared_ptr<Point>;


}	//	end of namespace
#endif /* Point_hpp */
