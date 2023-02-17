#ifndef COMMON_H
#define COMMON_H

//jyh
//	Replaced by your class, but I still need a "lightweight data structure to store the
//	coordinates of a point before is has become a full-fledged Point, witn an index
//	and a shared_ptr to it.
struct PointStruct
{
	float x, y;
};

enum class ApplicationMode
{
	NO_MODE = -1,
	//
	POINT_CREATION,
	SEGMENT_CREATION,
	//
	NUM_MODES
};

enum class PointType
{
	SINGLE_POINT,
	ENDPOINT,
	FIRST_ENDPOINT,
	EDIT_POINT
};

enum class SegmentType
{
	SEGMENT,
	CREATED_SEGMENT,
	EDITED_SEGMENT
};

#endif  //  COMMON_H
