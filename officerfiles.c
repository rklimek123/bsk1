#include "officerfiles.h"

int parse_date(char* date, time_t* result) {
    // DD-MM-YYYY
    date[2] = '\0';
    date[5] = '\0';
    date[10] = '\0';

    int day = atoi(date);
    if (day == 0) {
        fprintf(stderr, "Parse day error\n");
        return 1;
    }

    int month = atoi(date + 3);
    if (month == 0) {
        fprintf(stderr, "Parse month error\n");
        return 1;
    }

    int year = atoi(date + 6);
    if (year == 0) {
        fprintf(stderr, "Parse year error\n");
        return 1;
    }

    // Not unix epoch, but some other time, which allows for comparisons
    *result = day + 100*month + 10000*year;
    date[2] = '.';
    date[5] = '.';
    return 0;
}

static int get_start_date(FILE* file, time_t* result) {
    char* buf = NULL;
    size_t n = 0;

    while (getline(&buf, &n, file) != -1) {
        if (n >= 1) {
            if (buf[0] == 'D') {
                buf[16] = '\0';
                if (parse_date(buf + 6, result) != 0) {
                    fprintf(stderr, "Parse start date error\n");
                    free(buf);
                    rewind(file);
                    return 1;
                }

                free(buf);
                rewind(file);
                return 0;
            }
        }
        
    }

    free(buf);
    rewind(file);
    return -1;
}

static int get_latest_date(FILE* file, time_t* result) {
    char* buf = NULL;
    size_t n = 0;

    bool datefound = false;

    while (getline(&buf, &n, file) != -1) {
        if (n >= 1) {
            if (buf[0] == 'D') {
                buf[16] = '\0';
                if (parse_date(buf + 6, result) != 0) {
                    fprintf(stderr, "Parse latest date error\n");
                    free(buf);
                    rewind(file);
                    return 1;
                }

                datefound = true;
            }
        }
    }

    free(buf);
    rewind(file);
    return !datefound;
}

static int get_number(FILE* file, int32_t* result) {
    char* buf = NULL;
    size_t n = 0;

    while (getline(&buf, &n, file) != -1) {
        if (n >= 2) {
            if (buf[0] == 'N' && buf[1] == 'u') {
                buf[n - 1] = '\0';

                *result = atoi(buf + 8);
                rewind(file);
                free(buf);

                if (*result == 0) {
                    fprintf(stderr, "Parse number error\n");
                    return 1;
                }
                
                return 0;
            }
        }
    }

    free(buf);
    rewind(file);
    return -1;
}

int convert_stream(FILE* file, filecontent_t* filecontent, int fd, int type) {
    filecontent->fd = fd;
    filecontent->file_type = type;
    filecontent->stream = file;

    if (get_start_date(file, &(filecontent->start_date))   != 0 ||
        get_latest_date(file, &(filecontent->latest_date)) != 0 ||
        get_number(file, &(filecontent->number))           != 0
    ) {
        fprintf(stderr, "Particular convert filestream error\n");
        return 1;
    }

    return 0;
}


int file_compare(const void* a, const void* b)
{
    int32_t start_date_a = ((filecontent_t*)a)->start_date;
    int32_t start_date_b = ((filecontent_t*)b)->start_date;
    
    return start_date_a - start_date_b;
}

int sort_files(filecontent_t* files[], int files_count) {
    qsort(files, files_count, sizeof(filecontent_t*), file_compare);
}


static void print_file(int index, filecontent_t* file) {
    printf("File %d | ", index);
    if (file->file_type == FILE_CREDIT) {
        printf("CREDIT");
    }
    else if (file->file_type == FILE_DEPOSIT) {
        printf("DEPOSIT");
    }
    else {
        printf("UNKNOWN TYPE");
    }
    printf("\n");

    char* buf = NULL;
    size_t n = 0;

    while (getline(&buf, &n, file->stream) != -1) {
        printf(buf);
    }

    rewind(file->stream);
    free(buf);
}

void print_files(filecontent_t* files[], int files_count) {
    for (int i = 0; i < files_count; ++i) {
        printf("\n\e[38;5;238m");
        printf("--------------------------------------------------------------------------------\n");
        printf("\n\e[0m");
        print_file(i + 1, files[i]);
    }
}

void free_files(filecontent_t* files[], int files_count) {
    for (int i = 0; i < files_count; ++i) {
        flock(files[i]->fd, LOCK_UN);
        fclose(files[i]->stream);
        free(*(files + i));
    }
}
