#include "selection.h"


void selection(std::string fileName){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    std::string lFileName = fileName + "_config" +".ldf";
    std::fstream logFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary);
    logFile.open(lFileName, std::ios::in | std::ios::binary);
    if(!mainFile.is_open() || !logFile.is_open()){
        std::cout << "File open error at selection" << std::endl;
        exit(1);
    }
    uint32_t numOfRow;
    logFile.read(reinterpret_cast<char*>(&numOfRow),sizeof(numOfRow));
    uint8_t columnCount;
    logFile.read(reinterpret_cast<char*>(&columnCount),sizeof(columnCount));
    uint16_t totalBytes;
    logFile.read(reinterpret_cast<char*>(&totalBytes),sizeof(totalBytes));
    uint16_t colKey;
    logFile.read(reinterpret_cast<char*>(&colKey),sizeof(colKey));
    std::cout <<"\n"<<"====== Config details ======" << std::endl; 
    std::cout << "number of rows: " << numOfRow << std::endl;
    std::cout << "column count: " << static_cast<int>(columnCount) << std::endl;
    std::cout << "totalbytes: " << totalBytes-1 << std::endl;
    std::cout << "KeyBytes: " << colKey << std::endl;
    std::vector<int> opArr;
    opArr.reserve(columnCount);
    std::vector<int> columnArr;
    columnArr.reserve(columnCount);
    // Selecting which column to show
    std::cout << "====== Select column ======" << std::endl; 
    int countBytes = 0;
    for(int i = 0; i <  static_cast<int>(columnCount); i++){
        char columnName[30];
        bool isChar;
        uint8_t columnBytes;
        logFile.read(columnName, 30);
        logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
        logFile.read(reinterpret_cast<char*>(&columnBytes),1);
        countBytes += columnBytes;   
        std::cout <<"Do you want to select [" << columnName <<"], [y] or [n]: ";
        std::string stopIN;
        std::cin >> stopIN;
        if(stopIN.compare("y")== 0){
            if(countBytes == 0){
                columnArr.push_back(4);
            }else{
                columnArr.push_back(countBytes);
            }
        }else{
            columnArr.push_back(-1);
        }
        
    }

    // where clause
    int keyColumn = 0;
    int curBytes = 0;
    std::cout << "====== Where column ======" << std::endl; 
    logFile.seekg(9,std::ios::beg);
    for(int j = 0; j <  static_cast<int>(columnCount); j++){
        char columnName[30];
        bool isChar;
        uint8_t columnBytes;
        logFile.read(columnName, 30);
        logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
        logFile.read(reinterpret_cast<char*>(&columnBytes),1);
        if(columnBytes == 0){
            curBytes += 4;
        }else{
            curBytes += columnBytes;
        }
        std::cout <<"Do you want to select [" << columnName <<"], [y] or [n]: ";
        std::string stopIN;
        std::cin >> stopIN;
        if(stopIN.compare("y")== 0){
            std::cout <<"Pick an operation [=,0],[!=,1],[<,2][>,3][<=,4][>=,5][!<,6][!>,7][!<=,8][!>=,9]: ";
            int oper;
            std::cin >> oper;
            if(std::cin.fail() || (oper > 10 || oper < 0)){
                std::cout << "Failed to pick correct where clause, will skip this column." << std::endl; 
                opArr.push_back(-1);
            }else{
                if(curBytes == colKey){
                    keyColumn = j;
                }
                opArr.push_back(oper);
            }
        }else{
            opArr.push_back(-1);
        }
    }
    for(int k = 0; k < static_cast<int>(columnCount); k++){
        std::cout << columnArr[k] <<"==="<< opArr[k]<<std::endl; 
    }
}