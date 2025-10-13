#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

#include "datapage.h"

struct numFile {int mode; std::string fileName;};

struct insertionRow
{
    std::vector<unsigned char> row;
    uint32_t min;
};

struct isReadFile{ bool isRead; std::fstream* readFile;};

enum queryType {CREATE, INSERT, UPDATE, DELETE, SELECT, EMP};

numFile readInitialInput(bool isLoop, std::string modeType);

void readInputColumn(datapage* datapageName, isReadFile readingFile);

insertionRow readInsertion(std::string fileName, bool isFile, std::string readLine);