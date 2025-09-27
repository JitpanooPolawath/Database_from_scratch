#ifndef DATAPAGE_H
#define DATAPAGE_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <list>

struct returnHeader {int parentRow; int parentBytes;};

class datapage {
private:
    // page header (96 bytes)
    uint8_t numRow;      // (1 byte)
    uint16_t bytesLeft;   // (2 bytes)
    unsigned char isFull; // (1 byte)
    // 92 bytes left
    char pageHeaderBytes[92];

    // data rows
    char dataRows[8096] = {0};

    std::string fileName;
    std::string logFileName;
    std::fstream datapageFile;
    std::fstream logFile;
    

public:
    datapage(std::string tableName);
    returnHeader getHeader(int startingAddress, std::string parentName);
    void updateHeader(int startingAdress, int parentRow, int parentBytes);
    void createRoot();
    void createIntermediate();
    void createDataPage();
    void createLogFile();
};

#endif