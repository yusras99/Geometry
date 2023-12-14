//
//  Edge.hpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/28/23.
//

#ifndef Edge_hpp
#define Edge_hpp

#include <stdio.h>
#include <memory>
#include "Segment.hpp"
#include "Vertex.hpp"

namespace geometry{
	class Edge : public Segment{
		
		private:
			unsigned int idx_;
            
			
		public:
		
			// delete copy/move/default
            Edge() = delete;
            Edge(const Edge& copyEdge) = delete;
            Edge(Edge&& copyEdge) = delete;
            Edge& operator = (const Edge& copyEdge) = delete;
            Edge& operator = (Edge&& copyEdge) = delete;
        
            Edge(std::shared_ptr<Vertex> v1, std::shared_ptr<Vertex> v2, unsigned int idx);
			
			~Edge() = default;
            static void render_(const Vertex& v1, const Vertex& v2);
            
            unsigned int getIndex() const{
				return idx_;
			}
	};
}
#endif /* Edge_hpp */
