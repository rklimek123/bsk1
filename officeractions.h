#ifndef OFFICERACTIONS_H
#define OFFICERACTIONS_H

#include <pwd.h>
#include "officerfiles.h"

typedef struct passwd user_t;


int action1_selectuser(user_t* result);

int action2_getfiles(const user_t user, filecontent_t** result, int* result_size);

int action3_addfile(const user_t user);

int action4_editfile(const user_t user);


#endif /* OFFICERACTIONS_H */
