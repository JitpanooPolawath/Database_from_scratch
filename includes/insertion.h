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
    uint8_t row;
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

struct cHeader{
    uint32_t tempRow;
    uint8_t columnCount;
    uint16_t totalBytes;
    uint16_t keyBytes;
};


void getConfig(std::vector<char> header, std::fstream* conFile, cHeader* curHead);

void updateHeader(std::fstream* mainFile ,int curAddr, uint16_t updatedBytes, uint8_t rowCount, uint32_t minimum);

void updateLogTimestamp(uint8_t isValue, std::fstream* logTimeFile);

void updateConfig(uint32_t numRow, std::fstream* logFile);

void getHeader(std::vector<char> header, std::fstream* mainFile, pHeader* curHead,int curAddr);
/* 
Given an fstream, it will traverse through the file to find the address of the
datapage. Using pageheader to iterate through it.
*/
doubleAddr traversal(std::fstream* mainFile, int depth, uint32_t minimum ,int curAddr);

void insert(std::vector<unsigned char> inputtedRow, std::string fileName, int depth, uint32_t minimum, int parentAddr);