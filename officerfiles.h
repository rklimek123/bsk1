#ifndef OFFICERFILES_H
#define OFFICERFILES_H

#define FILE_DEPOSIT 0
#define FILE_CREDIT  1

#include <stdio.h>

typedef struct filecontent {
    int file_type;
    const char* content;

} filecontent_t;


int print_file(int index, filecontent_t file);

int print_files(filecontent_t* files, int files_count);


#endif /* OFFICERFILES_H */