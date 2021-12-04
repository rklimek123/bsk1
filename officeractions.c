#include "officeractions.h"

static void fake_getline() {
    char* temp;
    size_t n = 0;
    // Fake getline to get empty '\n'
    getline(&temp, &n, stdin);
    free(temp);
}

static int prompt(const char* msg, char** result, size_t* n) {
    char* temp;
    
    printf("%s", msg);
    ssize_t ret = getline(&temp, n, stdin);
    if (ret < 1) {
        fprintf(stderr, "Getline error\n");
        exit(1);
    }

    *n = ret - 1;
    *result = malloc(ret);

    if (*result == NULL) {
        free(temp);
        fprintf(stderr, "Malloc error\n");
        exit(1);
    }

    memcpy(*result, temp, ret - 1);
    *(*result + ret - 1) = '\0';
    return 0;
}

static bool client_gid_setup = false;
static gid_t client_gid;

static int get_client_gid() {
    struct group* grp = getgrnam("client");

    if (!grp) {
        return -1;
    }

    client_gid = grp->gr_gid;
    client_gid_setup = true;

    // Freeing group pointer is forbidden by "man getgrnam"
    return 0;
}

static int selectuser_raw(user_t** result, const char* username) {
    // Freeing previous client is forbidden by "man getpwnam"
    *result = getpwnam(username);
    if (!(*result)) {
        // user not found
        return -1;
    }

    if (!client_gid_setup) {
        if (get_client_gid() != 0) {
            fprintf(stderr, "Group 'client' doesn't exist\n");
            exit(1);
        }
    }

    // The tasks specification only permits one group per user.
    // That's why checking group isn't done with "getgrouplist" function.
    if ((*result)->pw_gid != client_gid) {
        // user is not client
        
        // Freeing previous client is forbidden by "man getpwnam"
        *result = NULL;
        return -1;
    }

    return 0;
}

int action1_selectuser(user_t** result) {
    char* username;
    size_t username_len = 0;

    fake_getline();

    if (prompt("Input username: ", &username, &username_len) != 0) {
        return -1;
    }

    int ret = selectuser_raw(result, username);
    free(username);

    return ret;
}


static size_t count_files_in_dir(const user_t* user, const char* dirname) {
    
    size_t result = 0;
    DIR *dirStream = NULL;
    struct stat buf;

    // Shouldn't be freed ("man readdir")
    struct dirent *dirEntry = NULL;

    if ((dirStream=opendir(dirname)) == NULL ) {
        fprintf(stderr, "Directory %s doesn't exist\n", dirname);
        exit(1);
    }

    if (chdir(dirname) != 0) {
        fprintf(stderr, "chdir error\n");
        closedir(dirStream);
        exit(1);
    }

    while((dirEntry = readdir(dirStream)) != NULL) {
        if (dirEntry->d_type == DT_REG) {
            if (user == NULL) {
                ++result;
            }
            else {
                lstat(dirEntry->d_name, &buf);
                if (buf.st_uid == user->pw_uid) {
                    ++result;
                }
            }
        }
    }

    closedir(dirStream);
    chdir("..");

    return result;
}

static size_t count_files(const user_t* user) {
    return count_files_in_dir(user, "credits") +
           count_files_in_dir(user, "deposits");
}

static int load_file(const user_t* user, filecontent_t** file, const char* fname, int type) {

    *file = malloc(sizeof(filecontent_t));
    if (*file == NULL) {
        fprintf(stderr, "Malloc error\n");
        return 1;
    }

    FILE* f = fopen(fname, "r");
    if (f == NULL) {
        fprintf(stderr, "fopen %s error\n", fname);
        free(*file);
        return 1;
    }

    int fd = fileno(f);
    if (fd == -1) {
        fclose(f);
        free(*file);
        return 1;
    }

    if (flock(fd, LOCK_SH) != 0) {
        fclose(f);
        free(*file);
        return 1;
    }

    if (convert_stream(f, *file, fd, type) != 0) {
        flock(fd, LOCK_UN);
        fclose(f);
        free(*file);
        return 1;
    }

    return 0;
}

static int load_files_from_dir(const user_t* user, filecontent_t* files[], size_t files_count, size_t* next_index, int type) {
    size_t result = 0;
    DIR *dirStream = NULL;
    struct stat buf;

    // Shouldn't be freed ("man readdir")
    struct dirent *dirEntry = NULL;

    const char* dirname;
    if (type == FILE_CREDIT) {
        dirname = "credits";
    }
    else if (type == FILE_DEPOSIT) {
        dirname = "deposits";
    }

    if ((dirStream=opendir(dirname)) == NULL ) {
        fprintf(stderr, "Directory %s doesn't exist\n", dirname);
        exit(1);
    }

    if (chdir(dirname) != 0) {
        fprintf(stderr, "chdir error\n");
        closedir(dirStream);
        return 1;
    }

    while((dirEntry = readdir(dirStream)) != NULL) {
        if (dirEntry->d_type == DT_REG) {
            lstat(dirEntry->d_name, &buf);
            if (user == NULL || buf.st_uid == user->pw_uid) {
                if (load_file(user, files + *next_index, dirEntry->d_name, type) != 0) {
                    fprintf(stderr, "Load file error\n");
                    return 1;
                }
                *next_index = *next_index + 1;
            }
        }
    }

    closedir(dirStream);
    chdir("..");

    return result;
}

