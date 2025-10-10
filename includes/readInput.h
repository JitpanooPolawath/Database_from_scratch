#include <iostream>
#include <string>
#include <vector>

#include "datapage.h"

struct numFile {int mode; std::string fileName;};

struct insertionRow
{
    std::vector<unsigned char> row;
    uint32_t min;
};


enum queryType {CREATE, INSERT, UPDATE, DEL, SELECT, EMP};

numFile readInitialInput(bool isLoop);

void readInputColumn(datapage* datapageName);

insertionRow readInsertion(std::string fileName);