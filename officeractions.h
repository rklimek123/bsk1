#ifndef OFFICERACTIONS_H
#define OFFICERACTIONS_H

#include <dirent.h>
#include <errno.h>
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


#define A3_OK      0
#define A3_CLIENT  1
#define A3_SUM     2
#define A3_DATE    3
#define A3_PROCENT 4
#define A3_TYPE    5
#define A3_OTHER  -1

int action3_addfile(const user_t* defaultUser);


#define A4_OK      0
#define A4_INDEX   1
#define A4_MOVE    2
#define A4_SUM     3
#define A4_DATE    4
#define A4_PROCENT 5
#define A4_OTHER  -1

int action4_editfile(const user_t* user);


#endif /* OFFICERACTIONS_H */
