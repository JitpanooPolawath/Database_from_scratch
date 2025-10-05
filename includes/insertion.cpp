#include "insertion.h"


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
    std::memcpy(&curHead->curID, header.data() + 12, 1);
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

// Traverse through the root and intermediate page to get datapage
doubleAddr traversal(std::fstream* mainFile,int depth, uint32_t minimum, int curAddr, int* parentAddr){
    const size_t pageHeader = 96;
    std::vector<char> buffer(pageHeader);
    pHeader curHead;
    getHeader(buffer, mainFile, &curHead, curAddr);
    // std::cout<<"=========== At: "<< curAddr <<std::endl;
    // std::cout<<curHead.row<<std::endl;
    // std::cout<<curHead.bytesLeft<<std::endl;
    // std::cout<<curHead.isFull<<std::endl;
    // std::cout<<curHead.curAddr<<std::endl;
    // std::cout<<curHead.minNum<<std::endl;
    // std::cout<<curHead.curID<<std::endl;
    // std::cout<<curHead.prevAddr<<std::endl;
    // std::cout<<curHead.nextAddr<<std::endl;

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
    std::cout<<confHeader.tempRow<<std::endl;
    std::cout<<static_cast<int>(confHeader.columnCount)<<std::endl;
    std::cout<<confHeader.totalBytes<<std::endl;
    std::cout<<static_cast<int>(confHeader.keyBytes)<<std::endl;

    std::cout << "====== Intermediate address ======\n" << intermediateAddr <<std::endl;
    const size_t pageHeader = 96;
    std::vector<char> bufferI(pageHeader);
    pHeader interHead;
    getHeader(bufferI, &mainFile, &interHead, intermediateAddr);
    std::cout<<interHead.nextAddr<<std::endl;

    std::vector<char> bufferP(pageHeader);
    pHeader curHead;
    getHeader(bufferP, &mainFile, &curHead, theAddr);
    std::cout<<"====== Datapage header ====== \nAt address: "<< theAddr <<std::endl;
    std::cout<<static_cast<int>(curHead.row)<<std::endl;
    std::cout<<curHead.bytesLeft<<std::endl;
    std::cout<<curHead.isFull<<std::endl;
    std::cout<<curHead.curAddr<<std::endl;
    std::cout<<curHead.minNum<<std::endl;
    std::cout<<curHead.curID<<std::endl;
    std::cout<<curHead.prevAddr<<std::endl;
    std::cout<<curHead.nextAddr<<std::endl;

    bool isSmaller = false;
    mainFile.seekg(theAddr+96+confHeader.keyBytes-4);
    std::cout<<"====== In for loop ======"<<std::endl;
    int count = curHead.row;
    for(int i = 0; i < curHead.row; i++){
        uint32_t curKeyVal;
        mainFile.read(reinterpret_cast<char*>(&curKeyVal),sizeof(curKeyVal));
        std::cout<<"current value: "<<curKeyVal<<std::endl;
        if(minimum < curKeyVal){
            isSmaller = true;
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
    uint16_t updatedBytesLeft = curHead.bytesLeft - confHeader.totalBytes;
    if(updatedBytesLeft > 0){
        isBytesLeft = false;
    }
    std::cout<<"====== Updating datapage header ======"<<std::endl;
    updateHeader(&mainFile,theAddr,updatedBytesLeft,++curHead.row, minimum, false);
    updateInterRow(&mainFile,intermediateAddr+96+(8*curHead.curID),minimum);
    updateHeader(&mainFile,intermediateAddr,0,0, minimum, true);
    updateInterRow(&mainFile,0+96+(8*interHead.curID),minimum);
    std::cout<<"====== Updating config & log file ======"<<std::endl;
    updateLogTimestamp(0,&lTainFile);
    updateConfig(++confHeader.tempRow, &lainFile);
    std::cout<<"====== Updating intermediate header ======"<<std::endl;
    std::cout<<"====== Closing files ======"<<std::endl;
    mainFile.close();
    lainFile.close();
    lTainFile.close();

}