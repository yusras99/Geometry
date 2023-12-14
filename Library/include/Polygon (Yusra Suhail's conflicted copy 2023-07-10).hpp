//
//  Polygon.hpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/28/23.
//

#ifndef Polygon_hpp
#define Polygon_hpp

#include <set>
#include <vector>
#include <Vertex.hpp>
#include <Edge.hpp>
namespace geometry{
//pure virtual class
namespace geometry{
	class Polygon{

		//list of vertices, list of edges,
		private:
			std::vector<std::shared_ptr<Vertex> > vertexList_;
			std::vector<std::shared_ptr<Edge> > edgeList_;
			
		public:
			// which constructors/operators???
			
			virtual ~Polygon() = default;

			virtual void render() const = 0;
			
            //pure virtual function that indicates that if child classes can perform these functions better(as in less computations) then they can 
			virtual bool isInside(float x, float y) const;
			virtual inline bool isInside(std::shared_ptr<Point> pt) const{
				return isInside(pt->getX(), pt->getY());
			}
			virtual inline bool isInside(const PointStruct& pt) const{
				return isInside(pt.x, pt.y);
			}
			
			std::vector<std::shared_ptr<ConvexPolygon> convexPartition() const;
			std::vector<std::shared_ptr<Triangle> triangulation() const;
			
		
	};
}
#endif /* Polygon_hpp */


bool isIn = myTriangle->isInside(2.3f, 4.5f);
