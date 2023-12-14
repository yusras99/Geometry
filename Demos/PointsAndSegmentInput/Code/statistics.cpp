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
#include "World.hpp"
#include "Point.hpp"
#include "Segment.hpp"
#include "IntersectionQuery.hpp"

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
void writeResultsToFile(const vector< vector< pair<int, long > > >& results,
						const string& fileName);

random_device randDev;
default_random_engine engine(randDev());
uniform_real_distribution<float> xCoordinateDist(World::X_MIN, World::X_MAX);
uniform_real_distribution<float> yCoordinateDist(World::Y_MIN, World::Y_MAX);

//	The plan is for increasing size N (10 to 1,000,000), repeat a dozen times
//
const vector<size_t> setSize{10, 20, 50,
						  100, 200, 500};/*,
						  1000, 2000, 5000};/*,
						  10000, 20000, 50000,
						  100000, 200000, 500000, 1000000};*/
//const vector<size_t> setSize{200, 10, 20};/*,
//						  10000, 20000, 50000,
//						  100000, 200000, 500000, 1000000};*/
const int runsPerSize = 12;

int main(int argc, char* argv[]){

	//	I am going to use these vectors to record execution time for my data sets.
	//	xyzResults[size index][run index] --> a pair <number of intersections, computaton time in µs>
	vector< vector< pair<int,long > > > bruteForceResults, smartAlgResults;

//auto start = chrono::high_resolution_clock::now();
//
//
//auto stop = chrono::high_resolution_clock::now();
//auto duration = duration_cast<chrono::microseconds>(stop - start);
//
//cout << duration.count() << endl;
	for (size_t sizeIndex = 0; sizeIndex < setSize.size(); sizeIndex++){
		vector< pair<int, long > > bruteForcePartialResults, smartAlgPartialResults;
		for (int k=0; k<runsPerSize; k++){
			vector<shared_ptr<Segment> > segList = generateRandomSegmentList(setSize[sizeIndex]);
		
			IntersectionQuery query(segList);
			
			auto start = chrono::high_resolution_clock::now();
			auto intersectionPointList = query.findAllIntersectionsBruteForce();
			auto stop = chrono::high_resolution_clock::now();
			auto duration = duration_cast<chrono::microseconds>(stop - start);
			long val = duration.count();
			bruteForcePartialResults.push_back(make_pair(intersectionPointList.size(), val));
			
			intersectionPointList.clear();
			start = chrono::high_resolution_clock::now();
			intersectionPointList = query.findAllIntersectionsSmart();
			stop = chrono::high_resolution_clock::now();
			duration = duration_cast<chrono::microseconds>(stop - start);
			val = duration.count();
			smartAlgPartialResults.push_back(make_pair(intersectionPointList.size(), val));
			intersectionPointList.clear();

			segList.clear();
		}
		
		bruteForceResults.push_back(bruteForcePartialResults);
		smartAlgResults.push_back(smartAlgPartialResults);
	}

	writeResultsToFile(bruteForceResults, "bruteResults.txt");
	writeResultsToFile(smartAlgResults, "smartResults.txt");

	return 0;
}


vector<shared_ptr<Segment> > generateRandomSegmentList(size_t numSegs){
	vector<shared_ptr<Segment> > segList(numSegs);

	for (size_t k=0; k<numSegs; k++)
		segList[k] = Segment::makeNewSegPtr(Point::makeNewPointPtr(xCoordinateDist(engine), xCoordinateDist(engine)),
										    Point::makeNewPointPtr(xCoordinateDist(engine), xCoordinateDist(engine)));

	return segList;
}

void writeResultsToFile(const vector< vector< pair<int, long > > >& results,
						const string& fileName){
	//	I export the data to be read and visualized in Mathematica. The simplest for
	//	me is to write a flat list of triplets
	//		<number segments, number intersection, computation time>
	ofstream outFile(fileName);
	for (size_t k=0; k< results.size(); k++){
		for (auto matchScore : results[k]){
			outFile << setSize[k] << " " << matchScore.first << " " <<
					   matchScore.second << " " << endl;
		}
	}

}
