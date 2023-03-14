#ifndef COMMON_H
#define COMMON_H

#include <cmath>
#include "Point.hpp"

struct World {

	static float X_MIN;
	static float X_MAX;
	static float Y_MIN;
	static float Y_MAX;
	static float WORLD_WIDTH;
	static float WORLD_HEIGHT;
	static float PIXEL_TO_WORLD;
	static float WORLD_TO_PIXEL;
	static float POINT_DISK_RADIUS;
	static float SNAP_TO_ANGLE_TOL;
	static float SNAP_TO_POINT_TOL;
	static float SNAP_TO_SEGMENT_TOL;
	static const float POINT_PIXEL_RAD;
	//
	/**	Tolerance for distance computations.  For any computed distance less
	 *	than the tolerance, we consider that points coincide, a point lies on a
	 *	segment, etc.
	 */
	static const float DISTANCE_TOL;

	/**	Tolerance for squared distance computations.  For any computed squared
	 *	distance less than the tolerance, we consider that points coincide,
	 *	a point lies on a segment, etc.
	 */
	static const float DISTANCE_SQ_TOL;

	static void setWorldBounds(float xmin, float xmax, float ymin, float ymax,
								int& paneWidth, int& paneHeight){
		if ((xmax <= xmin) || (ymax <= xmin)){
			exit(5);
		}
		X_MIN = xmin;
		X_MAX = xmax;
		Y_MIN = ymin;
		Y_MAX = ymax;
		WORLD_WIDTH = xmax - xmin;
		WORLD_HEIGHT = ymax - ymin;
		
		float widthRatio = WORLD_WIDTH / paneWidth;
		float heightRatio = WORLD_HEIGHT / paneHeight;
		float maxRatio = fmax(widthRatio,heightRatio);
		/** If the two ratios differ by more than 5%,  then reject the dimensions*/
		if (fabsf(widthRatio-heightRatio)/maxRatio > 0.05){
			exit(15);
		}

		PIXEL_TO_WORLD = maxRatio;
		WORLD_TO_PIXEL = 1.f / PIXEL_TO_WORLD;
		
		paneWidth = static_cast<int>(round(WORLD_WIDTH * WORLD_TO_PIXEL));
		paneHeight = static_cast<int>(round(WORLD_HEIGHT * WORLD_TO_PIXEL));

		geometry::Point::setPointDiskRadius(POINT_PIXEL_RAD * PIXEL_TO_WORLD);

		// snap stuff
		SNAP_TO_POINT_TOL = 1.5f*POINT_DISK_RADIUS;
		SNAP_TO_SEGMENT_TOL = 1.5f*POINT_DISK_RADIUS;
	}
};


#endif  //  WORLD_H
