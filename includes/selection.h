#pragma once

#include <string>
#include <vector>
#include "insertion.h"
#include "readInput.h"



// Operation value
// [=,0],[!=,1],[<,2][>,3][<=,4][>=,5][!<,6][!>,7][!<=,8][!>=,9]

struct operValue{
    int operType;
    std::string strComp;
    int numComp;
    bool isChar;
};
struct colValue{
    uint8_t size;
    std::string strComp;
};

void seTraversal(std::vector<operValue>* clauses, std::vector<int>* columns, std::fstream* mainFile, 
    std::fstream* configFile, int keyColumn, bool isAllCol, bool isAllWhere);

void selection(std::string fileName);