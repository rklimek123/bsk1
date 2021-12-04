#ifndef OFFICERFILES_H
#define OFFICERFILES_H

#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <time.h>

#define FILE_OTHER  -1
#define FILE_DEPOSIT 0
#define FILE_CREDIT  1

typedef struct filecontent {
    int fd;
    int file_type;
    FILE* stream;
    time_t start_date;
    time_t latest_date;
    int32_t number;
} filecontent_t;

int parse_date(char* date, time_t* result);

int convert_stream(FILE* file, filecontent_t* filecontent, int fd, int type);

int sort_files(filecontent_t* files[], int files_count);

void print_files(filecontent_t* files[], int files_count);

void free_files(filecontent_t* files[], int files_count);


#endif /* OFFICERFILES_H */