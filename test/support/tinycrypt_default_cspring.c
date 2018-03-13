#if defined(unix) || defined(__linux__) || defined(__unix__) || \
    defined(__unix) |  (defined(__APPLE__) && defined(__MACH__)) || \
    defined(uECC_POSIX)

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif

int default_CSPRNG(uint8_t *dest, unsigned int size) {

    /* input sanity check: */
    if (dest == (uint8_t *) 0 || (size <= 0))
        return 0;

    int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        fd = open("/dev/random", O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            return 0;
        }
    }

    char *ptr = (char *)dest;
    size_t left = (size_t) size;
    while (left > 0) {
        ssize_t bytes_read = read(fd, ptr, left);
        if (bytes_read <= 0) { // read failed
            close(fd);
            return 0;
        }
        left -= bytes_read;
        ptr += bytes_read;
    }

    close(fd);
    return 1;
}

#endif
