#include "readInput.h"


static void invalidAction(){
    std::cout << "Invalid option. Please try again" << std::endl;
    std::cin.clear(); 
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
        std::cout << "Type [mode] or type [--help] for more information: ";
        std::cin >> input;
        std::cout << "Your input: " << input << std::endl;
        if(input.compare("--help") == 0){
            std::cout <<"In help section - work in progress"<<std::endl;
        }else if(input.compare("mode") == 0){
            std::cout << "Pick a mode: CREATE, INSERT, UPDATE, DEL, SELECT: ";
            std::cin >> input;
            queryType queryMode = convertString(input);
            std::cout << "Table name: ";
            std::cin >> input;
            switch (queryMode)
            {
            case CREATE :
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
    uint16_t totalBytes = 0;
    uint8_t colKey = 0;
    bool foundPKey = false;
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
            totalBytes += temp_length;
            types.charLength = temp_length;
            types.isChar = true;
        }else if(std::strlen(typeIn) == 3){
            types.charLength = 0;
            types.isChar = false;
            totalBytes += 4;
        }else{
            invalidAction();
            continue;
        }
        if(foundPKey == false){
            std::string inKey;
            std::cout << "Is this column a primary key?(Only one primary key) [y] or [n]: ";
            std::cin >> inKey;
            if(inKey.compare("y") == 0){
                std::cout << "Is a primary key"<<std::endl;
                colKey = columnCount;
                foundPKey = true;
            }else{
                std::cout << "Not a primary key"<<std::endl;
            }
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    datapageName->setLogColumnCount(columnCount, totalBytes, colKey);
}


std::vector<unsigned char> readInsertion(std::string inputFileName){
    std::fstream logFile;
    std::string logFileName = inputFileName + "_config" + ".ldf";
    logFile.open(logFileName, std::ios::in | std::ios::binary);
    if(!logFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    // GET numOfRow, columnCount, totalBytes, columnKey
    
    uint32_t numOfRow;
    logFile.read(reinterpret_cast<char*>(&numOfRow),sizeof(numOfRow));
    uint8_t columnCount;
    logFile.read(reinterpret_cast<char*>(&columnCount),sizeof(columnCount));
    uint16_t totalBytes;
    logFile.read(reinterpret_cast<char*>(&totalBytes),sizeof(totalBytes));
    uint8_t colKey;
    logFile.read(reinterpret_cast<char*>(&colKey),sizeof(colKey));

    std::cout <<"\n"<<"----- Config details -----" << std::endl; 
    std::cout << "number of rows: " << numOfRow << std::endl;
    std::cout << "column count: " << static_cast<int>(columnCount) << std::endl;
    std::cout << "totalbytes: " << totalBytes << std::endl;
    std::cout << "primarykey location: " << static_cast<int>(colKey) << std::endl;

    std::vector<unsigned char> storedBytes;
    storedBytes.reserve(totalBytes);
    std::string fullOutput;
    for(int i = 0; i < columnCount; i++){
        char columnName[30];
        logFile.read(columnName, sizeof(columnName));
        bool isChar;
        logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
        int columnBytes;
        // Char
        std::string isCharString;
        if(isChar == 1){
            isCharString = true;
            logFile.read(reinterpret_cast<char*>(&columnBytes),1);
        }else{
            isCharString = false;
            logFile.read(reinterpret_cast<char*>(&columnBytes),4);
        }

        std::cout << "----- Column details-----" << std::endl; 
        std::cout << "name: " << columnName << std::endl;
        std::cout << "is this column char : " << isCharString << std::endl;
        // std::cout << "columnBytes: " << columnBytes << std::endl;
        
        // INT
        std::string outputType;
        if(isChar == 0){
            outputType = "integer";
        }else{
            outputType = "char[" + std::to_string(columnBytes)+"]";
        }
        // row total bytes (i.e. 100 bytes, 200b byte, etc)
        // bool loopStop = true;
        while(1){
            std::cout << "Column(" << outputType <<"): " << columnName << std::endl;
            if(isChar == 0){
                uint32_t inputNumber = 0;
                std::cout << "Input data: ";
                std::cin >> inputNumber;
                if(std::cin.fail()){
                    invalidAction();
                    continue;
                }else{
                    fullOutput = fullOutput + std::to_string(inputNumber) + "|";
                    unsigned char* bytes = reinterpret_cast<unsigned char*>(&inputNumber);
                    storedBytes.insert(storedBytes.end(), bytes, bytes + sizeof(uint32_t));
                    break;
                }
            }else{
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                char inputBuffer[columnBytes];
                std::cout << "Input data: ";
                std::cin.getline(inputBuffer, columnBytes);
                if(std::cin.fail()){
                    invalidAction();
                    continue;
                }else{
                    fullOutput = fullOutput + inputBuffer + "|";
                    size_t length = std::strlen(inputBuffer);
                    storedBytes.insert(
                        storedBytes.end(),
                        inputBuffer,                 
                        inputBuffer + length
                    );
                    break;
                }
            }
        }
    }
    std::cout << "----- Inputted data -----" << std::endl;
    std::cout << fullOutput << std::endl;
    logFile.close();
    return storedBytes;
}