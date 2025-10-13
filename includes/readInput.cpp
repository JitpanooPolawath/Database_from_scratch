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
    else if (str.compare("DELETE") == 0) return DELETE;
    else if (str.compare("SELECT") == 0) return SELECT;
    else{
        return EMP;
    }
}

numFile readInitialInput(bool isLoop, std::string modeType){
    std::string input;
    std::cout << "\n====== Start your Query ======" << std::endl;
    while(1){
        if(!isLoop){
            std::cout << "Type [mode], [file], [--help], or [exit]: ";
            std::cin >> input;
            std::cout << "Your input: " << input << std::endl;
        }      
        if(input.compare("--help") == 0){
            std::cout <<"In help section - work in progress"<<std::endl;
        }else if((input.compare("mode") == 0 || isLoop) ){
            if(modeType.length() < 1){
                std::cout << "Pick a mode: CREATE, INSERT, UPDATE, DELETE, SELECT, EXIT: ";
                std::cin >> input;
            }
            queryType queryMode;
            if(modeType.compare("") == 0){
               queryMode = convertString(input);
               if(queryMode != EMP){
                    std::cout << "Table name: ";
                    std::cin >> input;
                }
            }else{
                std::vector <std::string> tokens;
                std::stringstream check1(modeType);
                std::string intermediate;
                while(getline(check1, intermediate, ' '))
                {
                    tokens.push_back(intermediate);
                }
                queryMode = convertString(tokens[0]);
                input = tokens[1];
            }
            switch (queryMode)
            {
            case CREATE :
                return {0, input};
            case INSERT :
                return {1, input};
            case UPDATE :
                return {2, input};
            case DELETE :
                return {3, input};
            case SELECT:
                return {4, input};
            case EMP:
                std::cout <<"Exiting query command"<<std::endl; 
                exit(0);
            default:
                invalidAction();
            }
        }else if(input.compare("exit") == 0){
            std::cout <<"Exiting query command"<<std::endl;
            exit(0);
        }else if(input.compare("file") == 0){
            std::cout <<"In file reading mode"<<std::endl;
            return {-2, ""};
        }else{
            invalidAction();
        }
    }
    return {-1, ""};
}

