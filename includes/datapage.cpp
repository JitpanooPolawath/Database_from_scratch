#include "datapage.h"


void createLogFile(std::string fileName, bool isTime){
    std::fstream tempLogFile;
    tempLogFile.open(fileName,  std::ios::out | std::ios::binary );
    if (!tempLogFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    // just a file with number of rows
    if(isTime){

    }else{
        uint32_t tempRow = 0;
        uint8_t columnCount = 0;
        uint16_t totalBytes = 0;
        uint16_t keyBytes = 0;
        tempLogFile.write(reinterpret_cast<char*>(&tempRow),sizeof(tempRow));
        tempLogFile.write(reinterpret_cast<char*>(&columnCount),sizeof(columnCount));
        tempLogFile.write(reinterpret_cast<char*>(&totalBytes),sizeof(totalBytes));
        tempLogFile.write(reinterpret_cast<char*>(&keyBytes),sizeof(keyBytes));
        tempLogFile.close();
    }

}


datapage::datapage(std::string tableName) {
    // Num row is the number of row the root page current have
    
    fileName = tableName + ".mdf";
    logFileName = tableName + "_config" + ".ldf";
    logTimeName = tableName + "_log" + ".ldf";
}


returnHeader datapage::getHeader(int startingAddress, std::string parentName){
    // Get parent header from address
    std::cout << "Header name: " << parentName << std::endl;
    uint8_t Row;
    datapageFile.seekg(startingAddress,std::fstream::beg);
    datapageFile.read(reinterpret_cast<char*>(&Row), 1);
    std::cout << "Value: " <<static_cast<int>(Row) << std::endl;
    // Get byte left
    uint16_t Bytes;
    datapageFile.read(reinterpret_cast<char*>(&Bytes), 2);
    std::cout << "Bytes left: " <<static_cast<int>(Bytes) << std::endl;
    // Get address
    bool isFull;
    datapageFile.read(reinterpret_cast<char*>(&isFull), 1);
    std::cout << "Is full: " <<static_cast<int>(isFull) << std::endl;
    uint32_t address;
    datapageFile.read(reinterpret_cast<char*>(&address), sizeof(address));
    return {Row,Bytes,address};
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
    
    uint32_t emptyAddress=0;
    uint32_t emptyNumber = 0;
    uint8_t currentID = 0;
    uint8_t numRow = 0;      // (1 byte)
    uint16_t bytesLeft = 8096;   // (2 bytes)
    uint8_t isFull = 0; // (1 byte)
    datapageFile.write(reinterpret_cast<char*>(&numRow), sizeof(numRow));
    datapageFile.write(reinterpret_cast<char*>(&bytesLeft), sizeof(bytesLeft));
    datapageFile.write(reinterpret_cast<char*>(&isFull), sizeof(isFull));
    datapageFile.write(reinterpret_cast<char*>(&emptyAddress),sizeof(emptyAddress));
    datapageFile.write(reinterpret_cast<char*>(&emptyNumber),sizeof(emptyNumber));
    datapageFile.write(reinterpret_cast<char*>(&currentID),sizeof(currentID));
    datapageFile.write(reinterpret_cast<char*>(&emptyAddress),sizeof(emptyAddress));
    datapageFile.write(reinterpret_cast<char*>(&emptyAddress),sizeof(emptyAddress));
    datapageFile.write(reinterpret_cast<char*>(&pageHeaderBytes), sizeof(pageHeaderBytes));
    datapageFile.write(reinterpret_cast<char*>(&dataRows), sizeof(dataRows));
    datapageFile.close();
    createLogFile(logFileName, false);
    createLogFile(logTimeName, true);
}
        
void datapage::createIntermediate(){
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
   
    returnHeader header = getHeader(0, "root");
    updateHeader(0, header.parentRow,header.parentBytes);

    uint8_t numRow = header.parentRow + 1;
    std::cout << "Address: "<< 0 << std::endl;

    // Input intermediate address to root page
    datapageFile.seekp(96,std::ios::beg);
    // TODO: Update to sizeof(uint16_t) + (4 * (numRow - 1))
    if(numRow == 1){
        datapageFile.seekp(4*(numRow - 1),std::fstream::cur);
    }else{
        datapageFile.seekp(4*(numRow - 1) + 4,std::fstream::cur);
    }
    uint32_t minimumNumber = UINT32_MAX;
    datapageFile.write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    uint32_t address;
    if (header.parentRow != 0){
         // TODO: CHANGE  * 8192 * 7 -> 8192 + (header.parentRow * INDAOFFSET /* (6/11) */ * 8192)  
        address =  8192 + (header.parentRow * INDAOFFSET /* (6/11) */ * 8192);
    }else{
        address = numRow * 8192;
    }
    datapageFile.write(reinterpret_cast<char*>(&address),sizeof(address));
    datapageFile.close();
    
    // Create the intermediate page
    datapageFile.open(fileName, std::ios::in | std::ios::binary | std::ios::out | std::ios::app);
    if (!datapageFile.is_open()) {
        std::cout << "Error: file did not open correctly" << std::endl;
        exit(0);
    }
    // datapageFile.seekp(8192,std::fstream::beg);
    // Create intermediate page and 
    uint8_t tempFull = 0;
    uint8_t emptyRow = 0;
    uint16_t startingBytes = 8096;
    uint8_t currentID = header.parentRow;
    uint32_t prevAddress = 0;
    if(currentID != 0){
        prevAddress =  8192 + (8192 * (currentID - 1) * INDAOFFSET) ;
    }
    uint32_t nextAddress = 8192 + (8192 * (currentID + 1) * INDAOFFSET);
    datapageFile.write(reinterpret_cast<char*>(&emptyRow),sizeof(emptyRow));
    datapageFile.write(reinterpret_cast<char*>(&startingBytes),sizeof(startingBytes));
    datapageFile.write(reinterpret_cast<char*>(&tempFull),sizeof(tempFull));
    datapageFile.write(reinterpret_cast<char*>(&address),sizeof(address));
    datapageFile.write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    datapageFile.write(reinterpret_cast<char*>(&currentID),sizeof(currentID));
    datapageFile.write(reinterpret_cast<char*>(&prevAddress),sizeof(prevAddress));
    datapageFile.write(reinterpret_cast<char*>(&nextAddress),sizeof(nextAddress));
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
    returnHeader rootheader = getHeader(0, "root-datapage");
    int getParentRow = 8192 * rootheader.parentRow;
    if(rootheader.parentRow > 1){
        // TODO: CHANGED 8192 * 7 * (numRow-1) -> 8192 + ((numRow-1) * INDAOFFSET * 8192)
        getParentRow = 8192 + ((rootheader.parentRow-1) * INDAOFFSET * 8192);
    }
    returnHeader header = getHeader(getParentRow, "intermediate");
    updateHeader(getParentRow, header.parentRow,header.parentBytes);
    std::cout << "Address: "<< header.parentAddress << std::endl;
    // Input DataPage address to Intermediate page
    datapageFile.seekp(header.parentAddress + 96,std::ios::beg);
    if(header.parentRow == 0){
        datapageFile.seekp((header.parentRow),std::fstream::cur);
    }else{
        datapageFile.seekp((8*header.parentRow),std::fstream::cur);
    }
    uint32_t minimumNumber = UINT32_MAX;
    datapageFile.write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    uint32_t address = header.parentAddress + 8192 * (header.parentRow+1);
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
    uint8_t currentID = header.parentRow;
    uint32_t prevAddress = 0;
    if(currentID == 0 && rootheader.parentRow != 1 ){
        prevAddress = header.parentAddress - 8192 ;
    }else if(currentID != 0){
        prevAddress =  8192 + header.parentAddress + (8192 * (currentID - 1)) ;
    }
    uint32_t nextAddress = 8192 + header.parentAddress + (8192 * (currentID + 1));
    if(currentID == (INDAOFFSET - 2)){
        nextAddress = 8192 + 8192 + header.parentAddress + (8192 * (currentID + 1));
    }
    datapageFile.write(reinterpret_cast<char*>(&emptyRow), sizeof(emptyRow));
    datapageFile.write(reinterpret_cast<char*>(&startingBytes), sizeof(startingBytes));
    datapageFile.write(reinterpret_cast<char*>(&tempFull), sizeof(tempFull));
    datapageFile.write(reinterpret_cast<char*>(&address),sizeof(address));
    datapageFile.write(reinterpret_cast<char*>(&minimumNumber),sizeof(minimumNumber));
    datapageFile.write(reinterpret_cast<char*>(&currentID),sizeof(currentID));
    datapageFile.write(reinterpret_cast<char*>(&prevAddress),sizeof(prevAddress));
    datapageFile.write(reinterpret_cast<char*>(&nextAddress),sizeof(nextAddress));
    datapageFile.write(reinterpret_cast<char*>(&pageHeaderBytes), sizeof(pageHeaderBytes));
    datapageFile.write(reinterpret_cast<char*>(&dataRows), sizeof(dataRows));
    datapageFile.close();

}

void datapage::openLog(bool isLogTime){
    if (isLogTime){
        if (logTimeFile.is_open()) {
        return;
        }
        logTimeFile.open(logTimeName, std::ios::binary | std::ios::out | std::ios::app);
        if (!logTimeFile.is_open()) {
            std::cout << "Error: Log file did not open correctly" << std::endl;
            exit(0); // Use a non-zero exit code for errors
        } 
    }else{
        if (logFile.is_open()) {
        return;
        }
        logFile.open(logFileName, std::ios::binary | std::ios::out | std::ios::app);
        if (!logFile.is_open()) {
            std::cout << "Error: Log file did not open correctly" << std::endl;
            exit(0); // Use a non-zero exit code for errors
        } 
    }
    
}

void datapage::closeLog(bool isLogTime){
    if(isLogTime){
        if (logTimeFile.is_open()) {
            logTimeFile.close();
        }
    }else{
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
}


// Log table file (number of rows, columns name, columns type, log(timestamp, PST)) 
// Log file is continous (continue append data for log)
void datapage::setLogFile(char* name, columnType types){
    logFile.write(name,columnSize);
    if (types.isChar){
        logFile.write(reinterpret_cast<char*>(&types.isChar),sizeof(bool));
        logFile.write(reinterpret_cast<char*>(&types.charLength),sizeof(uint8_t));
    }else{
        uint32_t temp = 0;
        logFile.write(reinterpret_cast<char*>(&types.isChar),sizeof(bool));
        logFile.write(reinterpret_cast<char*>(&temp),sizeof(uint32_t));
    }
} 

// Update the number of column in the table
void datapage::setLogColumnCount(uint8_t count, uint16_t totalbytes, uint16_t keyBytes){
    logFile.open(logFileName, std::ios::binary | std::ios::out |std::ios::in);
    if (!logFile.is_open()) {
        std::cout << "Error: Log file did not open correctly" << std::endl;
        exit(0); // Use a non-zero exit code for errors
    }
    logFile.seekp(sizeof(uint32_t),std::fstream::beg);
    logFile.write(reinterpret_cast<char*>(&count),sizeof(uint8_t));
    logFile.write(reinterpret_cast<char*>(&totalbytes),sizeof(totalbytes));
    logFile.write(reinterpret_cast<char*>(&keyBytes),sizeof(keyBytes));
    logFile.close();
}

// Set lop timestamp
void datapage::setLogTimestamp(uint8_t isValue){
    openLog(true);

    // isValue: 0 = insert action, 1 = update action, 2 = delete action
    logTimeFile.write(reinterpret_cast<char*>(&isValue),sizeof(uint8_t));
    // Timestamp
    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    uint32_t epoch_time = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    logTimeFile.write(reinterpret_cast<char*>(&epoch_time),sizeof(uint32_t));
    closeLog(true);
}
