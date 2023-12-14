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
#include "Geometry.hpp"
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
			const PointStruct& potentialSegP1_;
			const PointStruct& potentialSegP2_;
			
			SegmentStruct(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2)
				: potentialSegP1_(potentialEndpt1), potentialSegP2_(potentialEndpt2)
			{}
			/**
             * we need this function because we need to check if the potential segment is on the opposite side or not
             */
			bool areOnOppositeSides(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2) const{
				return false;
			}
			
		friend class Segment;
	};
	
	class Segment{
		
		friend class Edge;
		
		private:

            std::shared_ptr<Point> p1_;
            std::shared_ptr<Point> p2_;
			unsigned int idx_;
						
			static std::set<std::shared_ptr<Segment> > segSet_;
			static std::vector<std::shared_ptr<Segment> > segVect_;
			static unsigned int count_;

			/**	Called for imntersection testing.  Creates a temporary Segment object
			 *	with no index.
			 * @param endpt1Ptr first endpoint
			 * @param endpt2Ptr second endpoint
			 */
			Segment(std::shared_ptr<Point> endpt1Ptr, std::shared_ptr<Point> endpt2Ptr,
						bool endpointsOrdered=false);

			//	Disabled constructors and operators
			Segment(void) = delete;
			Segment(const Segment& ) = delete;
			Segment(Segment&& ) = delete;
			Segment& operator = (const Segment& ) = delete;
			Segment& operator = (Segment&& ) = delete;

			static void render_(const Point& endpt1, const Point& endpt2, SegmentType type);
			
		public:
		
			Segment(SegmentToken token, std::shared_ptr<Point> endpt1Ptr, std::shared_ptr<Point> endpt2Ptr,
					bool endpointsOrdered=false);
	 
			~Segment();

			inline unsigned int getIndex() const {
				return idx_;
			}
            inline const std::shared_ptr<Point> getP1() const {
                return p1_;
            }
            inline const std::shared_ptr<Point> getP2() const {
                return p2_;
            }
            /**Maker function for the segment that will create a shared pointer to the segment so it can be stored in the set
             * @param endpt1Ptr a reference to a point 1 which will be used to create a segment
             * @param endpt2Ptr a reference to a point 2 which will be used to create a segment
             * @return a shared pointer to the segment
             */
            static std::shared_ptr<Segment> makeNewSegPtr(std::shared_ptr<Point> endpt1Ptr, std::shared_ptr<Point> endpt2Ptr);
             /**Maker function that calls the other constructor function and gets the segment on that pointer
              * @param endpt1Ptr a reference to a point 1 which will be used to create a segment
              * @param endpt2Ptr a reference to a point 2 which will be used to create a segment
              * @return the reference to a segment
             */
            static Segment& makeNewSeg(std::shared_ptr<Point>  endpt1Ptr, std::shared_ptr<Point>  endpt2Ptr);

            static Segment makeNewTempSeg(std::shared_ptr<Point>  endpt1Ptr, std::shared_ptr<Point>  endpt2Ptr);
            /**
             * @return segVect which is a vector of shared pointers to the Segments
             */
            static const std::vector<std::shared_ptr<Segment> >& getAllSegments(){
                return segVect_;
            }
            /**Destructor function that clears all segments.
             */
            static void clearAllSegments(){
                segSet_.clear();
                segVect_.clear();
                count_ = 0;
            }
            void render(SegmentType type = SegmentType::SEGMENT) const;
            static void renderCreated(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2);
            static void renderAllSegments();

            /**Function that checks if the point is on left of the segment or not
             *@param referencePt a point object which has to be checked on its position relative to the segment
             *@return boolean that tells if the point is on left or not
             */
			bool isOnLeftSide(const Point& referencePt) const;
            /**Function that checks if the point is on left of the segment or not
             *@param referencePtPtr a point object which has to be checked on its position relative to the segment
             *@return boolean that tells if the point is on left or not
             */
            bool isOnLeftSide(const std::shared_ptr<Point> referencePtPtr) const;
        
            /**Function that checks if the point is on left of the segment or not
             *@param potentialPt a point object which has to be checked on its position relative to the segment
             *@return boolean that tells if the point is on left or not
             */
			bool isOnLeftSide(const PointStruct& potentialPt) const;

			/**Function that checks the points of the intersecting segment if they are on opposite sides which will set the basis of intersection
			 *@param endpt1Ptr - a reference to a constant point 1 which has to be checked on its direction to the segment
			 *@param endpt2Ptr - a reference to a constant point 2 which has to be checked on its direction to the segment
			 *@return boolean that tells if the 2 points are on the opposite sides of the currSeg
			 */
			bool areOnOppositeSides(const std::shared_ptr<Point> endpt1Ptr, const std::shared_ptr<Point> endpt2Ptr) const;

			/**Function that checks the points of the intersecting segment if they are on opposite sides which will set the basis of intersection
			 *@param potentialEndpt1 - a reference to a constant point 1 struct which has to be checked on its direction to the segment
			 *@param potentialEndpt2 - a reference to a constant point 2 struct which has to be checked on its direction to the segment
			 *@return boolean that tells if the 2 points are on the opposite sides of the currSeg
			 */
			bool areOnOppositeSides(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2) const;
        
			/**Intersection possible iff:
			 *  Seg1.pt1 and seg1.pt2 are on different sides of Seg2
			 *  Seg2.pt1 and seg2.pt2 are on different sides of Seg1
			 *@param seg - the segment which is possible of intersection
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const Segment& seg) const;

			/**
			 *@param endpt1Ptr - a const reference to point  - part of a possibly intersecting segment
			 *@param endpt2Ptr - a const reference to another point - part of a possibly intersecting segment
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const std::shared_ptr<Point> endpt1Ptr, const std::shared_ptr<Point> endpt2Ptr) const;

			/**
			 *@param potentialEndpt1 - a const reference to pointStruct  - part of a possibly intersecting segment
			 *@param potentialEndpt2 - a const reference to another pointStruct - part of a possibly intersecting segment
			 *@return boolean that tells if the intersection exists between two segments
			 */
			bool intersects(const PointStruct& potentialEndpt1, const PointStruct& potentialEndpt2) const;

			/**Function that checks interSeg with the currSeg to see if there is an intersection, if there is pointer to the intersection pt is returned otherwise a null pointer is returned
			 *@param interSeg A const reference to a segment which is possible candidate of an intersection
			 *@return A pointer to Point struct variable which is the intersection point of both segments
			 */
            std::shared_ptr<PointStruct> findIntersection(const Segment& interSeg);

	};
	
    /**
     *This function takes the points which lies on the segment and put them in the a vector
     *@param vect  reference to a vector of shared pointers to the segments whose endpoints need to be found
     *@return a vector of shared pointers to the endpoints of type Point
     */
    std::vector<std::shared_ptr<Point> > getAllEndPoints(const std::vector<std::shared_ptr<Segment> >& vect);
    

 }
#endif /* Segment_hpp */
