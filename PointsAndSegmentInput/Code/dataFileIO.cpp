//
//  dataFileIO.cpp
//  Point Input
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <limits>
#include <vector>
#include "dataFileIO.hpp"
#include "World.hpp"
#include "Point.hpp"
#include "Segment.hpp"

using namespace std;
using namespace geometry;

string getNextDataLine(ifstream& inFile);
float getWorldDimension(const string& line, const string& label);

static const unsigned long NOT_FOUND = numeric_limits<string::size_type>::max();


void readDataFile(const string& filePath){

    ifstream inFile(filePath.c_str());
    if (!inFile.is_open()){
        cout << "File not found: " << filePath << endl;
        exit(7);
    }
    
    string line;
    
    //-----------------------------------------------------
    //    Section 1:    World Bounds
    //-----------------------------------------------------
    float xmin, xmax, ymin, ymax;
    line = getNextDataLine(inFile);
    xmin = getWorldDimension(line, "XMIN");
    line = getNextDataLine(inFile);
    xmax = getWorldDimension(line, "XMAX");
    line = getNextDataLine(inFile);
    ymin = getWorldDimension(line, "YMIN");
    line = getNextDataLine(inFile);
    ymax = getWorldDimension(line, "YMAX");
    World::setWorldBounds(xmin, xmax, ymin, ymax);
    
    //-----------------------------------------------------
    //    Section 2:    Point list
    //-----------------------------------------------------
    
    //    We are going to need a local copy of the global point list, to be able to
    //    access them by index when we create semgents.
    vector<shared_ptr<Point> > pointList;

    bool readingPointData = true;
    while (readingPointData){
        //    We get the next line of data (so, not a blank line and not a comment
        //    line, so the first non-blank character will be "meaningful"
        line  = getNextDataLine(inFile);
        
        //    if the first non-blank character is anything other than 'p' or 'v',
        //    then we are done with point data.
        char c = line[line.find_first_not_of(' ')];
        if ((c == 'p') || (c == 'v')){
            //    read the point's coordinates
            istringstream iSStr(line);
            string word;
            float x, y;
            iSStr >> word >> x >> y;
            if ((word == "v") || (word == "p")){
                //    Create the point
                pointList.push_back(Point::makeNewPointPtr(x, y));
            }else{
                cout << "Invalid Point coordinates format line: " << line << endl;
                cout << "\tExpected format: v|p" <<  " <float value> <float value>" << endl;
                exit(8);
            }
        }else{
            readingPointData = false;
        }
    }
    
    //-----------------------------------------------------
    //    Section 3:    Segment list
    //-----------------------------------------------------
    //    We came out of the Point section with a non-blank, non-comment line
    //    that hasn't been processed yet.
    bool readingSegmentData = true;
    do{
        
        //    if the first non-blank character is anything other than 'p' or 'v',
        //    then we are done with point data.
        char c = line[line.find_first_not_of(' ')];
        if (c == 's') {
            //    read the segment's endpoints' indices
            istringstream iSStr(line);
            string word;
            size_t index1, index2;
            iSStr >> word >> index1 >> index2;
            if ((word == "s")){
                //    Create the point
                Segment::makeNewSeg(*(pointList[index1]), *(pointList[index2]));
            }else{
                cout << "Invalid Segment format line: " << line << endl;
                cout << "\tExpected format: s  <point index 1> <point index 2>" << endl;
                exit(8);
            }
            //    As soon as we encounter a non-blank line that doesn’t define a segment, this
            //    section is over.
        }else{
            readingSegmentData = false;
        }
    } while (readingSegmentData && getline(inFile, line));

    //-----------------------------------------------------
    //    Section 4:    We're done
    //-----------------------------------------------------
    //    At this moment, the file format only defines points and segments
    inFile.close();
    
    //    really just to be nice
    pointList.clear();
}

void writeDataFile(const string& rootFilePath){

    cout << "Not open for business yet" << endl;
    exit(6);
}

string getNextDataLine(ifstream& inFile){
    bool stilSearching = true;
    string line;
    while (stilSearching && getline(inFile, line)) {
        //    Find the first non-space character
        string::size_type pos = line.find_first_not_of(' ');
        
        // If the line contains any non-space chars
        if (pos != NOT_FOUND){
            //    Skip if the first no-blank character indicates a comment line
            if (line[pos] != '#'){
                stilSearching = false;
            }
        }
    }
    //    if we were still searching when we left the loop, it's because
    //    we reached the end of the file --> return an empty string
    if (stilSearching) {
        line = "";
    }
    
    return line;
}

float getWorldDimension(const string& line, const string& label){
    istringstream iSStr(line);
    string firstWord, eqStr;
    float val;
    iSStr >> firstWord >> eqStr >> val;
    if ((firstWord != label) || (eqStr != "=")){
        cout << "Invalid World Bound format line: " << line << endl;
        cout << "\tExpected format: " << label << " = <float value>" << endl;
        exit(9);
    }
    
    //    If I was thorough, I would check the rest of the line to make sure that
    //    there is nothing other than comments or blank characters after the float
    //    value
    return val;
}
