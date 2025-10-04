#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <string>
#include <list>
#include <chrono>
#include <limits>
#include <typeinfo>

#include "component.h"

#define EMPTYHEADER 75
#define columnSize 30
#define INDAOFFSET 11

struct columnType{
    bool isChar;
    // Length of char
    uint8_t charLength;
};

struct returnHeader {int parentRow; int parentBytes; uint32_t parentAddress;};

class datapage {
private:
    // page header (96 bytes)
    uint8_t numRow;      // (1 byte)
    uint16_t bytesLeft;   // (2 bytes)
    uint8_t isFull; // (1 byte)
    // 92 bytes left
    char pageHeaderBytes[EMPTYHEADER] = {0};

    // data rows
    char dataRows[8096] = {0};

    std::string fileName;
    std::string logFileName;
    std::fstream datapageFile;
    std::fstream logFile;
    std::string logTimeName;
    std::fstream logTimeFile;

public:
    datapage(std::string tableName);
    returnHeader getHeader(int startingAddress, std::string parentName);
    void updateHeader(int startingAdress, int parentRow, int parentBytes);
    void createRoot();
    void createIntermediate();
    void createDataPage();
    
    // Create & update log
    void openLog(bool isLogTime);
    void closeLog(bool isLogTime);
    void setLogColumnCount(uint8_t count, uint16_t totalBytes, uint8_t colKey);
    void setLogFile(char* name, columnType types);
    void setLogTimestamp(uint8_t isValue);
};
