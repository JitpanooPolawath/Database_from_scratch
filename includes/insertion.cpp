#include "insertion.h"


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
int traversal(std::fstream* mainFile,bool useCurrentMin, uint32_t minimum, int curAddr){
    const size_t pageHeader = 96;
    std::vector<char> buffer(pageHeader);
    pHeader curHead;
    getHeader(buffer, mainFile, &curHead, curAddr);
    std::cout<<"=========== At: "<< curAddr <<std::endl;
    std::cout<<curHead.row<<std::endl;
    std::cout<<curHead.bytesLeft<<std::endl;
    std::cout<<curHead.isFull<<std::endl;
    std::cout<<curHead.curAddr<<std::endl;
    std::cout<<curHead.minNum<<std::endl;
    std::cout<<curHead.curID<<std::endl;
    std::cout<<curHead.prevAddr<<std::endl;
    std::cout<<curHead.nextAddr<<std::endl;

    // Address less than current 
    minimus prevMin;
    prevMin.mini = -1;
    prevMin.addr = -1;
    minimus curMin;
    curMin.mini = -1;
    curMin.addr = -1;

    // Start of the pageAddr + pageHeader, to get to the page body
    // for loop to iterate through the root and intermediate page - worst case O(n^2)
    mainFile->seekg(curAddr+96);
    uint32_t curPageVal; 
    uint32_t curPageAddr;
    mainFile->read(reinterpret_cast<char*>(&curPageVal),sizeof(curPageVal));
    mainFile->read(reinterpret_cast<char*>(&curPageAddr),sizeof(curPageAddr));
    std::cout<< "-------------- At: " << curPageAddr <<std::endl;
    for(int i = 0; i < curHead.row; i++){
        curMin.mini = curPageVal;
        curMin.addr = curPageAddr;
        if(minimum < curPageVal){
            if(prevMin.addr != -1){
                traversal(mainFile,false,minimum,prevMin.addr);
            }else{
                traversal(mainFile,false,minimum,curMin.addr);
            }
            break;
        }else{
            prevMin.mini = curPageVal;
            prevMin.addr = curPageAddr;
        }
        mainFile->read(reinterpret_cast<char*>(&curPageVal),4);
        mainFile->read(reinterpret_cast<char*>(&curPageAddr),4);
        std::cout<<"===========" << std::endl;
        std::cout<<static_cast<uint32_t>(prevMin.mini)<<std::endl;
        std::cout<<static_cast<uint32_t>(prevMin.addr)<<std::endl;
        std::cout<<static_cast<uint32_t>(prevMin.mini)<<std::endl;
        std::cout<<static_cast<uint32_t>(prevMin.addr)<<std::endl;
        std::cout<<"===========" << std::endl;
    }

    // getHeader(buffer, mainFile, &curHead, 8192+8192);
    return 0;
}


void insert(std::vector<unsigned char> inputtedRow, std::string fileName, bool useCurrentMin){
    std::string mFileName = fileName + ".mdf";
    std::fstream mainFile;
    mainFile.open(mFileName, std::ios::in | std::ios::binary);
    if(!mainFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    int address = traversal(&mainFile, useCurrentMin, 3,0);
}