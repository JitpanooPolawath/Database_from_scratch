#include "insertion.h"


void createHeader(std::fstream* mainFile, int startingAddress, int parentRow, int parentBytes ){
    // Update rootRow + 1
    mainFile->seekp(startingAddress,std::fstream::beg);
    uint8_t tempRow = parentRow + 1;
    mainFile->write(reinterpret_cast<char*>(&tempRow),sizeof(tempRow));
    // Update bytesLeft
    mainFile->seekp(startingAddress+1,std::fstream::beg);
    uint16_t tempBytes = parentBytes - 4;
    mainFile->write(reinterpret_cast<char*>(&tempBytes), 2);
}

void createIntermediate(std::fstream* mainFile, std::string fileName){
    mainFile->open(fileName,std::ios::in |std::ios::binary|std::ios::out);
    if (!mainFile->is_open()) {
        std::cout << "Error: file did not open correctly in linearInsertion intermediate page" << std::endl;
        exit(0);
    }
    const size_t pageHeader = 96;
    std::vector<char> bufferR(pageHeader);
    pHeader rootHead;
    getHeader(bufferR, mainFile, &rootHead, 0);
    createHeader(mainFile, 0, rootHead.row, rootHead.bytesLeft);

    uint8_t numRow = rootHead.row + 1;
    // Input intermediate address to root page
    mainFile->seekp(96,std::ios::beg);
    // TODO: Update to sizeof(uint16_t) + (4 * (numRow - 1))
    if(numRow == 1){
        mainFile->seekp(4*(numRow - 1),std::fstream::cur);
    }else{
        mainFile->seekp(4*(numRow - 1) + 4,std::fstream::cur);
    }
    uint32_t minimumNumber = UINT32_MAX;
    mainFile->write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    uint32_t address;
    if (rootHead.row != 0){
         // TODO: CHANGE  * 8192 * 7 -> 8192 + (header.parentRow * INDAOFFSET /* (6/11) */ * 8192)  
        address =  8192 + (rootHead.row * INDAOFFSET /* (6/11) */ * 8192);
    }else{
        address = numRow * 8192;
    }
    mainFile->write(reinterpret_cast<char*>(&address),sizeof(address));
    mainFile->close();
    
    // Create the intermediate page
    mainFile->open(fileName, std::ios::in | std::ios::binary | std::ios::out | std::ios::app);
    if (!mainFile->is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    // datapageFile.seekp(8192,std::fstream::beg);
    // Create intermediate page and 
    uint8_t tempFull = 0;
    uint8_t emptyRow = 0;
    uint16_t startingBytes = 8096;
    uint8_t currentID = rootHead.row;
    uint32_t prevAddress = 0;
    if(currentID != 0){
        prevAddress =  8192 + (8192 * (currentID - 1) * INDAOFFSET) ;
    }
    uint32_t nextAddress = 8192 + (8192 * (currentID + 1) * INDAOFFSET);
    char pageHeaderBytes[EMPTYHEADER] = {0};
    char dataRows[8096] = {0};
    mainFile->write(reinterpret_cast<char*>(&emptyRow),sizeof(emptyRow));
    mainFile->write(reinterpret_cast<char*>(&startingBytes),sizeof(startingBytes));
    mainFile->write(reinterpret_cast<char*>(&tempFull),sizeof(tempFull));
    mainFile->write(reinterpret_cast<char*>(&address),sizeof(address));
    mainFile->write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    mainFile->write(reinterpret_cast<char*>(&currentID),sizeof(currentID));
    mainFile->write(reinterpret_cast<char*>(&prevAddress),sizeof(prevAddress));
    mainFile->write(reinterpret_cast<char*>(&nextAddress),sizeof(nextAddress));
    mainFile->write(reinterpret_cast<char*>(&pageHeaderBytes),sizeof(pageHeaderBytes));
    mainFile->write(reinterpret_cast<char*>(&dataRows),sizeof(dataRows));
    mainFile->close();
}

void createDataPage(std::fstream* mainFile, std::string fileName){
    mainFile->open(fileName,std::ios::in |std::ios::binary|std::ios::out);
    if (!mainFile->is_open()) {
        std::cout << "Error: file did not open correctly in linearInsertion datapage" << std::endl;
        exit(0);
    }
    const size_t pageHeader = 96;
    std::vector<char> bufferR(pageHeader);
    pHeader rootHead;
    getHeader(bufferR, mainFile, &rootHead, 0);
    int getParentRow = 8192 * rootHead.row;
    if(rootHead.row > 1){
        // TODO: CHANGED 8192 * 7 * (numRow-1) -> 8192 + ((numRow-1) * INDAOFFSET * 8192)
        getParentRow = 8192 + ((rootHead.row-1) * INDAOFFSET * 8192);
    }
    std::vector<char> bufferI(pageHeader);
    pHeader interHead;
    getHeader(bufferI, mainFile, &interHead, getParentRow);
    createHeader(mainFile, getParentRow, interHead.row, interHead.bytesLeft);

    // Input DataPage address to Intermediate page
    mainFile->seekp(interHead.curAddr + 96,std::ios::beg);
    // TODO: Update to sizeof(uint16_t) + (4 * (numRow - 1))
    if(interHead.row == 0){
        mainFile->seekp((interHead.row),std::fstream::cur);
    }else{
        mainFile->seekp((8*interHead.row),std::fstream::cur);
    }
    uint32_t minimumNumber = UINT32_MAX;
    mainFile->write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    uint32_t address = interHead.curAddr + 8192 * (interHead.row+1);
    mainFile->write(reinterpret_cast<char*>(&address),sizeof(address));
    mainFile->close();
    
    // Create the intermediate page
    mainFile->open(fileName, std::ios::in | std::ios::binary | std::ios::out | std::ios::app);
    if (!mainFile->is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    // datapageFile.seekp(8192,std::fstream::beg);
    // Create intermediate page and 
    mainFile->seekp(8192*interHead.row,std::fstream::beg);
    uint8_t tempFull = 1;
    uint8_t emptyRow = 0;
    uint16_t startingBytes = 8096;
    uint8_t currentID = interHead.row;
    uint32_t prevAddress = 0;
    if(currentID == 0 && rootHead.row != 1 ){
        prevAddress = interHead.curAddr - 8192 ;
    }else if(currentID != 0){
        prevAddress =  8192 + interHead.curAddr + (8192 * (currentID - 1)) ;
    }
    uint32_t nextAddress = 8192 + interHead.curAddr + (8192 * (currentID + 1));
    if(currentID == (INDAOFFSET - 2)){
        nextAddress = 8192 + 8192 + interHead.curAddr + (8192 * (currentID + 1));
    }
    char pageHeaderBytes[EMPTYHEADER] = {0};
    char dataRows[8096] = {0};
    mainFile->write(reinterpret_cast<char*>(&emptyRow),sizeof(emptyRow));
    mainFile->write(reinterpret_cast<char*>(&startingBytes),sizeof(startingBytes));
    mainFile->write(reinterpret_cast<char*>(&tempFull),sizeof(tempFull));
    mainFile->write(reinterpret_cast<char*>(&address),sizeof(address));
    mainFile->write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    mainFile->write(reinterpret_cast<char*>(&currentID),sizeof(currentID));
    mainFile->write(reinterpret_cast<char*>(&prevAddress),sizeof(prevAddress));
    mainFile->write(reinterpret_cast<char*>(&nextAddress),sizeof(nextAddress));
    mainFile->write(reinterpret_cast<char*>(&pageHeaderBytes),sizeof(pageHeaderBytes));
    mainFile->write(reinterpret_cast<char*>(&dataRows),sizeof(dataRows));
    mainFile->close();
}

std::vector<char> copyRowBytes(std::fstream* mainFile, int lastStartingAddr, int lastEndingRowAddr, uint16_t totalBytes){
    int bytesToCopy = lastEndingRowAddr - lastStartingAddr;    
    std::vector<char> buffer(bytesToCopy);
    buffer.reserve(totalBytes);
    mainFile->seekg(lastStartingAddr, std::fstream::beg);
    mainFile->read(buffer.data(), bytesToCopy);
    return buffer;
}

void copyBytes(std::fstream* mainFile, int startingRowAddr, int endingRowAddr, int toAddr) {
    int bytesToCopy = endingRowAddr - startingRowAddr;    
    std::vector<char> buffer(bytesToCopy);
    
    mainFile->seekg(startingRowAddr, std::fstream::beg);
    mainFile->read(buffer.data(), bytesToCopy);
    
    mainFile->seekp(toAddr, std::fstream::beg);
    mainFile->write(buffer.data(), bytesToCopy);
    mainFile->flush();
}

void getConfig(std::vector<char> header, std::fstream* conFile, cHeader* curHead){
    conFile->read(header.data(), header.capacity());   
    std::memcpy(&curHead->tempRow, header.data(), 4);
    conFile->seekg(5,std::fstream::beg);
    std::memcpy(&curHead->columnCount, header.data() + 4, 1);
    std::memcpy(&curHead->totalBytes, header.data() + 5, 2);
    std::memcpy(&curHead->keyBytes, header.data() + 7, 2);
}

void getHeader(std::vector<char> header, std::fstream* mainFile, pHeader* curHead ,int curAddr){
    mainFile->seekg(curAddr,std::ios::beg);
    mainFile->read(header.data(), header.capacity());
    curHead->row = header[0];
    std::memcpy(&curHead->bytesLeft, header.data() + 1, 2);
    std::memcpy(&curHead->isFull, header.data() + 3, 1);
    std::memcpy(&curHead->curAddr, header.data() + 4, 4);
    std::memcpy(&curHead->minNum, header.data() + 8, 4);
    curHead->curID = header[12];
    std::memcpy(&curHead->prevAddr, header.data() + 13, 4);
    std::memcpy(&curHead->nextAddr, header.data() + 17, 4);
}

void updateHeader(std::fstream* mainFile ,int curAddr, uint16_t updatedBytes, uint8_t rowCount, uint32_t minimum, bool isInter){
    mainFile->seekg(curAddr+8,std::fstream::beg);
    uint32_t tempMin;
    mainFile->read(reinterpret_cast<char*>(&tempMin),sizeof(tempMin));
    if(minimum < tempMin){
        if(!isInter){
            mainFile->seekp(curAddr,std::fstream::beg);
            mainFile->write(reinterpret_cast<char*>(&rowCount), sizeof(rowCount));
            mainFile->write(reinterpret_cast<char*>(&updatedBytes), sizeof(updatedBytes));
        }
        mainFile->seekp(curAddr+8,std::fstream::beg);
        mainFile->write(reinterpret_cast<char*>(&minimum), sizeof(minimum));
        mainFile->flush();
    }else{
        if(!isInter){
            mainFile->seekp(curAddr,std::fstream::beg);
            mainFile->write(reinterpret_cast<char*>(&rowCount), sizeof(rowCount));
            mainFile->write(reinterpret_cast<char*>(&updatedBytes), sizeof(updatedBytes));
        }
    }
}

void updateInterRow(std::fstream* mainFile,int addr, uint32_t minimum){
    mainFile->seekg(addr,std::fstream::beg);
    uint32_t tempMin;
    mainFile->read(reinterpret_cast<char*>(&tempMin),sizeof(tempMin));
    if(minimum < tempMin){
        mainFile->seekp(addr,std::fstream::beg);
        mainFile->write(reinterpret_cast<char*>(&minimum), sizeof(minimum));
        mainFile->flush();
    }  
}


void updateLogTimestamp(uint8_t isValue, std::fstream* logTimeFile){
    // isValue: 0 = insert action, 1 = update action, 2 = delete action
    logTimeFile->write(reinterpret_cast<char*>(&isValue),sizeof(uint8_t));
    // Timestamp
    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    uint32_t epoch_time = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    logTimeFile->write(reinterpret_cast<char*>(&epoch_time),sizeof(uint32_t));
}

void updateConfig(uint32_t numRow, std::fstream* logFile){
    logFile->seekp(0,std::fstream::beg);
    logFile->write(reinterpret_cast<char*>(&numRow),sizeof(uint32_t));
}


void linearInsert(std::vector<char> lastElementRow, std::string fileName, std::fstream* mainFile, int prevID, uint8_t interRow, 
    int interID, uint32_t curAddr, uint16_t totalBytes, uint16_t keyByte){

    // check if intermediate page have enough space (intermediate cannot be inserted in the middle)
    if(interRow % (INDAOFFSET-1) == 0){
        createIntermediate(mainFile,fileName);
        ++interID;
    }
    if((prevID + 1) >= interRow){
        createDataPage(mainFile,fileName);
    }
    mainFile->open(fileName, std::ios::in | std::ios::binary | std::ios::out);
    if (!mainFile->is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    const size_t pageHeader = 96;
    std::vector<char> bufferI(pageHeader);
    pHeader interHead;
    std::vector<char> bufferD(pageHeader);
    pHeader curHead;
    int intermediateAddr = 8192 + ((interID*INDAOFFSET)*8192);
    getHeader(bufferI, mainFile, &interHead, intermediateAddr);
    getHeader(bufferD, mainFile, &curHead, curAddr);
    bool isLinearInsert = false;
    if((curHead.bytesLeft - totalBytes) < 0){
        isLinearInsert = true;
    }
    mainFile->seekg(curAddr+96+keyByte-4);
    int count = curHead.row;
    uint16_t minimum;
    int index = keyByte - 4;
    std::memcpy(&minimum, &lastElementRow[index], sizeof(uint16_t));
    for(int i = 0; i < curHead.row; i++){
        uint32_t curKeyVal;
        mainFile->read(reinterpret_cast<char*>(&curKeyVal),sizeof(curKeyVal));
        std::cout<<"current value: "<<curKeyVal<<std::endl;
        if(minimum < curKeyVal){
            count = i;
            break;
        }else if(minimum == curKeyVal){
            std::cout<<"ID already exist in table. Insertion failed!";
            return;
        }
        mainFile->seekg(totalBytes-4,std::ios::cur);
    }

    uint32_t startingRowAddr = (curHead.curAddr + 96) + (count * totalBytes);
    uint32_t destiRowAddr = (curHead.curAddr + 96) + ((count+1) * totalBytes);
    uint32_t endingRowAddr = (curHead.curAddr + 96) + (curHead.row * totalBytes);
    uint32_t lastStartRowAddr = endingRowAddr - totalBytes;
    uint32_t lastEndRowAddr = endingRowAddr;
    std::vector<char> lastTempRow = copyRowBytes(mainFile, lastStartRowAddr, lastEndRowAddr, totalBytes);
    bool skipCopy = false;
    if(count == curHead.row){
        skipCopy = true;
    }
    if(!skipCopy){
        copyBytes(mainFile,startingRowAddr,endingRowAddr,destiRowAddr);
    }
    int writePos = curAddr+96+(count * totalBytes);
    mainFile->seekp(writePos,std::fstream::beg);
    mainFile->write(reinterpret_cast<const char*>(lastElementRow.data()),lastElementRow.size());

    if(!isLinearInsert){
        updateHeader(mainFile,curAddr,curHead.bytesLeft - totalBytes,++curHead.row, minimum, false);
        updateInterRow(mainFile,intermediateAddr+96+(8*curHead.curID),minimum);
        updateHeader(mainFile,intermediateAddr,0,0, minimum, true);
        updateInterRow(mainFile,0+96+(8*interHead.curID),minimum);  
    }
    mainFile->close();
    if(isLinearInsert){
        linearInsert(lastTempRow, fileName, mainFile, curHead.curID, interHead.row
            , interHead.curID, curHead.nextAddr, totalBytes, keyByte);
    }

}



// Traverse through the root and intermediate page to get datapage
doubleAddr traversal(std::fstream* mainFile,int depth, uint32_t minimum, int curAddr, int* parentAddr){
    const size_t pageHeader = 96;
    std::vector<char> buffer(pageHeader);
    pHeader curHead;
    getHeader(buffer, mainFile, &curHead, curAddr);

    // Address less than current 
    minimus prevMin;
    prevMin.mini = -1;
    prevMin.addr = -1;
    minimus curMin;
    curMin.mini = -1;
    curMin.addr = -1;
    doubleAddr temp;
    std::cout<< "At depth["<<depth<<"], address: " <<  curHead.curAddr <<std::endl;
    if(curHead.row == 0 || depth == 2){
        temp.prevAddr = prevMin.addr;
        temp.curAddr = curHead.curAddr;
        return temp;
    }


    // Start of the pageAddr + pageHeader, to get to the page body
    // for loop to iterate through the root and intermediate page - worst case O(n^2)
    mainFile->seekg(curAddr+96);
    uint32_t curPageVal; 
    uint32_t curPageAddr;
    mainFile->read(reinterpret_cast<char*>(&curPageVal),sizeof(curPageVal));
    mainFile->read(reinterpret_cast<char*>(&curPageAddr),sizeof(curPageAddr));
    bool isLast = false;
    for(int i = 0; i < curHead.row; i++){
        curMin.mini = curPageVal;
        curMin.addr = curPageAddr;
        if(depth == 1){
                *parentAddr = curAddr;
        }
        if(minimum < curPageVal && i != 0){
            temp = traversal(mainFile,++depth,minimum,prevMin.addr, parentAddr);
            isLast = false;
            break;
        }else if(minimum < curPageVal && i == 0){    
            temp = traversal(mainFile,++depth,minimum,curMin.addr, parentAddr);
            isLast = false;
            break;
        }
        else{
            prevMin.mini = curPageVal;
            prevMin.addr = curPageAddr;
        }
        isLast = true;
        mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
        mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
    }
    if(isLast){
        temp = traversal(mainFile,++depth,minimum,curMin.addr, parentAddr);
    }

    return temp;
}

void insert(std::vector<unsigned char> inputtedRow, std::string fileName, int depth, uint32_t minimum, int parentAddr){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    std::string lFileName = fileName + "_config" +".ldf";
    std::fstream lainFile;
    std::string lTFileName = fileName + "_log" +".ldf";
    std::fstream lTainFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary|std::ios::out);
    lainFile.open(lFileName, std::ios::in | std::ios::binary|std::ios::out);
    lTainFile.open(lTFileName, std::ios::binary|std::ios::out|std::ios::app);
    if(!mainFile.is_open() || !lainFile.is_open() || !lTainFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    // Traverse through the pages to get the datapage that minimum is > but < next value
    doubleAddr bothAddr;
    int intermediateAddr; 
    bothAddr = traversal(&mainFile, depth, minimum,parentAddr, &intermediateAddr);

    // For loop to check if minimum is less than row address
    int theAddr = -1;
    if(bothAddr.prevAddr == -1){
        theAddr = bothAddr.curAddr;
    }else{
        theAddr = bothAddr.prevAddr;
    }
    const size_t cHeaderSize = 9;
    std::vector<char> cBuffer(cHeaderSize);
    cHeader confHeader;
    getConfig(cBuffer,&lainFile,&confHeader);
    std::cout<<"====== Config header ======"<<std::endl;
    std::cout<<"current number of row: "<<confHeader.tempRow<<std::endl;
    std::cout<<"number of column in table: "<<static_cast<int>(confHeader.columnCount)<<std::endl;
    std::cout<<"maximum row size(bytes): "<<confHeader.totalBytes-1<<std::endl;
    std::cout<<"location of primary key: "<<static_cast<int>(confHeader.keyBytes)<<std::endl;

    std::cout << "====== Intermediate address ======\n" <<"intermediate address: "<<intermediateAddr <<std::endl;
    const size_t pageHeader = 96;
    std::vector<char> bufferI(pageHeader);
    pHeader interHead;
    getHeader(bufferI, &mainFile, &interHead, intermediateAddr);
    std::cout<<"next intermediate page address: "<<interHead.nextAddr<<std::endl;

    std::vector<char> bufferP(pageHeader);
    pHeader curHead;
    getHeader(bufferP, &mainFile, &curHead, theAddr);
    std::cout<<"====== Datapage header ====== \nAt address: "<< theAddr <<std::endl;
    std::cout<<"datapage row count: "<<static_cast<int>(curHead.row)<<std::endl;
    std::cout<<"datapage bytes left: "<<curHead.bytesLeft<<std::endl;
    std::cout<<"is datapage full(not correct): "<<curHead.isFull<<std::endl;
    std::cout<<"datapage address: "<<curHead.curAddr<<std::endl;
    std::cout<<"minimum value: "<<curHead.minNum<<std::endl;
    std::cout<<"datapage ID: "<<curHead.curID<<std::endl;
    std::cout<<"previous datapage address: "<<curHead.prevAddr<<std::endl;
    std::cout<<"next datapage address: "<<curHead.nextAddr<<std::endl;

    mainFile.seekg(theAddr+96+confHeader.keyBytes-4);
    std::cout<<"====== In for-loop ======"<<std::endl;
    int count = curHead.row;
    for(int i = 0; i < curHead.row; i++){
        uint32_t curKeyVal;
        mainFile.read(reinterpret_cast<char*>(&curKeyVal),sizeof(curKeyVal));
        std::cout<<"current value: "<<curKeyVal<<std::endl;
        if(minimum < curKeyVal){
            count = i;
            break;
        }else if(minimum == curKeyVal){
            std::cout<<"ID already exist in table. Insertion failed!";
            return;
        }
        mainFile.seekg(confHeader.totalBytes-4,std::ios::cur);
    }
    std::cout<<"====== Inputting rows ======"<<std::endl;
    // Copy elements
    uint32_t startingRowAddr = (curHead.curAddr + 96) + (count * confHeader.totalBytes);
    uint32_t destiRowAddr = (curHead.curAddr + 96) + ((count+1) * confHeader.totalBytes);
    uint32_t endingRowAddr = (curHead.curAddr + 96) + (curHead.row * confHeader.totalBytes);
    uint32_t lastStartRowAddr = endingRowAddr - confHeader.totalBytes;
    uint32_t lastEndRowAddr = endingRowAddr;
    std::vector<char> lastElementRow = copyRowBytes(&mainFile, lastStartRowAddr, lastEndRowAddr, confHeader.totalBytes);
    // Insertion
    bool skipCopy = false;
    if(count == curHead.row){
        skipCopy = true;
    }
    if(!skipCopy){
        copyBytes(&mainFile,startingRowAddr,endingRowAddr,destiRowAddr);
    }
    // std::cout << startingRowAddr << " " << endingRowAddr << " " << lastStartRowAddr << " " << lastEndRowAddr<<std::endl;
    int writePos = theAddr+96+(count * confHeader.totalBytes);
    mainFile.seekp(writePos,std::fstream::beg);
    mainFile.write(reinterpret_cast<const char*>(inputtedRow.data()),inputtedRow.size());
    bool isBytesLeft = true;
    int updatedBytesLeft = curHead.bytesLeft - confHeader.totalBytes;
    if(updatedBytesLeft < 0){
        isBytesLeft = false; 
    }
    std::cout<<"====== Updating datapage header ======"<<std::endl;
    if(isBytesLeft){
        updateHeader(&mainFile,theAddr,updatedBytesLeft,++curHead.row, minimum, false);
        updateInterRow(&mainFile,intermediateAddr+96+(8*curHead.curID),minimum);
        updateHeader(&mainFile,intermediateAddr,0,0, minimum, true);
        updateInterRow(&mainFile,0+96+(8*interHead.curID),minimum);
    }
    std::cout<<"====== Updating config & log file ======"<<std::endl;
    updateLogTimestamp(0,&lTainFile);
    updateConfig(++confHeader.tempRow, &lainFile);
    std::cout<<"====== Updating intermediate header ======"<<std::endl;
    std::cout<<"====== Closing files ======"<<std::endl;
    mainFile.close();
    lainFile.close();
    lTainFile.close();

    // Linear insertion. Inserting into the next datapage
    if(!isBytesLeft){
        linearInsert(lastElementRow, mFileName,&mainFile, curHead.curID, 
            interHead.row, interHead.curID, curHead.nextAddr, 
            confHeader.totalBytes, confHeader.keyBytes);
    }

}