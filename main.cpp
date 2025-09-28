// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include "includes/datapage.h"
#include "includes/readInput.h"


int main(){

    // Reading input user
    numFile queryMode = readInitialInput();

    if (queryMode.mode == 0){
        datapage temp(queryMode.fileName);
        temp.createRoot();
        readInputColumn(&temp);
        temp.setLogTimestamp(0);
        temp.setLogTimestamp(1);
        temp.setLogTimestamp(2);
        temp.createIntermediate();
        temp.createDataPage();
        temp.createDataPage();
        temp.createDataPage();
        temp.createDataPage();
        temp.createDataPage();
    }
    
    return 0;
}