static int load_files(const user_t* user, filecontent_t* files[], size_t files_count) {
    size_t next_index = 0;
    if (files_count == 0) {
        return 0;
    }

    if (load_files_from_dir(user, files, files_count, &next_index, FILE_CREDIT) != 0) {
        free_files(files, next_index);
        fprintf(stderr, "Load files from dir '/credits' error\n");
        return 1;
    }
    if (load_files_from_dir(user, files, files_count, &next_index, FILE_DEPOSIT) != 0) {
        free_files(files, next_index);
        fprintf(stderr, "Load files from dir '/deposits' error\n");
        return 1;
    }

    return 0;
}

int action2_getfiles(const user_t* user) {
    if (user == NULL) {
        fprintf(stderr, "User is null\n");
        return 1;
    }

    size_t files_count = count_files(user);
    filecontent_t* files[files_count];

    printf("Loading %ld files\n", files_count);
    if (load_files(user, files, files_count) != 0) {
        fprintf(stderr, "Load files error\n");
        return 1;
    }

    if (sort_files(files, files_count) != 0) {
        free_files(files, files_count);
        fprintf(stderr, "Sort files error\n");
        return 1;
    }

    print_files(files, files_count);

    free_files(files, files_count);

    return 0;
}


static int a3_getType() {
    char* typestr;
    size_t typestr_len = 0;
    user_t* result;

    if (prompt("File type: ", &typestr, &typestr_len) != 0) {
        fprintf(stderr, "Op. 3: Input filetype error\n");
        return FILE_OTHER;
    }

    int ret;
    if (strcmp(typestr, "credit") == 0) {
        ret = FILE_CREDIT;
    }
    else if (strcmp(typestr, "deposit") == 0) {
        ret = FILE_DEPOSIT;
    }
    else {
        ret = FILE_OTHER;
    }

    free(typestr);
    return ret;
}

static const user_t* a3_getUser(const user_t* defaultUser) {
    char* username;
    size_t username_len = 0;
    user_t* result = NULL;

    if (prompt("Username: ", &username, &username_len) != 0) {
        fprintf(stderr, "Op. 3: Input username error\n");
        return NULL;
    }

    if (strlen(username) == 0 && defaultUser != NULL) {
        printf("\e[30mUsing the default user %s\n\e[0m", defaultUser->pw_name);
        return defaultUser;
    }
    else {
        if (selectuser_raw(&result, username) != 0) {
            result = NULL;
        }
    }

    free(username);
    return result;
}

static int a3_getDouble(const char* prompt_msg, double* result) {
    char* dstr;
    size_t dstr_len = 0;
    
    if (prompt(prompt_msg, &dstr, &dstr_len) != 0) {
        fprintf(stderr, "Op. 3: Input double error\n");
        return -1;
    }

    char* returned = NULL;
    double ret = strtod(dstr, &returned);

    if (ret == 0 && returned == dstr) {
        return -1;
    }
    
    *result = ret;
    return 0;
}

static double a3_getPositiveDouble(const char* prompt_msg) {
    char* dstr;
    size_t dstr_len = 0;
    
    if (prompt(prompt_msg, &dstr, &dstr_len) != 0) {
        fprintf(stderr, "Op. 3: Input double error\n");
        return -1;
    }

    char* returned = NULL;
    double ret = strtod(dstr, &returned);

    if (ret <= 0) {
        ret = -1;
    }

    return ret;
}

static time_t a3_getDate(const char* prompt_msg, char** datestr_result) {
    char* datestr;
    size_t datestr_len = 0;

    if (prompt(prompt_msg, &datestr, &datestr_len) != 0) {
        fprintf(stderr, "Op. 3: Input date error\n");
        return -1;
    }

    time_t result = 0;
    int ret = parse_date(datestr, &result);
    if (ret != 0) {
        free(datestr);
        return -1;
    }

    *datestr_result = datestr;
    return result;
}

