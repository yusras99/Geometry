//
//  Intersection.hpp
//  Demo
//
//  Created by Jean-Yves Hervé on 2023-05-04.
//  Copyright © 2023 URI 3D Group. All rights reserved.
//

#ifndef INTERSECTION_QUERY_HPP
#define INTERSECTION_QUERY_HPP

#include <utility>
#include <memory>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include "Geometry.hpp"
#include "Segment.hpp"

namespace geometry{

	inline float scanY;

    /**@struct InterQueueEvent is a type of variables that are event queue structure will contain
    *@var InterQueueEvent::isIntersection tells if the point is an intersection point or a regular point
    *@var InterQueueEvent::endptPtr specifies shared pointer to a regular endpoint
    *@var InterQueueEvent::interPtPtr specifies shared pointer to an intersection point of type Pointstruct
    */
    struct InterQueueEvent{
        bool isIntersection;
        std::shared_ptr<Point> endptPtr;
        std::shared_ptr<IntersectionPointStruct> interPtPtr;
    };

    /** Compare struct that helps to compare between InterQueueEvent points, to decide their position in the queue
     */
	struct compareEvent{
		float testInstance;
		bool operator()(const std::shared_ptr<geometry::InterQueueEvent>  p1, const std::shared_ptr<geometry::InterQueueEvent> p2) const{
			if (p1 != p2){
				float x1;
				float y1;
				float x2;
				float y2;
				if(p1->isIntersection){
					x1 = p1->interPtPtr->x;
					y1 = p1->interPtPtr->y;
				}else{
					x1 = p1->endptPtr->getX();
					y1 = p1->endptPtr->getY();
				}
				if(p2->isIntersection){
					x2 = p2->interPtPtr->x;
					y2 = p2->interPtPtr->y;
				}else{
					x2 = p2->endptPtr->getX();
					y2 = p2->endptPtr->getY();
				}

				if(y1 != y2){
					return y1 > y2;
				}else{
					return x1 < x2;
				}
			}else{
				return false;
			}
		}
    };
    /** Compare struct that helps to compare between Segments to order the prioritySegSet according to the order of segments position on x axis
     */
	struct compareSegment{
		/**	Compares two segments implementing strict weak ordering at the current scan line
		 *	(returns true if the first is strictly on the left side of the second at the current scan line)
		 * @param s1	the first segment in the comparison pair
		 * @param s2	the second segment in the comparison pair
		 * @return <code>true</code> if the first segment is strictly on the left of the second segment
		 * 			at the current scan line, <code>false</code> otherwise.
		 */
		bool operator()(const std::shared_ptr<geometry::Segment>  s1, const std::shared_ptr<geometry::Segment> s2) const{
			bool returnVal;
//			std::cout << "Comparing Segment " << s1->getIndex() << " and Segment " << s2->getIndex();
			if (s1 != s2){
				float s1x1 = (s1->getP1())->getX();
				float s1y1 = (s1->getP1())->getY();

				float s1x2 = (s1->getP2())->getX();
				float s1y2 = (s1->getP2())->getY();

				float s2x1 = (s2->getP1())->getX();
				float s2y1 = (s2->getP1())->getY();

				float s2x2 = (s2->getP2())->getX();
				float s2y2 = (s2->getP2())->getY();
				float slope1 = (s1y2 - s1y1) / (s1x2 - s1x1);
				float slope2 = (s2y2 - s2y1) / (s2x2 - s2x1);
				//if scan line's y coord is not the first point of the segment then compute the x coord for the scan y
				float scanX1;
				if(scanY != s1y1){
					scanX1 =  s1x1 + ( (scanY - s1y1) / slope1 );
				}else{//else the x of the scanY will be the x of the first point of the segment
					scanX1 = s1x1;
				}
				//Do the same for segment 2, if scan line's y coord is not the first point of the segment then compute the x coord for the scan y
				float scanX2;
				if(scanY != s2y1){
					scanX2 =  s2x1 + ( (scanY - s2y1) / slope2 );
				}else{//else the x of the scanY will be the x of the first point of the segment
					scanX2 = s2x1;
				}
				if (abs(scanX1-scanX2) > Geometry::DISTANCE_ABS_TOL){
					if(scanX1 < scanX2){
						returnVal = true;
					}else /*if(scanX1 > scanX2)*/{
						//if x are same,that means intersection, so we return slope to determine position of both segs
						returnVal = false;
					}
				// If the scan line is at the intersection point (we are comparing again the two
				// segments whose intersection defined the current event point and current scanY)
                }else{
                    if(slope1 <0)
                        returnVal = (slope2 < 0) && (slope1 < slope2);
                    else
                        returnVal = (slope2 < 0) || (slope2 > slope1);
				}
				
			}
			else{
				returnVal = false;
			}
//			std::cout << ": " << (returnVal ? "true" : "false") << std::endl;
			return returnVal;
		}
    };

	class IntersectionQuery{

		public:
		
			IntersectionQuery(const std::vector<std::shared_ptr<Segment> >& vect);
			~IntersectionQuery() = default;
			
			IntersectionQuery() = delete;
			IntersectionQuery(const IntersectionQuery&) = delete;
			IntersectionQuery(IntersectionQuery&&) = delete;
			IntersectionQuery& operator=(const IntersectionQuery&) = delete;
			IntersectionQuery& operator=(IntersectionQuery&&) = delete;
			
		   /**Intersection function that finds all intersections between the segments of a vector of shared pointers to the segments using brute force
			 * @return a vector of unique pointers to type pointStruct that are intersection points
			 */
			std::vector<std::shared_ptr<PointStruct> > findAllIntersectionsBruteForce();

