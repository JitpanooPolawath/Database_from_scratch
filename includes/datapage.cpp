#include "datapage.h"

void createLogFile(std::string fileName){
    std::fstream tempLogFile;
    tempLogFile.open(fileName,  std::ios::out | std::ios::binary );
    if (!tempLogFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    // just a file with number of rows
    uint8_t tempRow = 1;
    tempLogFile.write(reinterpret_cast<char*>(&tempRow),sizeof(tempRow));
    tempLogFile.close();
}


datapage::datapage(std::string tableName) {
    numRow = 0;
    bytesLeft = 8096;
    isFull = 0;
    fileName = tableName + ".mdf";
    logFileName = tableName + "_log" + ".ldf";
}

// Method IMPLEMENTATION

returnHeader datapage::getHeader(int startingAddress, std::string parentName){
    // Create entries in root
    // Get current root
    std::cout << "Name: " << parentName << std::endl;
    uint8_t Row;
    datapageFile.seekg(startingAddress,std::fstream::beg);
    datapageFile.read(reinterpret_cast<char*>(&Row), 1);
    std::cout << "Value: " <<static_cast<int>(Row) << std::endl;
    // Get byte left
    uint16_t Bytes;
    datapageFile.read(reinterpret_cast<char*>(&Bytes), 2);
    std::cout << "Bytes left: " <<static_cast<int>(Bytes) << std::endl;
    return {Row,Bytes};
}

void datapage::updateHeader(int startingAddress, int parentRow, int parentBytes ){
    // Update rootRow + 1
    datapageFile.seekp(startingAddress,std::fstream::beg);
    uint8_t tempRow = parentRow + 1;
    datapageFile.write(reinterpret_cast<char*>(&tempRow),sizeof(tempRow));
    // Update bytesLeft
    datapageFile.seekp(startingAddress+1,std::fstream::beg);
    uint16_t tempBytes = parentBytes - 4;
    datapageFile.write(reinterpret_cast<char*>(&tempBytes), 2);
}


void datapage::createRoot() {
    datapageFile.open(fileName, std::ios::app | std::ios::binary | std::ios::out);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    
    datapageFile.write(reinterpret_cast<char*>(&numRow), sizeof(numRow));
    datapageFile.write(reinterpret_cast<char*>(&bytesLeft), sizeof(bytesLeft));
    datapageFile.write(reinterpret_cast<char*>(&isFull), sizeof(isFull));
    datapageFile.write(reinterpret_cast<char*>(&pageHeaderBytes), sizeof(pageHeaderBytes));
    datapageFile.write(reinterpret_cast<char*>(&dataRows), sizeof(dataRows));
    datapageFile.close();
    createLogFile(logFileName);
}
        
void datapage::createIntermediate(){
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
   
    returnHeader header = getHeader(0, "root");
    updateHeader(0, header.parentRow,header.parentBytes);

    numRow = header.parentRow + 1;
    bytesLeft = header.parentBytes;

    // Input intermediate address to root page
    datapageFile.seekp(96,std::fstream::cur);
    datapageFile.seekp(4*(numRow - 1),std::fstream::cur);
    uint32_t address = (numRow) * 8192;
    datapageFile.write(reinterpret_cast<char*>(&address),sizeof(address));
    datapageFile.close();
    
    // Create the intermediate page
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out | std::ios::app);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    datapageFile.seekp(8192,std::fstream::beg);
    // Create intermediate page and 
    uint8_t tempFull = 0;
    uint8_t emptyRow = 0;
    uint16_t startingBytes = 8096;
    datapageFile.write(reinterpret_cast<char*>(&emptyRow),sizeof(emptyRow));
    datapageFile.write(reinterpret_cast<char*>(&startingBytes),sizeof(startingBytes));
    datapageFile.write(reinterpret_cast<char*>(&tempFull),sizeof(tempFull));
    datapageFile.write(reinterpret_cast<char*>(&pageHeaderBytes),sizeof(pageHeaderBytes));
    datapageFile.write(reinterpret_cast<char*>(&dataRows),sizeof(dataRows));
    datapageFile.close();

}

void datapage::createDataPage(){
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    returnHeader header = getHeader(8192*numRow, "intermediate");
    updateHeader(8192*numRow, header.parentRow,header.parentBytes);

    // Input DataPage address to Intermediate page
    datapageFile.seekp(96,std::fstream::cur);
    datapageFile.seekp(4*(header.parentRow),std::fstream::cur);
    uint32_t address = (header.parentRow+1) * 8192;
    datapageFile.write(reinterpret_cast<char*>(&address),sizeof(address));
    datapageFile.close();


    // Create the data page file
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out | std::ios::app);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }

    datapageFile.seekp(8192*header.parentRow,std::fstream::beg);
    uint8_t tempFull = 1;
    uint8_t emptyRow = 0;
    uint16_t startingBytes = 8096;
    datapageFile.write(reinterpret_cast<char*>(&emptyRow), sizeof(emptyRow));
    datapageFile.write(reinterpret_cast<char*>(&startingBytes), sizeof(startingBytes));
    datapageFile.write(reinterpret_cast<char*>(&tempFull), sizeof(tempFull));
    datapageFile.write(reinterpret_cast<char*>(&pageHeaderBytes), sizeof(pageHeaderBytes));
    datapageFile.write(reinterpret_cast<char*>(&dataRows), sizeof(dataRows));
    datapageFile.close();

}

// Log table file (number of rows, columns name, columns type, log(timestamp, PST)) 
// Log file is continous (continue append data for log)
void datapage::setLogFile(std::string name, columnType types){
    logFile.open(logFileName, std::ios::binary | std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
}