//
//  Triangle.hpp
//  Polygon-Source
//
//  Created by Jean-Yves Hervé on 2023-07-04.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include <utility>
#include "ConvexPolygon.hpp"

namespace geometry{

	class Triangle : public ConvexPolygon{
		public:
			~Triangle() = default;
			
			bool isInside(float x, float y) const;

//			void render() const;
		
        private:



	};
}

#endif /* Triangle_hpp */
