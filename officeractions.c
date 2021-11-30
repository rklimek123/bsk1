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

int action1_selectuser(user_t** result) {
    char* username;
    size_t username_len;

    if (prompt("Input username: ", &username, &username_len) != 0) {
        return -1;
    }

    // Freeing previous client is forbidden by "man getpwnam"
    *result = getpwnam(username);
    if (!(*result)) {
        // user not found
        return -1;
    }
    free(username);

    if (!client_gid_setup) {
        if (get_client_gid() != 0) {
            fprintf(stderr, "Group client doesn't exist\n");
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


int action2_getfiles(const user_t* user, filecontent_t** result, int* result_size) {
    return 0;
}


int action3_addfile(const user_t* user) {
    return 0;
}


int action4_editfile(const user_t* user) {
    return 0;
}
