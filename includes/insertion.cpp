#include "insertion.h"


// Traverse through the dataPageFile
int traversal(std::fstream* mainFile,bool useCurrentMin){
    return 0;
}


void insert(std::vector<unsigned char> inputtedRow, std::string fileName, bool useCurrentMin){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary);
    if(!mainFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    int address = traversal(&mainFile, useCurrentMin);
}