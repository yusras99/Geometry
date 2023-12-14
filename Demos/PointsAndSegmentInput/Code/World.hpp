#ifndef WORLD_HPP
#define WORLD_HPP

#include <cmath>
#include "Point.hpp"

/** Essentially a set of application-wide global variables defining the
 * dimensions of the "world" and the conversion factors from pixel to world
 * units and back, as well as a few rendering constants.
 *
 */
struct World {

	/**	Minimum x value to get mapped to the display area.
	 *	Set in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float X_MIN;

	/**	Maximum x value to get mapped to the display area.
	 *	Set in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float X_MAX;

	/**	Minimum y value to get mapped to the display area.
	 *	Set in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float Y_MIN;

	/**	Maximum x value to get mapped to the display area.
	 *	Set in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float Y_MAX;

	/**	Calculated as X_MAX-X_MIN in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float WORLD_WIDTH;

	/**	Calculated as Y_MAX-Y_MIN in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float WORLD_HEIGHT;

	/**	Scaling factor converting pixel units to world units.
	 *	Calculated in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float PIXEL_TO_WORLD;

	/**	Scaling factor converting world units to pixel units.
	 *	Calculated in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float WORLD_TO_PIXEL;
	
	/**	Radius (in pixel units) of the disk used to represent a point.
	 *	Set in the main program.
	 */
	static const float POINT_PIXEL_RADIUS;

	/**	Radius (in world units) of the disk used to represent a point.
	 *	Calculated in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float POINT_DISK_RADIUS;

	/**	When in snap-to-direction mode, degree range within which snap is enforced.
	 *	Set in the main program.
	 */
	static float SNAP_TO_ANGLE_TOL;

	/**	When in snap-to-point mode, distance (in world units) within which snap
	 *	is enforced.
	 *	Calculated in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float SNAP_TO_POINT_TOL;

	/**	When in snap-to-segment mode, distance (in world units) within which snap
	 *	is enforced.
	 *	Calculated in the main program by a call to setWorldBounds.
	 *	@see setWorldBounds
	 */
	static float SNAP_TO_SEGMENT_TOL;

	/**	Tolerance for distance computations.  For any computed distance less
	 *	than the tolerance, we consider that points coincide, a point lies on a
	 *	segment, etc.
	 *	Set in the main program.
	 */
	static const float DISTANCE_TOL;

	/**	Tolerance for squared distance computations.  For any computed squared
	 *	distance less than the tolerance, we consider that points coincide,
	 *	a point lies on a segment, etc.
	 *	Set in the main program.
	 */
	static const float DISTANCE_SQ_TOL;


	/** Function called through the initialization of a global variable in the
	 *	main program.  Although the user specifies dimensions for the rendering pane,
	 *	the function may set different values that agree better with the world
	 *	aspect ratio.
	 * @param xmin	Minimum x value to get mapped to the display area.
	 * @param xmax	Maximum x value to get mapped to the display area.
	 * @param ymin	Minimum y value to get mapped to the display area.
	 * @param ymax	Maximum y value to get mapped to the display area.
	 * @param paneWidth		user-set width of the redering pane
	 * @param paneHeight	user-set height of the redering pane
	 * */
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

		POINT_DISK_RADIUS = POINT_PIXEL_RADIUS * PIXEL_TO_WORLD;
		geometry::Point::setPointDiskRadius(POINT_DISK_RADIUS);
		
		// snap stuff
		SNAP_TO_POINT_TOL = 1.5f*POINT_DISK_RADIUS;
		SNAP_TO_SEGMENT_TOL = 1.5f*POINT_DISK_RADIUS;
	}
	

};

/**	Free function to render the label of a point or segment
 *	@param prefix the string prefix of the label (e.g. "P_ or "S_")
 *	@param index	the index of the opeject to render the label of
 *	@param xPos		x coordinate of where to render the label
 *	@param yPos		y coordinate of where to render the label
 *	@param isLarge	whether to use large or small font for the rendering
*/
void renderLabel(const std::string& prefix, unsigned int index, int xPos, int yPos, bool isLarge);


#endif  //  WORLD_HPP
