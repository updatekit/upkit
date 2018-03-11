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

void print_hex(char *title, unsigned char *buf, int buf_len) {
    int i = 0;
    fprintf(stdout, "%s\n", title);
    for(i = 0; i < buf_len; ++i) {
        fprintf(stdout, "0x%02x%s", buf[i], ( i + 1 ) % 16 == 0 ? ",\r\n" : "," );
    }
    printf("\n");
}

static void print_buf(char *title, unsigned char *buf, int buf_len) {
    int i = 0;
    fprintf(stdout, "%s\n", title);
    for(i = 0; i < buf_len; ++i) {
        fprintf(stdout, "0x%02x%s", buf[i], ( i + 1 ) % 16 == 0 ? ",\r\n" : "," );
    }
    printf("\n");
}
int check_exists(std::string path) {
    struct stat buf;
    return !stat(path.c_str(), &buf);
}*/
