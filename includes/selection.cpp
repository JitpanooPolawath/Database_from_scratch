#include "selection.h"



int convertOperator(std::string str) {
    if (str.compare("==") == 0) return 0;
    else if (str.compare("!=") == 0) return 1;
    else if (str.compare("<") == 0) return 2;
    else if (str.compare(">") == 0) return 3;
    else if (str.compare("<=") == 0) return 4;
    else if (str.compare(">=") == 0) return 5;
    else if (str.compare("!<") == 0) return 6;
    else if (str.compare("!>") == 0) return 7;
    else if (str.compare("!<=") == 0) return 8;
    else if (str.compare("!>=") == 0) return 9;
    else{
        return -1;
    }
}


int reTraversal(std::fstream* mainFile,int depth, uint32_t minimum, int curAddr, int* parentAddr, int oper){
    const size_t pageHeader = 96;
    std::vector<char> buffer(pageHeader);
    pHeader curHead;
    getHeader(buffer, mainFile, &curHead, curAddr);

    // Address less than current 
    int reAddress;
    if(depth >= 2){
        return curHead.curAddr;
    }

    // Start of the pageAddr + pageHeader, to get to the page body
    // for loop to iterate through the root and intermediate page - worst case O(n^2)
    mainFile->seekg(curAddr+96);
    uint32_t curPageVal; 
    uint32_t curPageAddr;
    uint32_t prevAddr;
    mainFile->read(reinterpret_cast<char*>(&curPageVal),sizeof(curPageVal));
    mainFile->read(reinterpret_cast<char*>(&curPageAddr),sizeof(curPageAddr));
    bool isFound = false;
    bool isLastCase = true;
    for(int i = 0; i < curHead.row; i++){
        if(depth == 1){
                *parentAddr = curAddr;
        }
        if(i == 0){
            prevAddr = curPageAddr;
        }
        if(curHead.row == 1){
            isLastCase = false;
            reAddress = reTraversal(mainFile,++depth,minimum,curPageAddr, parentAddr, oper);
            break;
        }
        switch(oper){
            case 0:
            if(minimum < curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }else if(minimum == curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,curPageAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }
            else{
                break;
            }
            case 3:
            if(minimum <= curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }else{
                break;
            }
            case 5:
            if(minimum <= curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }else{
                break;
            }
            case 6:
            if(minimum <= curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }else{
                break;
            }
            case 8:
            if(minimum <= curPageVal){
                reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
                isFound = true;
                isLastCase = false;
                break;
            }else{
                break;
            }
            default:
                std::cout<<"Wrong operation input"<<std::endl;
                break;
        }
        if(isFound){
            break;
        }
        prevAddr = curPageVal;
        mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
        mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
    }
    // mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
    // mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
    if(isLastCase){
        reAddress = reTraversal(mainFile,++depth,minimum,prevAddr, parentAddr, oper);
    }
    return reAddress;
}


void outputRow(std::fstream* mainFile, std::vector<colValue> columns, 
    std::vector<operValue> clauses, bool isAllWhere, bool isAllCol){
    std::string finalOutput = "";
    bool isRow = false;
    for(int j = 0; j < static_cast<int>(columns.size()); j++){
        std::vector<char> rowBuffer(columns[j].size);
        mainFile->read(rowBuffer.data(), rowBuffer.size());
        int integerOutput = 0;
        std::string output(rowBuffer.data(), columns[j].size);
        if(columns[j].isChar){
            if(isAllCol){
                finalOutput = finalOutput + output +"|";
            }else{
                if(columns[j].isShow){
                    finalOutput = finalOutput + output +"|";
                }else{
                    finalOutput = finalOutput + "";
                }
            }
            
        }else{
            std::memcpy(&integerOutput, rowBuffer.data(), sizeof(int));
            if(isAllCol){
                finalOutput = finalOutput + std::to_string(integerOutput) +"|";;
            }else{
                if(columns[j].isShow){
                    finalOutput = finalOutput + std::to_string(integerOutput) +"|";;
                }else{
                    finalOutput = finalOutput + "";
                }
            }
        }
        if(!isAllWhere){       
            switch(clauses[j].operType){
                case 0:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp.compare(output) == 0){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp == integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 1:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp.compare(output) != 0){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp != integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 2:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp > output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp > integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 3:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp < output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp < integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 4:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp >= output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp >= integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 5:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp <= output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp <= integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 6:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp <= output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp <= integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 7:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp >= output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp >= integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 8:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp < output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp < integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                case 9:
                    if(clauses[j].isChar){
                        if(clauses[j].strComp > output){
                            isRow = true;
                        }
                    }else{
                        if(clauses[j].numComp > integerOutput){
                            isRow = true;
                        }
                    }
                    break;
                default:
                    isRow = false;
                    break;
            }
        }else{
            isRow = true;
        }
    }
    if(isRow){
        std::cout << finalOutput << std::endl; 
    }
}


