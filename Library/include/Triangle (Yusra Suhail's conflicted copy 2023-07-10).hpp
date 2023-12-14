//
//  Triangle.hpp
//  Polygon-Source
//
//  Created by Jean-Yves Hervé on 2023-07-04.
//

#ifndef Triangle_hpp
#define Triangle_hpp

namespace geometry{

	class Triangle : public ConvexPolygon{
		public:
			~Triangle() = default;
            //pure virtual function 
			bool isInside(float x, float y) const;

			void render() const;
		
	private:
		unique_ptr<int> data;

	};
}

#endif /* Triangle_hpp */
