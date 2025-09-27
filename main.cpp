// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include "includes/datapage.h"

void readInitialInput(std::string* input ){
    std::cout << "Enter your query or type --help for more info: " << std::endl;
    std::cin >> *input;
    if(input->compare("--help") == 0){
        std::cout <<"In help section"<<std::endl;
    }
}

// Read user inputs, then while loop getting user inputs 
// void readInputColumn(datapage datapageName){
//     // example case
//     for(int i = 0; i < 4; i++ ){
//         std::string name = "test" + i;
//         columnType types;
//         if(i % 2 == 0){
//             types.isChar = true;
//             types.charLength = 20;
//         }else{
//             types.isChar = false;
//         }
//         datapageName.setLogFile(name, types);
//     }
    
// }


int main(){

    // Reading input user
    std::string tabInput;
    readInitialInput(&tabInput);
    std::cout << "Your input: " << tabInput << std::endl;


    datapage temp(tabInput);
    temp.createRoot();
    temp.createIntermediate();
    temp.createDataPage();
    // temp.createDataPage();
    // temp.createIntermediate();
    return 0;
}