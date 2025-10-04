// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "includes/datapage.h"
#include "includes/readInput.h"
#include "includes/insertion.h"


void createTable(datapage* filePage){

        filePage->createRoot();
        readInputColumn(filePage);
        // if(true){
            // filePage->setLogTimestamp(0);
            // filePage->setLogTimestamp(1);
            // filePage->setLogTimestamp(2);
        // }
        filePage->createIntermediate();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createDataPage();
        filePage->createIntermediate();
        filePage->createDataPage();

}
bool pathExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int main(){

    // Reading input user
    numFile queryMode = readInitialInput();
    std::string fileName = queryMode.fileName + ".mdf";
    if(pathExists(fileName.c_str()) && queryMode.mode == 0){
        std::string logFileName = queryMode.fileName + "_config" + ".ldf";
        std::string logTimeName = queryMode.fileName + "_log" + ".ldf";
        std::cout << "Deleting original file" << std::endl;
        std::remove(fileName.c_str());
        std::remove(logFileName.c_str());
        std::remove(logTimeName.c_str());
    }
    datapage filePage(queryMode.fileName);
    if (queryMode.mode == 0){
        createTable(&filePage);
    }else if(queryMode.mode == 1){
        std::vector<unsigned char> inputtedRow;
        // std::vector<unsigned char> inputtedRow = readInsertion(queryMode.fileName);
        insert(inputtedRow, queryMode.fileName, false);
    }
    
    return 0;
}