#include "update.h"



void updateRow(std::string fileName, isReadFile readingFil){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    std::string lFileName = fileName + "_config" +".ldf";
    std::fstream logFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary|std::ios::out);
    logFile.open(lFileName, std::ios::in | std::ios::binary|std::ios::out);
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
    if(numOfRow == 0){
        std::cout<<"This table have no data"<<std::endl;
        return;
    }
    std::cout <<"\n"<<"====== Config details ======" << std::endl; 
    std::cout << "number of rows: " << numOfRow << std::endl;
    std::cout << "column count: " << static_cast<int>(columnCount) << std::endl;
    std::cout << "totalbytes: " << totalBytes-1 << std::endl;
    std::cout << "KeyBytes: " << colKey << std::endl;
    std::vector<operValue> opArr;
    opArr.reserve(columnCount);
    std::cout << "====== Update column ======" << std::endl; 
    int countBytes = 0;
    bool isAllWhere = true;
    std::vector<colValue> columnArr;
    columnArr.reserve(columnCount);
    uint8_t prev = 0;
    logFile.seekg(9,std::ios::beg);
    bool isAllCol = true;
    int keyColumn = -1;
    int curBytes = 0;
    for(int i = 0; i <  static_cast<int>(columnCount); i++){
        char columnName[30];
        bool isChar;
        uint8_t columnBytes;
        logFile.read(columnName, 30);
        logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
        logFile.read(reinterpret_cast<char*>(&columnBytes),1);
        if(columnBytes == static_cast<uint8_t>(0)){
            countBytes += 4;
        }else{
            countBytes += columnBytes;
        }
        std::string colIN = "n";
        colValue temp;
        temp.strComp = columnName;
        temp.isShow = false;
        bool isPrim = true;
        if(colKey != countBytes){
            std::cout <<"Do you want to select [" << columnName <<"], [y] or [n]: ";
            std::cin >> colIN;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            isPrim = false;
        }
        isAllCol = false;
        if(colIN.compare("y")== 0 && !isPrim){
            if(isChar){
                std::string updateString;
                std::cout <<"Input string value: ";
                std::getline(std::cin,updateString);
                std::cout<<updateString<<std::endl;
                temp.strValue = updateString;
            }else{
                int updateInt;
                std::cout <<"Input integer value: ";
                std::cin >> updateInt;
                temp.intValue = updateInt;
            } 
            temp.isShow = true;
        }
        if(isChar){
            temp.size = columnBytes;
        }else{
            temp.size = 4;
        } 
        temp.prevAddr = prev;
        temp.curAddr = countBytes;
        temp.isChar = isChar;
        columnArr.push_back(temp);
        prev = countBytes;
    }
    std::cout << "====== Where column ======" << std::endl; 
    logFile.seekg(9,std::ios::beg);
    int whereCount = 0;
    std::string stopIN;
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
        std::string result = isChar ? "Yes" : "No";
        std::string primary = "NOT PRIMARY";
        if(curBytes == colKey){
            primary = "PRIMARY";
        }
        std::cout <<"Do you want to select [" << columnName <<"][ Is char: " << result <<"]["<<primary<<"], [y] or [n]: ";
        std::cin >> stopIN;
        operValue temp; 
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
                    temp.numComp = -1;
                    temp.isChar = true;

                }else{
                    int inputVal;
                    std::cout << "Input condition value: ";
                    std::cin >> inputVal; 
                    temp.numComp = inputVal;
                    temp.strComp = "";
                    temp.isChar = false;
                    if(curBytes == colKey){
                        keyColumn = whereCount;
                    }
                }
                whereCount++;
                temp.operType = oper;
                opArr.push_back(temp);
            }
        }else{
            temp.operType = -1;
            opArr.push_back(temp);
        }
    }
    seTraversal(opArr,columnArr,&mainFile,&logFile,keyColumn,isAllWhere, isAllCol, true);
    mainFile.close();
    logFile.close();
}