#include "readInput.h"


static void invalidAction(){
    std::cout << "Invalid option. Please try again" << std::endl;
}

enum queryType convertString(std::string str) {
    if (str.compare("CREATE") == 0) return CREATE;
    else if (str.compare("INSERT") == 0) return INSERT;
    else if (str.compare("UPDATE") == 0) return UPDATE;
    else if (str.compare("DEL") == 0) return DEL;
    else if (str.compare("SELECT") == 0) return SELECT;
    else{
        return EMP;
    }
}

numFile readInitialInput(){
    std::string input;
    while(1){
        std::cout << "Type mode or type --help for more information: ";
        std::cin >> input;
        std::cout << "Your input: " << input << std::endl;
        if(input.compare("--help") == 0){
            std::cout <<"In help section - work in progress"<<std::endl;
        }else if(input.compare("mode") == 0){
            std::cout << "Pick a mode: CREATE, INSERT, UPDATE, DEL, SELECT" << std::endl;
            std::cin >> input;
            queryType queryMode = convertString(input);
            switch (queryMode)
            {
            case CREATE :
                std::cout << "Name your table: ";
                std::cin >> input;
                return {0, input};
            case INSERT :
                return {1, input};
            case UPDATE :
                return {2, input};
            case DEL :
                return {3, input};
            case SELECT:
                return {4, input};
            default:
                invalidAction();
            }
        }else{
            invalidAction();
        }
    }
    return {-1, ""};
}

// Read user inputs, then while loop getting user inputs 
void readInputColumn(datapage* datapageName){
    // example case
    uint8_t columnCount = 0;
    datapageName->openLog(false);
    for(int i = 0; i <= 4; i++ ){
        char* name = new char[30];
        std::strcpy(name, "Hello");
        columnType types;
        if(i % 2 == 0){
            types.isChar = true;
            types.charLength = 20;
        }else{
            types.isChar = false;
            types.charLength = 0;
        }
        std::cout<<name<< std::endl;
        datapageName->setLogFile(name, types);
        delete[] name;
        columnCount++;
    }
    datapageName->closeLog(false);
    datapageName->setLogColumnCount(columnCount);
}