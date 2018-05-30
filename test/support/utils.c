#include <stdio.h>
#include "utils.h"

int file_compare(char* fname1, char* fname2) {
    FILE *fp1, *fp2;
    int ch1, ch2;
    fp1 = fopen(fname1, "r");
    if (fp1 == NULL) {
        printf("Cannot open %s for reading\n", fname1);
        return 1;
    }
    fp2 = fopen(fname2, "r");
    if (fp2 == NULL) {
        printf("Cannot open %s for reading\n", fname2);
        return 1;
    }
    do {
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    } while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2));
    fclose(fp1);
    fclose(fp2);
    return (ch1 == ch2)? 0: 1;
}