// Read user inputs, then while loop getting user inputs 
void readInputColumn(datapage* datapageName, isReadFile readingFile){
    // example case
    uint8_t columnCount = 0;
    datapageName->openLog(false);
    uint16_t totalBytes = 0;
    uint16_t keyBytes = 0;
    bool foundPKey = false;
    if(readingFile.isRead){
        std::string line;
        while(std::getline(*readingFile.readFile,line)){
            if(line.empty()){
                break;
            }
            std::vector <std::string> tokens;
            std::stringstream check1(line);
            std::string intermediate;
            while(getline(check1, intermediate, '('))
            {
                tokens.push_back(intermediate);
            }
            columnType types;
            if(tokens[1].compare("char") == 0){
                types.isChar = true;
                std::string number_string = tokens[2].substr(0, tokens[2].length() - 2);
                int temp_length = std::stoi(number_string);
                totalBytes += temp_length;
                types.charLength = temp_length;
            }
            else{
                types.isChar = false;
                totalBytes += 4;
                types.charLength = 0;
                if(tokens.size() == 3){
                    keyBytes = totalBytes;
                }
                
            }
            char name[30] = {0};
            columnCount++;
            strcpy(name, tokens[0].c_str());
            datapageName->setLogFile(name, types);
        }
        datapageName->closeLog(false);
        datapageName->setLogColumnCount(columnCount, totalBytes+1, keyBytes);
    }else{
        while(1){
            char name[30] = {0};
            std::cout<<"====== Set column ======"<<std::endl;
            std::cout<<"Current bytes used: ["<<totalBytes<<"], out of 8095. You have: "<< 8095-totalBytes <<" bytes left." << std::endl;
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
            if(foundPKey == false && std::strlen(typeIn) == 3){
                std::string inKey;
                std::cout << "Is this column a primary key?(Only one primary key) [y] or [n]: ";
                std::cin >> inKey;
                if(inKey.compare("y") == 0){
                    std::cout << "Is a primary key"<<std::endl;
                    keyBytes = totalBytes;
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
            if(totalBytes > 8096){
                std::cout<<"This row byte size exceed limit (8096 bytes)\nThe previous column was removed.\nIf you want to keep all column please try again."<<std::endl;
            }
        }
        datapageName->closeLog(false);
        datapageName->setLogColumnCount(columnCount, totalBytes+1, keyBytes);
    }
}


insertionRow readInsertion(std::string inputFileName,  bool isFile, std::string readLine){
    std::fstream logFile;
    std::string logFileName = inputFileName + "_config" + ".ldf";
    logFile.open(logFileName, std::ios::in | std::ios::binary);
    if(!logFile.is_open()){
        std::cout << "File open error at insertion" << std::endl;
        exit(1);
    }
    // GET numOfRow, columnCount, totalBytes, columnKey
    if(isFile){
         if(readLine.compare("") != 0){
            uint32_t numOfRow;
            logFile.read(reinterpret_cast<char*>(&numOfRow),sizeof(numOfRow));
            uint8_t columnCount;
            logFile.read(reinterpret_cast<char*>(&columnCount),sizeof(columnCount));
            uint16_t totalBytes;
            logFile.read(reinterpret_cast<char*>(&totalBytes),sizeof(totalBytes));
            uint16_t colKey;
            logFile.read(reinterpret_cast<char*>(&colKey),sizeof(colKey));

            std::vector<unsigned char> storedBytes;
            storedBytes.reserve(totalBytes);
            std::string fullOutput;
            uint32_t minValue = 0;
            int toKeyBytes = 0;
            // line Tokenizer
            std::vector <std::string> tokens;
            std::stringstream check1(readLine);
            std::string intermediate;
            while(getline(check1, intermediate, '|'))
            {
                tokens.push_back(intermediate);
            }
            for(int i = 0; i <  static_cast<int>(columnCount); i++){
                char columnName[30];
                logFile.read(columnName, 30);
                bool isChar;
                logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
                uint8_t  columnBytes;
                // Char
                std::string isCharString;
                if(isChar == 1){
                    isCharString = "true";
                    logFile.read(reinterpret_cast<char*>(&columnBytes),1);
                }else{
                    isCharString = "false";
                    logFile.read(reinterpret_cast<char*>(&columnBytes),1);
                }

                std::cout << "----- Column details-----" << std::endl; 
                std::cout << "name: " << columnName << std::endl;
                std::cout << "is this column char : " << isCharString << std::endl;
                
                // INT
                if(isChar == 0){
                    toKeyBytes += 4;
                    fullOutput = fullOutput + tokens[i] + "|";
                    int inputNumber = stoi(tokens[i]);
                    unsigned char* bytes = reinterpret_cast<unsigned char*>(&inputNumber);
                    storedBytes.insert(storedBytes.end(), bytes, bytes + sizeof(uint32_t));
                    if(toKeyBytes == colKey){
                        minValue = inputNumber;
                    }
                }else{
                    if(tokens[i].length() >= columnBytes){
                        std::cout<<"ERROR: This row at column["<<columnName<<"], exceed column size!"<<std::endl;
                        exit(0);
                    }
                    fullOutput = fullOutput + tokens[i] + "|";
                    unsigned char inputArray[columnBytes] = {0};
                    std::memcpy(inputArray, tokens[i].c_str(), tokens[i].length());
                    size_t length = columnBytes;
                    toKeyBytes += columnBytes;
                    storedBytes.insert(
                        storedBytes.end(),
                        inputArray,                 
                        inputArray + length
                    );
                }
            }
            // Deleted column
            bool deletedColumn = 0;
            unsigned char* bytes = reinterpret_cast<unsigned char*>(&deletedColumn);
            storedBytes.insert(storedBytes.end(), bytes, bytes + sizeof(bool));
            std::cout << "====== Row parsed ======" << std::endl;
            std::cout << "row: " <<fullOutput << std::endl;
            logFile.close();
            insertionRow temp;
            temp.row = storedBytes;
            temp.min = minValue;
            // exit(0);
            return temp;
        }else{
            std::cout<<"====== File insertion failed ======" << std::endl;
            exit(0);
        }
    }else{
        uint32_t numOfRow;
        logFile.read(reinterpret_cast<char*>(&numOfRow),sizeof(numOfRow));
        uint8_t columnCount;
        logFile.read(reinterpret_cast<char*>(&columnCount),sizeof(columnCount));
        uint16_t totalBytes;
        logFile.read(reinterpret_cast<char*>(&totalBytes),sizeof(totalBytes));
        uint16_t colKey;
        logFile.read(reinterpret_cast<char*>(&colKey),sizeof(colKey));

        std::cout <<"\n"<<"====== Config details ======" << std::endl; 
        std::cout << "number of rows: " << numOfRow << std::endl;
        std::cout << "column count: " << static_cast<int>(columnCount) << std::endl;
        std::cout << "totalbytes: " << totalBytes-1 << std::endl;
        std::cout << "KeyBytes: " << colKey << std::endl;

        std::vector<unsigned char> storedBytes;
        storedBytes.reserve(totalBytes);
        std::string fullOutput;
        uint32_t minValue = 0;
        int toKeyBytes = 0;
        for(int i = 0; i <  static_cast<int>(columnCount); i++){
            char columnName[30];
            logFile.read(columnName, 30);
            bool isChar;
            logFile.read(reinterpret_cast<char*>(&isChar),sizeof(isChar));
            uint8_t  columnBytes;
            // Char
            std::string isCharString;
            if(isChar == 1){
                isCharString = "true";
                logFile.read(reinterpret_cast<char*>(&columnBytes),1);
            }else{
                isCharString = "false";
                logFile.read(reinterpret_cast<char*>(&columnBytes),1);
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
                        toKeyBytes += 4;
                        fullOutput = fullOutput + std::to_string(inputNumber) + "|";
                        unsigned char* bytes = reinterpret_cast<unsigned char*>(&inputNumber);
                        storedBytes.insert(storedBytes.end(), bytes, bytes + sizeof(uint32_t));
                        if(toKeyBytes == colKey){
                            minValue = inputNumber;
                        }
                        break;
                    }
                }else{
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    char inputBuffer[columnBytes] = {0};
                    std::cout << "Input data: ";
                    std::cin.getline(inputBuffer, columnBytes);
                    if(std::cin.fail()){
                        invalidAction();
                        continue;
                    }else{
                        fullOutput = fullOutput + inputBuffer + "|";
                        size_t length = columnBytes;
                        toKeyBytes += columnBytes;
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
        // Deleted column
        bool deletedColumn = 0;
        unsigned char* bytes = reinterpret_cast<unsigned char*>(&deletedColumn);
        storedBytes.insert(storedBytes.end(), bytes, bytes + sizeof(bool));
        std::cout << "====== Inputted data ======" << std::endl;
        std::cout << "row: " <<fullOutput << std::endl;
        logFile.close();
        insertionRow temp;
        temp.row = storedBytes;
        temp.min = minValue;
        return temp;   
    }
}