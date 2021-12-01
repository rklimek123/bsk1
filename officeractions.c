#include "officeractions.h"

static int prompt(const char* msg, char** result, size_t* n) {
    char* temp;
    
    printf("%s", msg);

    // Fake getline to get empty '\n'
    getline(&temp, n, stdin);

    // Meaningful getline
    ssize_t ret = getline(&temp, n, stdin);
    
    if (ret <= 1) {
        fprintf(stderr, "Getline error\n");
        return -1;
    }

    *n = ret - 1;
    *result = malloc(ret);

    if (*result == NULL) {
        free(temp);
        fprintf(stderr, "Malloc error\n");
    }

    memcpy(*result, temp, ret - 1);
    *(*result + ret - 1) = '\0';

    free(temp);
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

static int action1_selectuser_raw(user_t** result, const char* username) {
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
    size_t username_len;

    if (prompt("Input username: ", &username, &username_len) != 0) {
        return -1;
    }

    int ret = action1_selectuser_raw(result, username);
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
        return 1;
    }

    while((dirEntry = readdir(dirStream)) != NULL) {
        if (dirEntry->d_type == DT_REG) {
            lstat(dirEntry->d_name, &buf);
            if (buf.st_uid == user->pw_uid) {
                ++result;
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
            if (buf.st_uid == user->pw_uid) {
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


int action3_addfile(const user_t* user) {
    return 0;
}


int action4_editfile(const user_t* user) {
    return 0;
}
