#include "selection.h"

void seTraversal(std::vector<operValue>* clauses, std::vector<int>* columns, std::fstream* mainFile, 
    std::fstream* configFile, int keyColumn, bool isAllCol, bool isAllWhere){
    const size_t pageHeader = 96;
    const size_t cHeaderSize = 9;
    std::vector<char> cBuffer(cHeaderSize);
    cHeader confHeader;
    configFile->seekg(0,std::ios::beg);
    getConfig(cBuffer,configFile,&confHeader);
    if(isAllWhere){
        int curAddr = 8192+8192;
        uint32_t count = 0;
        int pageCount = 0;
        while(1){
            std::vector<char> bufferP(pageHeader);
            pHeader curHead;
            getHeader(bufferP, mainFile, &curHead, curAddr);
            mainFile->seekg(curAddr+96,std::ios::beg);
            for(int i = 0; i<curHead.row;i++){

                count++;
            }
            if(count == confHeader.tempRow){
                break;
            }
            pageCount++;
            if(count%(INDAOFFSET-1) == 0){
                curAddr += 8192;
            }
            curAddr+=8192;
        }
        
    }
    

}

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
    std::vector<operValue> opArr;
    opArr.reserve(columnCount);
    // Selecting which column to show
    std::cout << "====== Select column ======" << std::endl; 
    int countBytes = 0;
    bool isAllCol = true;
    bool isAllWhere = true;
    std::cout <<"Select all columns [y] or specific [n]: ";
    std::string stopIN;
    std::cin >> stopIN;
    std::vector<int> columnArr;
    columnArr.reserve(columnCount);
    if(stopIN.compare("n")== 0){
        for(int i = 0; i <  static_cast<int>(columnCount); i++){
            char columnName[30];
            bool isChar;
            uint8_t columnBytes;
            logFile.read(columnName, 30);
            logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
            logFile.read(reinterpret_cast<char*>(&columnBytes),1);
            countBytes += columnBytes;   
            std::cout <<"Do you want to select [" << columnName <<"], [y] or [n]: ";
            std::cin >> stopIN;
            if(stopIN.compare("y")== 0){
                isAllCol = false;
                if(countBytes == 0){
                    columnArr.push_back(4);
                }else{
                    columnArr.push_back(countBytes);
                }
            }else{
                columnArr.push_back(-1);
            }
        }
    }

    // where clause
    int keyColumn = -1;
    int curBytes = 0;
    std::cout << "====== Where column ======" << std::endl; 
    logFile.seekg(9,std::ios::beg);
    std::cout <<"Select all columns [y] or specific [n]: ";
    std::cin >> stopIN;
    if(stopIN.compare("n") == 0){
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
            std::cin >> stopIN;
            operValue temp;
            std::string result = isChar ? "Yes" : "No";
            std::cout <<"Is this column char: [" << result <<"]"<<std::endl;
            if(stopIN.compare("y")== 0){
                std::cout <<"Pick an operation [=,0],[!=,1],[<,2][>,3][<=,4][>=,5][!<,6][!>,7][!<=,8][!>=,9]: ";
                int oper;
                std::cin >> oper;
                if(std::cin.fail() || (oper > 10 || oper < 0)){
                    std::cout << "Failed to pick correct where clause, will skip this column." << std::endl; 
                    temp.operType= -1;
                    opArr.push_back(temp);
                }else{
                    isAllWhere = false;
                    if(isChar){
                        std::string inputVal;
                        std::cout << "Input condition value: ";
                        std::cin >> inputVal; 
                        temp.strComp = inputVal;
                        temp.isChar = true;
                    }else{
                        int inputVal;
                        std::cout << "Input condition value: ";
                        std::cin >> inputVal; 
                        temp.numComp = inputVal;
                        temp.isChar = false;
                    }

                    if(curBytes == colKey){
                        keyColumn = j;
                    }
                    temp.operType = oper;
                    opArr.push_back(temp);
                }
            }else{
                temp.operType = -1;
            }
        }
    }

    if(isAllWhere){
        seTraversal(&opArr,&columnArr,&mainFile,&logFile,keyColumn,isAllCol,isAllWhere);
    }


    for(int k = 0; k < static_cast<int>(columnCount); k++){
        std::cout << columnArr[k] <<"==="<< opArr[k].operType<<std::endl; 
    }
}