/*----------------------------------------------------------------------------------+
 |                                                                                    |
 |    A Simple global header to check the development platform and load the proper    |
 |    OpenGL, glu, and glut headers.                                                    |
 |    Supports Mac OS X, Windows, Linux,                                                |
 |             and Xcode, gcc, Visual Studio, MinGW                                    |
 |                                                                                    |
 |    Author:        Jean-Yves Hervé,    University of Rhode Island                        |
 |                                    Dept. of Computer Science and Statistics        |
 |                                    3D Group for Interactive Visualization            |
 |    Fall 2011, Rev. Fall 2022                                                        |
 +---------------------------------------------------------------------------------*/
#ifndef GL_PLATFORM_H
#define GL_PLATFORM_H

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

#endif    //    GL_PLATFORM_H
