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
#include <set>
#include <map>
#include "Geometry.hpp"
#include "Segment.hpp"

namespace geometry{

	inline float scanY;

    /**@struct InterQueueEvent is a type of variables that are event queue structure will contain
    * @var InterQueueEvent::isIntersection tells if the point is an intersection point or a regular point
    * @var InterQueueEvent::endpt specifies shared pointer to a regular endpoint
    * @var InterQueueEvent::interPt specifies shared pointer to an intersection point of type Pointstruct
    */
    struct InterQueueEvent{
        bool isIntersection;
        std::shared_ptr<Point> endpt;
        std::shared_ptr<PointStruct> interPt;
    };

    /** Compare struct that helps to compare between InterQueueEvent points, to decide their position in the queue
     */
    struct compareEvent{
      bool operator()(const std::shared_ptr<geometry::InterQueueEvent>  p1, const std::shared_ptr<geometry::InterQueueEvent> p2){
          float x1;
          float y1;
          float x2;
          float y2;
          if((*p1).isIntersection){
              x1 = ((*p1).interPt)->x;
              y1 = ((*p1).interPt)->y;
          }else{
              x1 = ((*p1).endpt)->getX();
              y1 = ((*p1).endpt)->getY();
          }if((*p2).isIntersection){
              x2 = ((*p2).interPt)->x;
              y2 = ((*p2).interPt)->y;
          }else{
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
    /** Compare struct that helps to compare between Segments to order the prioritySegSet according to the order of segments position on x axis
     */
    struct compareSegment{
      bool operator()(const std::shared_ptr<geometry::Segment>  s1, const std::shared_ptr<geometry::Segment> s2){
          float scanX1;
          float scanX2;
          float slope1;
          float slope2;
          float s1x1;
          float s1y1;
          float s1x2;
          float s1y2;
          
          float s2x1;
          float s2y1;
          float s2x2;
          float s2y2;
          s1x1 = (s1->getP1()).getX();
          s1y1 = (s1->getP1()).getY();

          s1x2 = (s1->getP2()).getX();
          s1y2 = (s1->getP2()).getY();
          
          s2x1 = (s2->getP1()).getX();
          s2y1 = (s2->getP1()).getY();

          s2x2 = (s2->getP2()).getX();
          s2y2 = (s2->getP2()).getY();
          slope1 = (s1y2 - s1y1) / (s1x2 - s1x1);
          slope2 = (s2y2 - s2y1) / (s2x2 - s2x1);
          //if scan line's y coord is not the first point of the segment then compute the x coord for the scan y
          if(scanY != s1y1){
                    scanX1 =  s1x1 + ( (scanY - s1y1) * (1 / slope1) );
          }else{//else the x of the scanY will be the x of the first point of the segment
                    scanX1 = s1x1;
          }
          //Do the same for segment 2, if scan line's y coord is not the first point of the segment then compute the x coord for the scan y
          if(scanY != s2y1){
                    scanX2 =  s2x1 + ( (scanY - s2y1) * (1 / slope2) );
          }else{//else the x of the scanY will be the x of the first point of the segment
                    scanX2 = s2x1;
          }//if x1 is less than it means s1 is on left and s2 on right
          if(scanX1 < scanX2){
              return scanX1 < scanX2;
          }else if(scanX1 == scanX2){//if x are same,that means intersection, so we return slope to determine position of both segs
              return slope1 <= slope2;
          }
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
		
			const std::vector<std::shared_ptr<Segment> >& vect_;
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
             void addEvent(std::set<std::shared_ptr<geometry::InterQueueEvent> , compareEvent>& eventQueue,std::shared_ptr<PointStruct> currInterPt);
	};
}
#endif // INTERSECTION_QUERY_HPP
