#include "commands.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>

bool write_binary(std::string file, uint8_t* data, size_t data_len) {
    std::ofstream out(file, std::ios_base::binary);
    if (!out) {
        return false;
    }
    out.write((char*)data, data_len);
    out.close();
    return true;
}

bool file_exists(std::string file_name) {
    struct stat buffer;   
    return (stat(file_name.c_str(), &buffer) == 0);
}

void print_buf(const unsigned char* data, size_t data_len, size_t offset) {
    for(size_t i = 0; i < data_len; ++i) { 
        std::cout << std::setfill('0');
        std::cout << std::hex << std::setw(2) << (int)data[i];
        if (((i + 1) % 16) == 0 && i != data_len-1) {
            std::cout << std::setfill(' ');
            std::cout << std::endl << std::setw(offset) << " ";
        } else {
            std::cout << " ";
        }
    }
    std::setfill(' ');
    std::cout << std::endl;
}


