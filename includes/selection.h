#pragma once

#include <string>
#include <vector>
#include "insertion.h"
#include "readInput.h"



// Operation value
// [=,0],[!=,1],[<,2][>,3][<=,4][>=,5][!<,6][!>,7][!<=,8][!>=,9]
// reTraversal : 0,3,5,6,8 


struct operValue{
    int operType;
    std::string strComp;
    int numComp;
    bool isChar;
};
struct colValue{
    bool isShow;
    uint8_t size;
    std::string strValue;
    int intValue;
    std::string strComp;
    uint8_t prevAddr;
    uint8_t curAddr;
    bool isChar;
};

int reTraversal(std::fstream* mainFile,int depth, uint32_t minimum, int curAddr, int* parentAddr, int oper);

void seTraversal(std::vector<operValue> clauses, std::vector<colValue> columns, std::fstream* mainFile, 
    std::fstream* configFile, int keyColumn, bool isAllWhere, bool isAllCol, bool isUpdate);

void selection(std::string fileName, isReadFile readingFile);