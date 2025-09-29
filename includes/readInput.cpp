#include "readInput.h"


static void invalidAction(){
    std::cout << "Invalid option. Please try again" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    while(1){
        char name[30] = {0};
        std::cout<< "\n" << "Enter column name (LIMIT 30 character): ";
        std::cin >> name;
        if (std::cin.fail()) {
            invalidAction();
            continue;
        }
        if(std::strlen(name) > 30){
            invalidAction();
            continue;
        }
        columnType types;
        char* typeIn = new char[4];
        std::cout << "Type char or int: ";
        std::cin >> typeIn;
        if(std::strlen(typeIn) == 4){
            int temp_length = 0; // 1. Read into a standard int
            std::cout << "Input char size (limit 100): ";
            std::cin >> temp_length;
            if(temp_length >= 100 || temp_length <= 0){
                invalidAction();
                continue;
            }
            types.charLength = temp_length;
            types.isChar = true;
        }else if(std::strlen(typeIn) == 3){
            types.charLength = 0;
            types.isChar = false;
        }else{
            invalidAction();
            continue;
        }
        std::string stopIN;
        std::cout << "Type [y] to add more column OR [n] to complete: ";
        std::cin >> stopIN;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        datapageName->setLogFile(name, types);
        columnCount++;
        if(stopIN.compare("n") == 0){
            break;
        }
    }
    datapageName->closeLog(false);
    datapageName->setLogColumnCount(columnCount);
}