// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "includes/datapage.h"
#include "includes/readInput.h"
#include "includes/insertion.h"
#include "includes/selection.h"
#include "includes/update.h"

void createTable(datapage* filePage, isReadFile readingFile){
        filePage->createRoot();
        readInputColumn(filePage, readingFile);
        filePage->createIntermediate();
        filePage->createDataPage();

}
bool pathExists(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

int main(){
    // Reading input user
    numFile queryMode = readInitialInput(false, "");

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
        std::getline(readFile,line);
        if(line.compare("mode") == 0){
            while(std::getline(readFile,line)){
                if(line.empty()){
                    continue;
                }
                queryMode = readInitialInput(true,line);
                std::string fileName = queryMode.fileName + ".mdf";
                if(pathExists(fileName.c_str()) && queryMode.mode == 0){
                    std::string logFileName = queryMode.fileName + "_config" + ".ldf";
                    std::string logTimeName = queryMode.fileName + "_log" + ".ldf";
                    std::cout << "Deleting original file" << std::endl;
                    std::remove(fileName.c_str());
                    std::remove(logFileName.c_str());
                    std::remove(logTimeName.c_str());
                }
                if (queryMode.mode == 0){
                    datapage filePage(queryMode.fileName);
                    isReadFile readingFile;
                    readingFile.isRead = true;
                    readingFile.readFile = &readFile;
                    createTable(&filePage, readingFile);
                }else if(queryMode.mode == 1){
                    // std::vector<unsigned char> inputtedRow;
                    std::string fileInRow;
                    while(getline(readFile,fileInRow)){
                        insertionRow inputtedRow;
                        if(fileInRow.empty()){
                            break;
                        }else if(fileInRow.compare("FILE") == 0){
                            std::string csvFileName;
                            getline(readFile,csvFileName);
                            std::fstream csvFile;
                            csvFile.open(csvFileName, std::ios::in);
                            std::string fileInRow;
                            while(getline(csvFile,fileInRow)){
                                inputtedRow = readInsertion(queryMode.fileName, true, fileInRow);
                                insert(inputtedRow.row, queryMode.fileName, 0,inputtedRow.min,0);
                            }
                        }else{
                            inputtedRow = readInsertion(queryMode.fileName, true, fileInRow);
                            insert(inputtedRow.row, queryMode.fileName, 0,inputtedRow.min,0);
                        } 
                    }
                }else if(queryMode.mode == 4){
                    isReadFile readingFile;
                    readingFile.isRead = true;
                    readingFile.readFile = &readFile;
                    selection(queryMode.fileName, readingFile);
                }else if(queryMode.mode == 2){
                    isReadFile readingFile;
                    readingFile.isRead = true;
                    readingFile.readFile = &readFile;
                    updateRow(queryMode.fileName, readingFile, false);
                }else if(queryMode.mode == 3){
                    isReadFile readingFile;
                    readingFile.isRead = true;
                    readingFile.readFile = &readFile;
                    updateRow(queryMode.fileName, readingFile, true);
                }
                std::cout << "\n" << std::endl;
            }
        }else{
            return 0;
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
                isReadFile readingFile;
                readingFile.isRead = false;
                readingFile.readFile = nullptr;
                createTable(&filePage, readingFile);
            }else if(queryMode.mode == 1){
                insertionRow inputtedRow = readInsertion(queryMode.fileName, false, "");
                insert(inputtedRow.row, queryMode.fileName, 0,inputtedRow.min,0);
            }else if(queryMode.mode == 4){
                isReadFile readingFile;
                readingFile.isRead = false;
                readingFile.readFile = nullptr;
                selection(queryMode.fileName, readingFile);
            }else if(queryMode.mode == 2){
                isReadFile readingFile;
                readingFile.isRead = false;
                readingFile.readFile = nullptr;
                updateRow(queryMode.fileName, readingFile, false);
            }
            else if(queryMode.mode == 3){
                isReadFile readingFile;
                readingFile.isRead = false;
                readingFile.readFile = nullptr;
                updateRow(queryMode.fileName, readingFile, true);
            }
            queryMode = readInitialInput(true, "");
        }
    }
    return 0;
}