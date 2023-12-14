//
//  Polygon.cpp
//  Polygon-Source
//
//  Created by Yusra Suhail on 6/28/23.
//
#include <set>
#include <map>
#include <vector>
#include "Polygon.hpp"

using namespace std;
using namespace geometry;

void handleStart();
void handleEnd();
void handleMerge();
void handleSplit();
void handleRegular();
void handleNoType();


Polygon::Polygon(const std::vector<std::shared_ptr<Vertex> > vertexList)
	:	vertexList_(vertexList){
	if (vertexList.size()<3)
        exit(0);
	/*if (!isValidVertexList(vertexList)){
        exit(0);
	}*/
	//	constructor that populates the edge list of the polygon
	//	Everything inside the polygon will be accessed by the index of the polygon
	size_t numVerts = vertexList.size();
    //(i+1)%n -> i+1 and (n)%n -> 0
    for (unsigned int i = 0; i< vertexList_.size(); i++){
		edgeList_.push_back(make_shared<Edge>(vertexList_[i],vertexList_[(i+1)%numVerts],i));
		vertexList[i]->setEdge(edgeList_[i]);
		vertexList[i]->setPolygon(shared_from_this());
    }
}

bool Polygon::isInside(float x, float y) const{
	return false;
}

enum class VertexType{
        START,
        END,
        MERGE,
        SPLIT,
        REGULAR,
        NO_TYPE
    };

//	A free function,that is a "file-level private function" by only declaring/defining it here.
VertexType findVertexType(const shared_ptr<Vertex> prevVert, const shared_ptr<Vertex> currVert, const shared_ptr<Vertex> nextVert, const vector<shared_ptr<Edge> > edgeList){
    // if previous and next vertex lie below currVert
    if( ( prevVert->getY() < currVert->getY() ) && (nextVert->getY() < currVert->getY() ) ){
        // if Interior Angle less than Pi, checked by determining if the there is a left turn or right
         // Left = less than pie, right = more then pie
        if(edgeList[prevVert ->getIndex()] ->isOnLeftSide(nextVert ->getPoint())){
            return VertexType::START;
        }
        else{
            return VertexType::SPLIT;
        }
    }else if( ( prevVert->getY() > currVert->getY() ) && (nextVert->getY() > currVert->getY() ) ){
        // Interior Angle less than Pi
        if(edgeList[prevVert -> getIndex()] -> isOnLeftSide(nextVert->getPoint() ) ){
            return VertexType::END;
        }else{
            return VertexType::MERGE;
        }
    }else{
        return VertexType::REGULAR;
    }
}

void partitionPolygon(const set<shared_ptr<Vertex>,compareVertEvent > vertexQueue, vector<shared_ptr<Polygon> > partList, vector<vector<shared_ptr<Vertex> > > connectionList ){
    while (!vertexQueue.empty()){
        //    get first element in queue --> start of a new poly
        shared_ptr<Vertex> currVertex;
        vector<shared_ptr<Vertex> > newPolygonVertList{currVertex};
        bool polygonCompleted = false;
        
        while(!polygonCompleted){
            // pick next vertex in the connections list of currentVertex
            newPolygonVertList.push_back(connectionList[currVertex->getIndex()][0]);
            
            // remove next vertex from current vertex connection list
            //        if list is empty, then remove current vertex from queue
            // if next vertex is not the first on poly list
            //        push next vertex on poly list
            //        current vertex = next vertex
            // else
            //        create polygon
            //        polygon completed = true

        }
    }
}

//    Only inside the partitioning function there should be a local variable that is a set

vector<shared_ptr<Polygon> > Polygon::monotonePartition() const{
    
    // The one or more polygons that will be created after the partition of the main polygon
    vector<shared_ptr<Polygon> > partList;
	//-------------
	//	First pass:
	//	Define the type of all vertices, and push them in the vertex set that is the vertexQueue
	vector<VertexType> vertTypeVector;
    set<shared_ptr<Vertex>,compareVertEvent > vertexQueue;
    
	shared_ptr<Vertex>  prevVert = vertexList_.back(),
						nextVert = vertexList_[1];
    //stores for each vertex the---possibly empty---list of diagonals(vertices), and other vertices attached to it
    //sorted according to counterclockwise wrapping order
    //list<shared_ptr<Vertex> >  connectionList;
    unordered_map<int ,pair<shared_ptr<Vertex>,set<shared_ptr<Vertex>,ConnectionComparator> > > connectionList;

    for (size_t i = 0; i< vertexList_.size(); i++){
        //Question, shouldn't this vector hold indices of vector too? because order of queue and vector will be different.
        VertexType currVertType = findVertexType( prevVert, vertexList_[i], nextVert,edgeList_);
        vertTypeVector.push_back(currVertType);
        
        vertexQueue.insert(vertexList_[i]);
        // if it is the last vertex then its next is the first vertex.
        if (i < vertexList_.size()-1){
            nextVert = vertexList_[i+1];
        }else{
            nextVert = vertexList_[0];
        }
        prevVert = vertexList_[i];
        auto it = connectionList.begin();
        std::advance(it, i);
        connectionList[prevVert] = {nextVert};
    }
    
    //-------------
    //    Second pass:
    //    Traverse the eventQueue to do the partition
	vector<shared_ptr<Vertex> > helper(vertexList_.size());
    
    //jyh
    //	vertEvennt really is set<shared_ptr<Vertex>,compareVertEvent >::iterator,
    // so you need to dereference it first to access the shared_patr<Segment>
    //
    for (auto vertEvent = vertexQueue.begin(); vertEvent != vertexQueue.end(); vertEvent++){
        //handle vertices and populate the connection list
        switch (vertTypeVector[(*vertEvent)->getIndex()]) {
            using enum VertexType;
            case START:
                handleStart();
                break;
            case END:
                handleEnd();
                break;
            case MERGE:
                handleMerge();
                break;
            case SPLIT:
                handleSplit();
                break;
            case REGULAR:
                handleRegular();
                break;
            case NO_TYPE:
                handleNoType;
                break;
                
            default:
                break;
        }
    }
    // Function to create polygons
    partitionPolygon(vertexQueue, partList,connectionList);


	return partList;
}

void handleStart(){}
void handleEnd(){}
void handleMerge(){}
void handleSplit(){}
void handleRegular(){}
void handleNoType(){}



vector<shared_ptr<ConvexPolygon> > Polygon::convexPartition() const{
	
	vector<shared_ptr<ConvexPolygon> >	partList;
	
	//-------------
	//	First pass:
	//	Define the type of all vertices
	vector<VertexType> vertexType;


	return partList;
}

vector<shared_ptr<Triangle> > Polygon::triangulation() const{

	vector<shared_ptr<Triangle> >	triangleList;

	//-------------
	//	First pass:
	//	Define the type of all vertices
	vector<VertexType> vertexType;


	return triangleList;
}

bool Polygon::isStaticVertexList(const std::vector<std::shared_ptr<Vertex> >){
	return true;
}
