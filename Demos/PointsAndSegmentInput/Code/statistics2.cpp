//
//  statistics.cpp
//  Statistics
//
//  Created by Jean-Yves Hervé on 2023-06-28.
//	This program compute comparative statistics for the "smart" algorithm compared
//	to the "brute force" one, for multiple datasets of random segments.

#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <utility>
#include <memory>
#include <chrono>
#include <sstream>
#include "World.hpp"
#include "Point.hpp"
#include "Segment.hpp"
#include "IntersectionQuery.hpp"
#include "dataFileIO.hpp"

using namespace std;
using namespace geometry;

const float Geometry::DISTANCE_ABS_TOL = 3E-4f;
const float Geometry::DISTANCE_REL_TOL = 1E-6f;
const float Geometry::DISTANCE_ABS_SQ_TOL = 1E-6f;
const float Geometry::DISTANCE_REL_SQ_TOL = 1E-7f;
//
float World::X_MIN = -100;
float World::X_MAX = +100;
float World::Y_MIN = -100;
float World::Y_MAX = +100;
float World::WORLD_WIDTH = World::X_MAX - World::X_MIN;
float World::WORLD_HEIGHT = World::Y_MAX - World::Y_MIN;
float World::POINT_DISK_RADIUS;
float World::PIXEL_TO_WORLD;
float World::WORLD_TO_PIXEL;
float World::SNAP_TO_ANGLE_TOL;
float World::SNAP_TO_POINT_TOL;
float World::SNAP_TO_SEGMENT_TOL;
const float World::POINT_PIXEL_RADIUS = 5.f;

vector<shared_ptr<Segment> > generateRandomSegmentList(size_t);

random_device randDev;
default_random_engine engine(randDev());
uniform_real_distribution<float> xCoordinateDist(World::X_MIN, World::X_MAX);
uniform_real_distribution<float> yCoordinateDist(World::Y_MIN, World::Y_MAX);

const int NUM_SEGS = 10;
const int NUM_EXAMPLES = 5;

int main(int argc, char* argv[]){

	//	I am going to use these vectors to record execution time for my data sets.
	//	xyzResults[size index][run index] --> a pair <number of intersections, computaton time in µs>
	vector< vector< pair<int,long > > > bruteForceResults, smartAlgResults;

	for (int k=0; k<NUM_EXAMPLES; ){
		vector<shared_ptr<Segment> > segList = generateRandomSegmentList(NUM_SEGS);
		
		IntersectionQuery query(segList);
			
		auto intersectionPointList = query.findAllIntersectionsBruteForce();
		size_t refSize = intersectionPointList.size();
		intersectionPointList.clear();
		intersectionPointList = query.findAllIntersectionsSmart();

		//	If we got one...
		if (intersectionPointList.size() != refSize){
			k++;
			ostringstream outStream;
			outStream << "#  Brute force intersections: " << refSize << endl;
			outStream << "#  Smart algo, intersections: " << intersectionPointList.size() << endl;
			
			// Record the configuration
			writeDataFile("../../../../DataFiles/problemConfig", outStream.str());
		}

		//	Before the next iteration, delete all points and segments
		Point::clearAllPoints();
		Segment::clearAllSegments();
	}

	return 0;
}


vector<shared_ptr<Segment> > generateRandomSegmentList(size_t numSegs){
	vector<shared_ptr<Segment> > segList(numSegs);

	for (size_t k=0; k<numSegs; k++)
		segList[k] = Segment::makeNewSegPtr(Point::makeNewPointPtr(xCoordinateDist(engine), xCoordinateDist(engine)),
										    Point::makeNewPointPtr(xCoordinateDist(engine), xCoordinateDist(engine)));

	return segList;
}