static int32_t getNextNumber(int type) {
    const char* dirc;
    if (type == FILE_CREDIT) {
        dirc = "/credits";
    }
    else if (type == FILE_DEPOSIT) {
        dirc = "/deposits";
    }
    else {
        fprintf(stderr, "Get next number\n");
        exit(1);
    }

    size_t files_count = count_files_in_dir(NULL, dirc);
    filecontent_t* files[files_count];
    size_t next_index = 0;

    if (load_files_from_dir(NULL, files, files_count, &next_index, type) != 0) {
        free_files(files, next_index);
        fprintf(stderr, "Load files from dir error\n");
        exit(1);
    }
    
    int32_t result = 0;

    for (int i = 0; i < files_count; ++i) {
        result = files[i]->number > result ? files[i]->number : result;
    }

    free_files(files, files_count);
    return result + 1;
}

void a3_numbertostr(int type, int32_t number, char* buf) { 
    sprintf(buf, "%c%d.txt", type == FILE_CREDIT ? 'c' : 'd', number);
}

void writeOrDie(int fd, const char* str) {
    size_t n = strlen(str);

    ssize_t ret = write(fd, str, n);
    if (ret != n) {
        fprintf(stderr, "Couldn't write to file\n");
        exit(1);
    }
}

void place_canaries(char* buffer, size_t size) {
    if (size >= 4) {
        buffer[size - 4] = '\0';
        buffer[size - 3] = 233;
        buffer[size - 2] = 179;
        buffer[size - 1] = 165;
    }
}

bool check_canaries(char* buffer, size_t size) {
    return
        buffer[size - 3] == 233 &&
        buffer[size - 2] == 179 &&
        buffer[size - 1] == 165;
}

int a3_newFile(const user_t* user, int type, double sum, char* start_date_str, double procent, int32_t number) {
    const char* dirc;
    if (type == FILE_CREDIT) {
        dirc = "/credits";
    }
    else if (type == FILE_DEPOSIT) {
        dirc = "/deposits";
    }
    else {
        exit(1);
    }

    if (chdir(dirc) != 0) {
        exit(1);
    }

    char filename[20];
    a3_numbertostr(type, number, filename);
    
    int fd = open(filename, O_WRONLY | O_CREAT);
    if (fd < 0) {
        fprintf(stderr, "Couldn't open file \n");
        exit(1);
    }

    ssize_t ret;
    size_t buf_size = 512;
    char buf[buf_size];
    
    writeOrDie(fd, "Name: ");
    writeOrDie(fd, user->pw_gecos);

    writeOrDie(fd, "\nNumber: ");
    snprintf(buf, buf_size, "%d", number);
    writeOrDie(fd, buf);
    
    writeOrDie(fd, "\nSum: ");
    snprintf(buf, buf_size, "%f", sum);
    writeOrDie(fd, buf);

    writeOrDie(fd, "\nDate: ");
    writeOrDie(fd, start_date_str);

    writeOrDie(fd, "\nProcent: ");
    snprintf(buf, buf_size, "%f", procent);
    writeOrDie(fd, buf);
    writeOrDie(fd, "\n");

    snprintf(buf, buf_size, "chmod 600 %s", filename);
    system(buf);

    snprintf(buf, buf_size, "setfacl -m group:client:--- %s", filename);
    system(buf);

    snprintf(buf, buf_size, "setfacl -m group:officer:rw- %s", filename);
    system(buf);

    snprintf(buf, buf_size, "sudo chown %s:client %s", user->pw_name, filename);
    system(buf);

    close(fd);
    chdir("..");
    return 0;
}

int action3_addfile(const user_t* defaultUser) {
    int type;
    const user_t* user;
    double sum;
    char* start_date_str;
    time_t start_date;
    double procent;

    fake_getline();

    type = a3_getType();
    if (type != FILE_CREDIT && type != FILE_DEPOSIT) {
        return A3_TYPE;
    }

    user = a3_getUser(defaultUser);
    if (user == NULL) {
        return A3_CLIENT;
    }

    sum = a3_getPositiveDouble("Sum: ");
    if (sum == -1) {
        return A3_SUM;
    }

    start_date = a3_getDate("Start date: ", &start_date_str);
    if (start_date == -1) {
        return A3_DATE;
    }

    int ret = a3_getDouble("Procent: ", &procent);
    if (ret == -1) {
        free(start_date_str);
        return A3_PROCENT;
    }


    const char* lck;
    if (type == FILE_CREDIT) {
        lck = "/add_credit.lck";
    }
    else if (type == FILE_DEPOSIT) {
        lck = "/add_deposit.lck";
    }

    int fd = open(lck, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Couldn't open file%s, type %d\n", lck, type);
        return A3_OTHER;
    }

    if (flock(fd, LOCK_EX) != 0) {

        free(start_date_str);
        return A3_OTHER;
    }

    int32_t number = getNextNumber(type);

    a3_newFile(user, type, sum, start_date_str, procent, number);

    flock(fd, LOCK_UN);
    close(fd);
    
    return A3_OK;
}


int action4_editfile(const user_t* user) {
    return 0;
}
