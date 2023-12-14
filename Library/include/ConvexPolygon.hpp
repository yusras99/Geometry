//
//  ConvexPolygon.hpp
//  Polygon-Source
//
//  Created by Jean-Yves Hervé on 2023-07-04.
//

#ifndef ConvexPolygon_hpp
#define ConvexPolygon_hpp

#include "Polygon.hpp"
namespace geometry{

	class ConvexPolygon : public Polygon{
	
		virtual bool isInside(float x, float y) const;
	
//		void render() const;

	};
}

#endif /* ConvexPolygon_hpp */
