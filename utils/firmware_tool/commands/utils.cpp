#include "commands.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

bool write_binary(std::string file, uint8_t* data, size_t data_len) {
    std::ofstream out(file, std::ios_base::binary);
    if (!out) {
        return false;
    }
    out.write((char*)data, data_len);
    out.close();
    return true;
}
/*int binary_writer(char *file_name, uint8_t*buffer, int len) {
    int file = open(file_name, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (file < 0) {
        printf ("Error: impossible to write the file %s\n", file_name);
        return 1;
    }
    if (write(file, buffer, len) != len) {
        printf ("Error: file not correcly written\n");
        return 1;
    }
    close(file);
    return 0;
}

int binary_reader(char *file_name, uint8_t*buffer, int len) {
    int file = open(file_name, O_RDONLY);
    if (file < 0) {
        printf ("Impossible to read the file %s\n", file_name);
        return 1;
    }
    if (read(file, buffer, len) != len) {
        printf ("Error, impossible to read the file!\n");
        return 1;
    }
    close(file);
    return 0;
}
*/

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