			/**Intersection function that finds all intersections between the segments of a vector of shared pointers to the segments using smart way of computational geometry
			 * @return a vector of unique pointers to type PointStruct that are intersection points
			 */
			std::vector<std::shared_ptr<PointStruct> > findAllIntersectionsSmart();

			
		private:
		
			const std::vector<std::shared_ptr<Segment> >& potentialInterSegVect_;
            /**
             *This function creates a set of type InterQueueEvent, which is the event Queue. It uses  a vector of shared pointers to the segments whose endpoints & intersection points need to be added in the queue
             *@return a vector of shared pointers to the the points of queue of type InterQueueEvent
             */
             std::set<std::shared_ptr<geometry::InterQueueEvent> , compareEvent> buildEventSet();
            /**
             *This function adds the intersection points in the eventQueue
             *@param eventQueue - a reference to the eventQueue set in which we will add the intersection point
             *@param currInterPt - the intersection point which has to be added in the eventQueue
             */
             void addEvent(std::set<std::shared_ptr<geometry::InterQueueEvent> , compareEvent>& eventQueue,std::shared_ptr<IntersectionPointStruct> currInterPt);
             /**IntersectionChecking function that sees if the the two segments passed to it intersect or not. If they do the intersection point is added to the eventQueue and the smartIntersectVect 
              *@param currSegOfEvent - the segment which will be checked for intersection
              *@param compareSeg  - neighbor of current seg which will be checked for intersection with currSegOfEvent
              *@param comparedSegForInterVec - the vector that will have a pair of shared pointers to segments that have been checked for intersection
              *@param eventQueue  - the queue that holds all points that need to be processed
              *@param smartIntersectVect - Vector that contains all intersection points
              *@param prioritySegSet - The T structure that contains all segments which will be checked for intersection
              */
             void checkSegIntersection(std::shared_ptr<Segment> currSegOfEvent,
                                 std::shared_ptr<Segment> compareSeg,
                                 std::vector< std::pair<std::shared_ptr<Segment>,std::shared_ptr<Segment> > >& comparedSegForInterVec,
                                 std::set<std::shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,  std::vector<std::shared_ptr<PointStruct> >& smartIntersectVect,
                                 std::set< std::shared_ptr<Segment>, compareSegment>& prioritySegSet);
            /**A function to check the neighbors of the segment which is just added in the T structure to see if they intersect with the seg that is just inserted  or not.
             *@param prioritySegSet - The T structure that contains all segments which will be checked for intersection
             *@param currSegOfEventItr - the iterator to the segment that is just added
             *@param comparedSegForInterVec - the vector that will have a pair of shared pointers to segments that have been checked for intersection
             *@param eventQueue  - the queue that holds all points that need to be processed
             *@param smartIntersectVect - Vector that contains all intersection points
             */
             void postInsertSegCheck(std::set<std::shared_ptr<Segment>, compareSegment>& prioritySegSet,
                                      std::set<std::shared_ptr<Segment>, compareSegment>::iterator currSegOfEventItr,
                                      std::vector< std::pair<std::shared_ptr<Segment>,std::shared_ptr<Segment> > >&comparedSegForInterVec,
                                      std::set<std::shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,
                                      std::vector<std::shared_ptr<PointStruct> >& smartIntersectVect);
            /**A function to remove the segment which is being processed right not and check if its left and right neighbors intersect with each other or not.
             *@param prioritySegSet - The T structure that contains all segments which will be checked for intersection
             *@param currSegOfEventItr - the iterator to the segment that is just added
             *@param comparedSegForInterVec - the vector that will have a pair of shared pointers to segments that have been checked for intersection
             *@param eventQueue  - the queue that holds all points that need to be processed
             *@param smartIntersectVect - Vector that contains all intersection points
             */
            void removeSeg(std::set<std::shared_ptr<Segment>, compareSegment>& prioritySegSet,
                                          std::set<std::shared_ptr<Segment>, compareSegment>::iterator currSegOfEventItr,
                                          std::vector< std::pair<std::shared_ptr<Segment>,std::shared_ptr<Segment> > >&comparedSegForInterVec,
                                          std::set<std::shared_ptr<InterQueueEvent> , compareEvent>& eventQueue,
                                          std::vector<std::shared_ptr<PointStruct> >& smartIntersectVect);
            /**Produces a string (for file or terminal output) giving the current content of
			 * the event queue
			 * @parameter eventQueue  the event queue to print the content of
			 * @parameter detailed 	if true, then detailed information about the event points
			 * 						(indices of the segments, etc.) is written as well.
			 * @return formatted string giving the content of the event queue
			 */
			std::string getEventQueueContent(const std::set<std::shared_ptr<InterQueueEvent>, compareEvent >& eventQueue,
											 bool detailed=false) const;
			
			/**Produces a string (for file or terminal output) giving the current content of
			 * the T list
			 * @parameter prioritySegSet the T list we want to pront the content of
			 * @parameter intersectionCheckSegs the list of pairs of segments that have been tested for intersection
			 * @parameter swappedSegs the list of pairsof segments that have already been swapped
			 * @return formatted string giving the content of the T list
			 */
			std::string getTListContent(const std::set<std::shared_ptr<Segment>, compareSegment >& prioritySegSet,
										const std::vector<std::pair<std::shared_ptr<Segment>, std::shared_ptr<Segment> > >& intersectionCheckSegs,
										const std::vector<std::pair<std::shared_ptr<Segment>, std::shared_ptr<Segment> > >& swappedSegs) const;
	};
}
#endif // INTERSECTION_QUERY_HPP