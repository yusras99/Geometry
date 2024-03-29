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
#include <vector>
#include <set>
#include <map>
#include "Point.hpp"

namespace geometry {

	/** Enum type only used for rendering.
	 */
	enum class SegmentType{
		SEGMENT,
		CREATED_SEGMENT,
		EDITED_SEGMENT
	};
	
	/**	Class created for passkey purposes (so that make_shared can make calls to
	 *	a public constructor, but nobody else can.
	 */
	class SegmentToken{
		private:
			SegmentToken() {}
		friend class Segment;
	};

    /** Struct used to sort the points of a "potential Segment," by which
     *    we mean a geometric segment that hasn't been confirmed as a Segment object.
     */
	struct SegmentStruct{
		private:
			const PointStruct& p1_;
			const PointStruct& p2_;
			
			SegmentStruct(const PointStruct& pt1, const PointStruct& pt2)
				: p1_(pt1), p2_(pt2)
			{}
			/**
             * we need this function because we need to check if the potential segment is on the opposite side or not
             */
			bool areOnOppositeSides(const PointStruct& pt1, const PointStruct& pt2) const{
				return false;
			}
			
		friend class Segment;
	};
	
	class Segment{

		private:

            std::shared_ptr<Point> p1_;
            std::shared_ptr<Point> p2_;
			unsigned int idx_;
						
			static std::set<std::shared_ptr<Segment> > segSet_;
			static std::vector<std::shared_ptr<Segment> > segVect_;
			static unsigned int count_;

			Segment(std::shared_ptr<Point> pt1, std::shared_ptr<Point> pt2);

			//	Disabled constructors and operators
			Segment(void) = delete;
			Segment(const Segment& ) = delete;
			Segment(Segment&& ) = delete;
			Segment& operator = (const Segment& ) = delete;
			Segment& operator = (Segment&& ) = delete;

			static void render_(const Point& pt1, const Point& pt2, SegmentType type);
			
		public:
		
        Segment(SegmentToken token, std::shared_ptr<Point> pt1, std::shared_ptr<Point> pt2);
 
        ~Segment(void);

			inline unsigned int getIndex(void) const {
				return idx_;
			}
            inline const std::shared_ptr<Point> getP1(void) const {
                return p1_;
            }
            inline const std::shared_ptr<Point> getP2(void) const {
                return p2_;
            }
            //The vector of segments with which this segment has swapped
            std::vector<int> swappedSegs;
            //The vector of segments with which this segment has done a comparison
            std::vector<int> comparedSegs;
			
			void render(SegmentType type = SegmentType::SEGMENT) const;

			bool isOnLeftSide(const std::shared_ptr<Point> pt) const;
            /**Function that checks if the point is on left of the segments or not
             *@param pt - a reference to a constant point struct which has to be checked on its direction to the segment
             *@return boolean that tells if the point is on left or not
             */
			bool isOnLeftSide(const PointStruct& pt) const;

			/**Function that checks the points of the intersecting segment if they are on opposite sides which will set the basis of intersection
			 *@param pt1 - a reference to a constant point 1 which has to be checked on its direction to the segment
			 *@param pt2 - a reference to a constant point 2 which has to be checked on its direction to the segment
			 *@return boolean that tells if the 2 points are on the opposite sides of the currSeg
			 */
			bool areOnOppositeSides(const std::shared_ptr<Point> pt1, const std::shared_ptr<Point> pt2) const;

			/**Function that checks the points of the intersecting segment if they are on opposite sides which will set the basis of intersection
			 *@param pt1 - a reference to a constant point 1 struct which has to be checked on its direction to the segment
			 *@param pt2 - a reference to a constant point 2 struct which has to be checked on its direction to the segment
			 *@return boolean that tells if the 2 points are on the opposite sides of the currSeg
			 */
			bool areOnOppositeSides(const PointStruct& pt1, const PointStruct& pt2) const;
        
			/**Intersection possible iff:
			 *  Seg1.pt1 and seg1.pt2 are on different sides of Seg2
			 *  Seg2.pt1 and seg2.pt2 are on different sides of Seg1
			 *@param seg - the segment which is possible of intersection
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const Segment& seg) const;

			/**
			 *@param pt1 - a const reference to point  - part of a possibly intersecting segment
			 *@param pt2 - a const reference to another point - part of a possibly intersecting segment
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const std::shared_ptr<Point> pt1, const std::shared_ptr<Point> pt2) const;

			/**
			 *@param pt1 - a const reference to pointStruct  - part of a possibly intersecting segment
			 *@param pt2 - a const reference to another pointStruct - part of a possibly intersecting segment
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const PointStruct& pt1, const PointStruct& pt2) const;

			/**Function that checks interSeg with the currSeg to see if there is an intersection, if there is pointer to the intersection pt is returned                            otherwise a null pointer is returned
			 *@param interSeg A const reference to a segment which is possible candidate of an intersection
			 *@return A pointer to Point struct variable which is the intersection point of both segments
			 */
            std::unique_ptr<PointStruct> findIntersection(const Segment& interSeg);
            /**Maker function for the segment that will create a shared pointer to the segment so it can be stored in the set
             * @param pt1 a reference to a point 1 which will be used to create a segment
             * @param pt2 a reference to a point 2 which will be used to create a segment
             * @return a shared pointer to the segment
             */
            static std::shared_ptr<Segment> makeNewSegPtr(std::shared_ptr<Point> pt1, std::shared_ptr<Point> pt2);
             /**Maker function that calls the other constructor function and gets the segment on that pointer
             * @param pt1 a reference to a point 1 which will be used to create a segment
             * @param pt2 a reference to a point 2 which will be used to create a segment
             * @return the reference to a segment
             */
			static Segment& makeNewSeg(std::shared_ptr<Point>  pt1, std::shared_ptr<Point>  pt2);

