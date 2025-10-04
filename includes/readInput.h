#include <iostream>
#include <string>
#include <vector>

#include "datapage.h"

struct numFile {int mode; std::string fileName;};

enum queryType {CREATE, INSERT, UPDATE, DEL, SELECT, EMP};

numFile readInitialInput();

void readInputColumn(datapage* datapageName);

std::vector<unsigned char> readInsertion(std::string fileName);