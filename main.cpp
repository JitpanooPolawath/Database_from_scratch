// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include "includes/datapage.h"
#include "includes/readInput.h"


void createTable(datapage* filePage){
        filePage->createRoot();
        readInputColumn(filePage);
        filePage->setLogTimestamp(0);
        filePage->setLogTimestamp(1);
        filePage->setLogTimestamp(2);
        // temp.createIntermediate();
        // temp.createDataPage();
        // temp.createDataPage();
        // temp.createDataPage();
        // temp.createDataPage();
        // temp.createDataPage();
}


int main(){

    // Reading input user
    numFile queryMode = readInitialInput();

    if (queryMode.mode == 0){
        datapage filePage(queryMode.fileName);
        createTable(&filePage);
    }
    
    return 0;
}