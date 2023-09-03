//
// Created by ACER on 03/09/2023.
//

#ifndef BD2_PROJECT1_SEQUENTIALFILE_H
#define BD2_PROJECT1_SEQUENTIALFILE_H

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <algorithm>

template <typename T>
class SequentialFile {
    std::string filename;
    std::fstream file;
    std::unordered_map<std::string, int> index;
    std::vector<T> buffer;
    int buffer_size;
    int buffer_index;
    int file_size;
    int file_index;
    bool is_open;

}
#endif //BD2_PROJECT1_SEQUENTIALFILE_H
