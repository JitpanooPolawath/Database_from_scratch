#pragma once

/*
Rewrite dataaPage.cpp due to the lack of flexibility it have.
I also have a more concrete plan of how I want to access the information
----
This .h file will read and write the data into the .mdf file. It will also
append data when needed such as, during adding dataPage and Intermediate page.
*/


#include <string>
#include <vector>
#include "datapage.h"

# define MAXDEPTH 2

struct pHeader{
    int row;
    int bytesLeft;
    bool isFull;
    int curAddr;
    uint32_t minNum;
    int curID;
    int prevAddr;
    int nextAddr;
};

struct doubleAddr{
    int prevAddr;
    int curAddr;
};

struct minimus{
    int mini;
    int addr;
};

void getHeader(std::vector<char> header, std::fstream* mainFile, pHeader* curHead,int curAddr);
/* 
Given an fstream, it will traverse through the file to find the address of the
datapage. Using pageheader to iterate through it.
*/
doubleAddr traversal(std::fstream mainFile, bool useCurrentMin, uint32_t minimum ,int curAddr);

void insert(std::vector<unsigned char> inputtedRow, std::string fileName ,bool useCurrentMin);