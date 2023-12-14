//
//  dataFileIO.hpp
//  Point Input
//
//  Created by Jean-Yves Hervé on 2023-02-23.
//  Copyright © 2023 URI 3D Group. All rights reserved.
//

#ifndef dataFileIO_hpp
#define dataFileIO_hpp

#include <string>

void readDataFile(const std::string& filePath, int& paneWidth, int& paneHeight);
std::string writeDataFile(const std::string& fileRootPath, const std::string& comment="");

#endif /* dataFileIO_hpp */
