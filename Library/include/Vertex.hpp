//
//  Vertex.hpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/27/23.
//

#ifndef Vertex_hpp
#define Vertex_hpp

#include <memory>
#include <vector>
#include "Point.hpp"

namespace geometry {

	class Edge;
	class Polygon;
	
    class Vertex{
		friend class Polygon;
		
		private:
			std::shared_ptr<Point> pt_;
			std::shared_ptr<Edge> edge_;
			std::shared_ptr<Polygon> polygon_;
//			VertexType type_;
			unsigned int idx_;
//			static unsigned int count_;
//            /**    The vector of all vertices
//             */
//            static std::vector<std::shared_ptr<Vertex> > vertexVector_;

			//	Setters only used by the Polygon
//			void setType(VertexType type){
//                //since the default type of vertex is no_type, if the type is any different than set to that
//				if (type != VertexType::NO_TYPE)
//					type_ = type;
//			}
            void setEdge(std::shared_ptr<Edge> connectingEdge){
                edge_ = connectingEdge;
            }
            void setIndex(unsigned int idx){
				idx_ = idx;
			}
			void setPolygon(std::shared_ptr<Polygon> polygon){
				polygon_ = polygon;
			}

		public:
		
			// disable copy/move/default
            Vertex() = delete;
            Vertex(const Vertex& vert) = delete;
            Vertex(Vertex&& vert) = delete;
            Vertex& operator = (const Vertex& vert) = delete;
            Vertex& operator = (Vertex&& vert) = delete;
            
            //Constructor for the vertex, where we define its coords and types.
            Vertex(float x, float y);
			~Vertex() = default;
        
			//Setter and Getter Functions
            unsigned int getIndex() const{
				return idx_;
			}
//			VertexType getType(){
//				return type_;
//			}
			inline float getX() const{
				return pt_->getX();
			}
			inline float getY() const{
				return pt_->getY();
			}
            inline const std::shared_ptr<Edge> getEdge() const{
                return edge_;
            }
            void setPoint(std::shared_ptr<Point> vertexPt){
                pt_ = vertexPt;
            }
            const std::shared_ptr<Point> getPoint() const{
                return pt_;
            }
            
//            std::vector<std::shared_ptr<Vertex> >getVertexList(){
//                return vertexVector_;
//            }
//            /** Creates a vector of all events which are the vertices of the polygon
//             *@param xCoord - the horizontal coordinate of the vertex
//             *@param yCoord - the vertical  coordinate of the vertex
//             *@return a shared pointer to the vertex that was created from the coordinates
//             */
//            std::shared_ptr<Vertex> makeNewVertexPtr(float xCoord,float yCoord);
//            /** Creates a vector of all events which are the vertices of the polygon
//             *@param xCoord - the horizontal coordinate of the vertex
//             *@param yCoord - the vertical  coordinate of the vertex
//             *@return a reference to the vertex that was created from the coordinates
//             */
//            Vertex& makeNewVertex(float xCoord,float yCoord);
//            static void render(VertexType type);
    };
}

#endif /* Vertex_hpp */
