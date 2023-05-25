//
//  Geometry.hpp

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

//-----------------------------------------------------------------------
//  Determines which OpenGL & glut header files to load, based on
//  the development platform and target (OS & compiler)
//-----------------------------------------------------------------------

//  Windows platform
#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || (defined( __MWERKS__) && __INTEL__))
    //  Visual
    #if defined(_MSC_VER)
        #include <GL\gl.h>
        #include <GL\glut.h>
    //  gcc-based compiler
    #elif defined(__CYGWIN__) || defined(__MINGW32__)
        #include <GL/gl.h>
        #include <GL/glut.h>
    #elif (defined( __MWERKS__) && __INTEL__))
    #endif

//  Linux and Unix
#elif  (defined(__FreeBSD__) || defined(linux) || defined(sgi) || defined(__NetBSD__) || defined(__OpenBSD) || defined(__QNX__))
    #include <GL/gl.h>
    #include <GL/glut.h>

//  Macintosh
#else
    #if defined(__MWERKS__)
        #include <gl.h>
        #include <glut.h>
    #endif

    //  gcc-based compiler (Xcode, Eclipse, Code::Blocks)
    #if defined(__GNUC__)
        #include <OpenGL/gl.h>
        #include <GLUT/glut.h>
    #endif
#endif

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