            static Segment makeNewTempSeg(std::shared_ptr<Point>  pt1, std::shared_ptr<Point>  pt2);

			static const std::vector<std::shared_ptr<Segment> >& getAllSegments(void){
				return segVect_;
			}

			static void clearAllSegments(void){
				segSet_.clear();
				count_ = 0;
			}

			static void renderCreated(const PointStruct& pt1, const PointStruct& pt2);
			static void renderAllSegments(void);
	};
    /**Intersection function that finds all intersections between the segments using brute force
     * @param vect  reference to a vector of shared pointers to the segments whose intersections need to be found
     * @return a vector of unique pointers to type pointStruct that are intersection points
     */
    std::vector<std::unique_ptr<PointStruct> > findAllIntersectionsBruteForce(const std::vector<std::shared_ptr<Segment> >& vect);
    
    // Enum used to store all types of points in the queue
    struct InterQueueEvent{
        bool isIntersection;
        std::shared_ptr<Point> endpt;
        std::shared_ptr<PointStruct> interPt;
        

    };
    /**
     *This function takes the points which lies on the segment and put them in the a vector
     *@param vect  reference to a vector of shared pointers to the segments whose endpoints need to be found
     *@return a vector of shared pointers to the endpoints of type Point
     */
    std::vector<std::shared_ptr<Point> > getAllEndPoints(const std::vector<std::shared_ptr<Segment> >& vect);
    
    // Compare struct that helps to compare between InterQueueEvent points, to decide their position in the queue
    struct compareEvent{
      bool operator()(const std::shared_ptr<geometry::InterQueueEvent>  p1, const std::shared_ptr<geometry::InterQueueEvent> p2){
          float x1;
          float y1;
          float x2;
          float y2;
          if((*p1).isIntersection){
              x1 = ((*p1).interPt)->x;
              y1 = ((*p1).interPt)->y;
              x2 = ((*p2).interPt)->x;
              y2 = ((*p2).interPt)->y;
          }else{
              x1 = ((*p1).endpt)->getX();
              y1 = ((*p1).endpt)->getY();
              x2 = ((*p2).endpt)->getX();
              y2 = ((*p2).endpt)->getY();
          }
          
          if(y1 == y2){
              return x1 < x2;
          }else{
             return y1 > y2;
          }
      }
    };
    // Compare struct that helps to compare between InterQueueEvent points, to decide their position in the queue
    struct compareSegment{
      bool operator()(const std::shared_ptr<geometry::Segment>  s1, const std::shared_ptr<geometry::Segment> s2){

          //return s2.p1_ is on left side of s1
          return (*s1).isOnLeftSide((*s2).getP1());
      }
    };
    /**
     *This function creates a set of type InterQueueEvent, which is the event Queue
     *@param vect  reference to a vector of shared pointers to the segments whose endpoints & intersection points need to be added in the queue
     *@return a vector of shared pointers to the the points of queue of type InterQueueEvent
     */
     std::set<std::shared_ptr<InterQueueEvent> , compareEvent> buildEventSet(const std::vector<std::shared_ptr<Segment> >& vect);
    /**
     *This function adds the intersection points in the eventQueue
     *@param eventQueue - a reference to the eventQueue set in which we will add the intersection point
     *@param currInterPt - the intersection point which has to be added in the eventQueue
     */
     void addEvent(std::set<std::shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,std::shared_ptr<PointStruct> currInterPt);
    /**Intersection function that finds all intersections between the segments using smart way of computational geometry
     * @param vect  reference to a vector of shared pointers to the segments whose intersections need to be found
     * @return a vector of unique pointers to type PointStruct that are intersection points
     */
    std::vector<std::unique_ptr<PointStruct> > findAllIntersectionsSmart(const std::vector<std::shared_ptr<Segment> >& vect);
    void orderMap(std::map<int,unsigned int> prioritySegMap,const std::vector<std::shared_ptr<Segment> >& segVect, std::shared_ptr<InterQueueEvent> currPoint);

}
#endif /* Segment_hpp */
