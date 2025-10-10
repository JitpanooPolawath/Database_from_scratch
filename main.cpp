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
        filePage->createIntermediate();
        filePage->createDataPage();

}
bool pathExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int main(){
    // Reading input user
    numFile queryMode = readInitialInput(false);

    if(queryMode.mode == -2){
        std::string readFileName;
        std::cout << "type your query command file name: ";
        std::cin >> readFileName;
        std::fstream readFile;
        readFile.open(readFileName, std::ios::in);
        if(!readFile.is_open()){
            std::cout << "Error at reading inputted file" << std::endl;
            exit(1);
        }
        // Doing the command again
        std::string line;
        while(std::getline(readFile,line)){
            std::cout<<line<<std::endl;
        }
    }else{
        while(1){
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
                // std::vector<unsigned char> inputtedRow;
                insertionRow inputtedRow = readInsertion(queryMode.fileName);
                insert(inputtedRow.row, queryMode.fileName, 0,inputtedRow.min,0);
            }
            queryMode = readInitialInput(true);
        }
    }
    return 0;
}