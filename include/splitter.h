#ifndef SPLITTER_H
#define SPLITTER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Splitter {
private:
    const size_t numFiles;
    const std::string prefix;
    const std::string inputFile;
    const int level;
public:
    Splitter(size_t numFiles_, const std::string& prefix_, 
        const std::string& inputFile_, int level_=0) :
        numFiles(numFiles_),
        prefix(prefix_),
        inputFile(inputFile_),
        level(level_) {}
    
    int split();
};

#endif  // ~SPLITTER_H