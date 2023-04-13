//
//  Geometry.hpp

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

namespace geometry{

	/**	In Java, this would be an interface, not for the abstract
	 *	functions but for the application-wide constants.
	 */
	class Geometry{
		public:
			/**
			 *	Absolute tolerance for distance computations.  For any computed distance less
			 *	than the tolerance, we consider that points coincide, a point lies on a
			 *	segment, etc.
			 *	Set in the main program.
			 */
			static const float DISTANCE_ABS_TOL;

			/**
			 *	Relative tolerance for distance computations.  For any computed distance less
			 *	than the tolerance, we consider that points coincide, a point lies on a
			 *	segment, etc.
			 *	Set in the main program.
			 */
			static const float DISTANCE_REL_TOL;

			/**	Absolute tolerance for squared distance computations.  For any computed squared
			 *	distance less than the tolerance, we consider that points coincide,
			 *	a point lies on a segment, etc.
			 *	Set in the main program.
			 */
			static const float DISTANCE_ABS_SQ_TOL;

			/**	Relative tolerance for squared distance computations.  For any computed squared
			 *	distance less than the tolerance, we consider that points coincide,
			 *	a point lies on a segment, etc.
			 *	Set in the main program.
			 */
			static const float DISTANCE_REL_SQ_TOL;

			//	disable all constructors
			Geometry() = delete;
			Geometry(const Geometry&) = delete;
			Geometry(Geometry&&) = delete;

	};
}

#endif //	GEOMETRY_HPP
