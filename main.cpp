// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include "includes/datapage.h"
#include "includes/readInput.h"
#include "includes/insertion.h"


void createTable(datapage* filePage){
        filePage->createRoot();
        if(true){
            readInputColumn(filePage);
            filePage->setLogTimestamp(0);
            filePage->setLogTimestamp(1);
            filePage->setLogTimestamp(2);
        }
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
        // filePage->createIntermediate();
        // filePage->createDataPage();

}


int main(){

    // Reading input user
    numFile queryMode = readInitialInput();

    if (queryMode.mode == 0){
        datapage filePage(queryMode.fileName);
        createTable(&filePage);
    }else if(queryMode.mode == 1){
        readInsertion(queryMode.fileName);
        // insert();
    }
    
    return 0;
}