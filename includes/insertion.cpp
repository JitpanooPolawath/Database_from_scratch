#include "insertion.h"



void getConfig(std::vector<char> header, std::fstream* conFile, cHeader* curHead){
    conFile->read(header.data(), header.capacity());   
    std::memcpy(&curHead->tempRow, header.data(), 4);
    std::memcpy(&curHead->columnCount, header.data() + 4, 1);
    std::memcpy(&curHead->totalBytes, header.data() + 5, 2);
    std::memcpy(&curHead->colKey, header.data() + 7, 1);
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

// Traverse through the root and intermediate page to get datapage
doubleAddr traversal(std::fstream* mainFile,int depth, uint32_t minimum, int curAddr){
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
    for(int i = 0; i < curHead.row; i++){
        curMin.mini = curPageVal;
        curMin.addr = curPageAddr;
        if(minimum < curPageVal && i != 0){
            temp = traversal(mainFile,++depth,minimum,prevMin.addr);
            break;
        }else if(minimum < curPageVal && i == 0){
            temp = traversal(mainFile,++depth,minimum,curMin.addr);
            break;
        }
        else{
            prevMin.mini = curPageVal;
            prevMin.addr = curPageAddr;
        }
        mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
        mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
        // std::cout<<"===========" << std::endl;
        // std::cout<<static_cast<uint32_t>(prevMin.mini)<<std::endl;
        // std::cout<<static_cast<uint32_t>(prevMin.addr)<<std::endl;
        // std::cout<<static_cast<uint32_t>(prevMin.mini)<<std::endl;
        // std::cout<<static_cast<uint32_t>(prevMin.addr)<<std::endl;
        // std::cout<<"===========" << std::endl;
    }

    return temp;
}


void insert(std::vector<unsigned char> inputtedRow, std::string fileName, int depth, int minimum, int parentAddr){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    std::string lFileName = fileName + "_config" +".ldf";
    std::fstream lainFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary);
    lainFile.open(lFileName, std::ios::in | std::ios::binary);
    if(!mainFile.is_open() || !lainFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    // Traverse through the pages to get the datapage that minimum is > but < next value
    doubleAddr bothAddr;
    bothAddr = traversal(&mainFile, depth, minimum,parentAddr);
    std::cout << bothAddr.prevAddr<<std::endl;
    std::cout<<static_cast<uint32_t>(bothAddr.curAddr)<<std::endl;

    // For loop to check if minimum is less than row address
    int theAddr = -1;
    if(bothAddr.prevAddr == -1){
        theAddr = bothAddr.curAddr;
    }else{
        theAddr = bothAddr.prevAddr;
    }
    const size_t cHeaderSize = 8;
    std::vector<char> cBuffer(cHeaderSize);
    cHeader confHeader;
    getConfig(cBuffer,&lainFile,&confHeader);
    std::cout<<confHeader.tempRow<<std::endl;
    std::cout<<static_cast<int>(confHeader.columnCount)<<std::endl;
    std::cout<<confHeader.totalBytes<<std::endl;
    std::cout<<static_cast<int>(confHeader.colKey)<<std::endl;

    const size_t pageHeader = 96;
    std::vector<char> buffer(pageHeader);
    pHeader curHead;
    getHeader(buffer, &mainFile, &curHead, theAddr);
    std::cout<<"At address: "<< theAddr <<std::endl;
    std::cout<<curHead.row<<std::endl;
    std::cout<<curHead.bytesLeft<<std::endl;
    std::cout<<curHead.isFull<<std::endl;
    std::cout<<curHead.curAddr<<std::endl;
    std::cout<<curHead.minNum<<std::endl;
    std::cout<<curHead.curID<<std::endl;
    std::cout<<curHead.prevAddr<<std::endl;
    std::cout<<curHead.nextAddr<<std::endl;

    mainFile.seekg(theAddr+96);
    // mainFile.read(reinterpret_cast<char*>(&curPageVal),sizeof(curPageVal));
    // mainFile.read(reinterpret_cast<char*>(&curPageAddr),sizeof(curPageAddr));
    // for(int i = 0; i < curHead.row; i++){
    //     curMin.mini = curPageVal;
    //     curMin.addr = curPageAddr;
    //     if(minimum < curPageVal && i != 0){
    //         temp = traversal(mainFile,++depth,minimum,prevMin.addr);
    //         break;
    //     }else if(minimum < curPageVal && i == 0){
    //         temp = traversal(mainFile,++depth,minimum,curMin.addr);
    //         break;
    //     }
    //     else{
    //         prevMin.mini = curPageVal;
    //         prevMin.addr = curPageAddr;
    //     }
    //     mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
    //     mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
    // }

    bool isBytesLeft = true;



}