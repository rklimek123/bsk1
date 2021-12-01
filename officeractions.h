#ifndef OFFICERACTIONS_H
#define OFFICERACTIONS_H

#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include "officerfiles.h"

typedef struct passwd user_t;

// Password structure from <pwd.h>:
// struct passwd {
//     char   *pw_name;       /* username */
//     char   *pw_passwd;     /* user password */
//     uid_t   pw_uid;        /* user ID */
//     gid_t   pw_gid;        /* group ID */
//     char   *pw_gecos;      /* user information */
//     char   *pw_dir;        /* home directory */
//     char   *pw_shell;      /* shell program */
// };


int action1_selectuser(user_t** result);

int action2_getfiles(const user_t* user);

int action3_addfile(const user_t* user);

int action4_editfile(const user_t* user);


#endif /* OFFICERACTIONS_H */
