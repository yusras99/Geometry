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
#include "Vertex.hpp"
#include "Edge.hpp"

//pure virtual class
namespace geometry{
	
	class Triangle;
	class ConvexPolygon;
	
    /** Compare struct that helps to compare between InterQueueEvent points, to decide their position in the queue
     */
    struct compareVertEvent{
        float testInstance;
        bool operator()(const std::shared_ptr<Vertex>  v1, const std::shared_ptr<Vertex> v2) const{
            if (v1 != v2){
                float x1;
                float y1;
                float x2;
                float y2;
                
                x1 = v1->getPoint()->getX();
                y1 = v1->getPoint()->getY();
                x2 = v2->getPoint()->getX();
                y2 = v2->getPoint()->getY();
                

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
	class Polygon : public std::enable_shared_from_this<Polygon>{

		//list of vertices, list of edges,
		private:
			std::vector<std::shared_ptr<Vertex> > vertexList_;
			std::vector<std::shared_ptr<Edge> > edgeList_;
			
			std::vector<float> fillingColor_;
			std::vector<float> contourColor_;
			
		public:
        
			//	disable copy/move constructors and operators
            Polygon() = delete;
            Polygon(const Polygon& poly) = delete;
            Polygon(Polygon&& poly) = delete;
            Polygon& operator = (const Polygon& poly) = delete;
            Polygon& operator = (Polygon&& poly) = delete;
            
            Polygon(const std::vector<std::shared_ptr<Vertex> >);
			
            virtual ~Polygon() = default;

			virtual void render() const = 0;
			
			
			virtual bool isInside(float x, float y) const;
			virtual inline bool isInside(std::shared_ptr<Point> pt) const{
				return isInside(pt->getX(), pt->getY());
			}
			virtual inline bool isInside(const PointStruct& pt) const{
				return isInside(pt.x, pt.y);
			}
			
			//	see explanation in the source file
//            enum class VertexType{
//                    START,
//                    END,
//                    MERGE,
//                    SPLIT,
//                    REGULAR,
//                    NO_TYPE
//                };
//            VertexType findVertexType(Vertex leftVert, Vertex currVert, Vertex rightVert);
			std::vector<std::shared_ptr<Polygon> > monotonePartition() const;
			std::vector<std::shared_ptr<ConvexPolygon> > convexPartition() const;
			std::vector<std::shared_ptr<Triangle> > triangulation() const;
			
			static bool isStaticVertexList(const std::vector<std::shared_ptr<Vertex> >);
	};
}
#endif /* Polygon_hpp */


//bool isIn = myTriangle->isInside(2.3f, 4.5f);