void seTraversal(std::vector<operValue> clauses, std::vector<colValue> columns, std::fstream* mainFile, 
    std::fstream* configFile, int keyColumn, bool isAllWhere, bool isAllCol){
    const size_t pageHeader = 96;
    const size_t cHeaderSize = 9;
    std::vector<char> cBuffer(cHeaderSize);
    cHeader confHeader;
    configFile->seekg(0,std::ios::beg);
    getConfig(cBuffer,configFile,&confHeader);
    std::cout<<"====== Outputting Rows ======"<<std::endl;
    std::cout<<"Column Name: ";
    for(int i = 0; i < static_cast<int>(columns.size()); i++){
        if(columns[i].isShow){
            std::cout<< columns[i].strComp <<"|";
        }else if(isAllCol){
            std::cout<< columns[i].strComp <<"|";
        }
    }
    std::cout<<std::endl;
    if(isAllWhere){
        int curAddr = 8192+8192;
        uint32_t count = 0;
        int pageCount = 0;
        while(1){
            std::vector<char> bufferP(pageHeader);
            pHeader curHead;
            getHeader(bufferP, mainFile, &curHead, curAddr);
            for(int i = 0; i<curHead.row;i++){
                mainFile->seekg(curAddr+96+(confHeader.totalBytes*i), std::ios::beg);
                outputRow(mainFile, columns, clauses, isAllWhere, isAllCol);
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
        
    }else{
        // Root + intermediate
        int startingAddr = 8192+8192;
        int parentAddr = 0;
        if(keyColumn != -1){
            int minimum = clauses[keyColumn].numComp;
            int operationType = clauses[keyColumn].operType;
            startingAddr = reTraversal(mainFile,0,minimum,0,&parentAddr,operationType);
        }
        const size_t pageHeader = 96;
        uint32_t count = 0;
        int pageCount = 0;
        while(1){
            std::vector<char> bufferP(pageHeader);
            pHeader curHead;
            getHeader(bufferP, mainFile, &curHead, startingAddr);
            for(int i = 0; i<curHead.row;i++){
                mainFile->seekg(startingAddr+96+(confHeader.totalBytes*i), std::ios::beg);
                outputRow(mainFile, columns, clauses, isAllWhere, isAllCol);
                count++;
            }
            if(count == confHeader.tempRow){
                break;
            }
            pageCount++;
            if(count%(INDAOFFSET-1) == 0){
                startingAddr += 8192;
            }
            startingAddr+=8192;
        }
    }
    

}

void selection(std::string fileName, isReadFile readingFile){
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
    // Selecting which column to show
    std::cout << "====== Select column ======" << std::endl; 
    int countBytes = 0;
    bool isAllWhere = true;
    std::vector<colValue> columnArr;
    columnArr.reserve(columnCount);
    uint8_t prev = 0;
    logFile.seekg(9,std::ios::beg);
    bool isAllCol = true;
    int keyColumn = -1;
    int curBytes = 0;

    if(readingFile.isRead){
        std::string line;
        std::getline(*readingFile.readFile,line);
        bool isAllStar = false;
        std::vector <std::string> tokens;
        if(line.compare("*") == 0){
            isAllStar = true;
        }else{
            std::stringstream check1(line);
            std::string intermediate;
            while(getline(check1, intermediate, ','))
            {
                tokens.push_back(intermediate);
            }
        }
        for(int iF = 0; iF <  static_cast<int>(columnCount); iF++){
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
            colValue temp;
            temp.strComp = columnName;
            temp.isShow = false;
            if(!isAllStar){
                for(int j = 0; j<static_cast<int>(tokens.size()); j++ ){
                    if(tokens[j].compare(columnName) == 0){
                        isAllCol = false;
                        temp.isShow = true;
                    }
                }
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
        logFile.seekg(9,std::ios::beg);
        std::getline(*readingFile.readFile,line);
        if(line.empty()){
            isAllWhere = true;
            for(int j = 0; j <  static_cast<int>(columnCount); j++){
                operValue temp; 
                temp.operType = -1;
                opArr.push_back(temp);
            }
        }else if(line.compare("WHERE") == 0){
            isAllWhere = false;
            std::getline(*readingFile.readFile,line);
            std::vector <std::string> tokensWhereBig;
            std::stringstream check1(line);
            std::string intermediate;
            while(getline(check1, intermediate, ','))
            {
                tokensWhereBig.push_back(intermediate);
            }
            for(int j = 0; j <  static_cast<int>(columnCount); j++){
                char columnName[30];
                bool isChar;
                uint8_t columnBytes;
                int whereCount = 0;
                logFile.read(columnName, 30);
                logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
                logFile.read(reinterpret_cast<char*>(&columnBytes),1);
                if(columnBytes == 0){
                    curBytes += 4;
                }else{
                    curBytes += columnBytes;
                }
                operValue temp; 
                bool isFoundInToken = false;
                for(int jW = 0; jW < static_cast<int>(tokensWhereBig.size()); jW++){
                    std::vector <std::string> tokensWhereSmall;
                    std::stringstream check2(line);
                    std::string intermediate2;
                    while(getline(check2, intermediate2, ' '))
                    {
                        tokensWhereSmall.push_back(intermediate2);
                    }
                    if(tokensWhereSmall[0].compare(columnName) == 0){
                        isAllWhere = false;
                        isFoundInToken = true;
                        if(isChar){
                            std::string inputVal = tokensWhereSmall[2];
                            temp.strComp = inputVal;
                            temp.numComp = -1;
                            temp.isChar = true;

                        }else{
                            temp.numComp = std::stoi(tokensWhereSmall[2]);
                            temp.strComp = "";
                            temp.isChar = false;
                            if(curBytes == colKey){
                                keyColumn = whereCount;
                            }
                        }
                        whereCount++;
                        int convertedOp = convertOperator(tokensWhereSmall[1]);
                        temp.operType = convertedOp;
                        opArr.push_back(temp);
                    }
                }
                if(!isFoundInToken){
                    temp.operType = -1;
                    opArr.push_back(temp);
                }
            }
        }else{
            std::cout<<"ERROR WHERE Clause format. Please check if you have typed the WHERE" << std::endl;
            exit(0);
        }
        seTraversal(opArr,columnArr,&mainFile,&logFile,keyColumn,isAllWhere, isAllCol);
    }else{
        std::cout <<"Select all columns [y] or specific [n]: ";
        std::string stopIN;
        std::cin >> stopIN;
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
            if(stopIN.compare("n") == 0){
                std::cout <<"Do you want to select [" << columnName <<"], [y] or [n]: ";
                std::cin >> colIN;
                isAllCol = false;
                if(colIN.compare("y")== 0){
                    temp.isShow = true;
                }
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
        // where clause
        std::cout << "====== Where column ======" << std::endl; 
        logFile.seekg(9,std::ios::beg);
        std::cout <<"Select all columns [y] or specific [n]: ";
        std::cin >> stopIN;
        int whereCount = 0;
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
        }
        seTraversal(opArr,columnArr,&mainFile,&logFile,keyColumn,isAllWhere, isAllCol);
    }

    mainFile.close();
    logFile.close();
    // for(int k = 0; k < static_cast<int>(columnCount); k++){
    //     std::cout << columnArr[k].strComp <<"==="<< opArr[k].operType<<std::endl; 
    // }
}