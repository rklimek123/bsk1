#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>

#define PAM_SM_AUTH

#define _PAM_EXTERN_FUNCTIONS
#include <security/_pam_macros.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

PAM_EXTERN int pam_sm_authenticate(pam_handle_t * pamh, int flags
				   ,int argc, const char **argv)
{
    int retval;

	srand(time(NULL));
    int token_int = rand();
    char token[12];
    sprintf(token, "%d", token_int);

    char* response;
    retval = pam_prompt(pamh, PAM_TEXT_INFO, &response, "Token to repeat: %s\n", token);
    free(response);

    retval = pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &response, "Repeat the token: ");
	if (retval != PAM_SUCCESS) {
		fprintf(stderr, "Authorization token provide error\n");
        free(response);
		return PAM_PERM_DENIED;
	}

    if (strcmp(response, token) != 0) {
        fprintf(stderr, "Wrong authorization token provided\n");
        free(response);
		return PAM_PERM_DENIED;
    }

    free(response);
	return PAM_SUCCESS;
}


#ifdef PAM_STATIC
struct pam_module _pam_randcode_modstruct = {
    "pam_randcode",
    pam_sm_authenticate,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
#endif