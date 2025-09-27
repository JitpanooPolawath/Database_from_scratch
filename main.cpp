// point of access of DB using parameter to create, insert, update, delete, and select tables
#include <iostream>
#include <string>
#include "includes/datapage.h"

void readInput(std::string* input ){
    std::cout << "Enter your query or type --help for more info: " << std::endl;
    std::getline(std::cin,*input);
}


int main(){

    // Reading input user
    std::string input;
    readInput(&input);
    std::cout << "Your input: " << input << std::endl;


    datapage temp(input);
    temp.createRoot();
    temp.createIntermediate();
    temp.createDataPage();
    temp.createDataPage();
    temp.createIntermediate();
    return 0;
}