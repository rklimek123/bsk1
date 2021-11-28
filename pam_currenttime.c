#include <stdbool.h>
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

bool checktimestamp(time_t timestamp);

PAM_EXTERN int pam_sm_authenticate(pam_handle_t * pamh, int flags,
                                   int argc, const char **argv)
{
    int retval;

    char* response;

    retval = pam_prompt(pamh, PAM_PROMPT_ECHO_ON, &response,
                "What time is it? (UNIX Epoch, run the \"python3 clock.py\" command): ");
	if (retval != PAM_SUCCESS) {
		fprintf(stderr, "PAM error while acquiring the timestamp\n");
        free(response);
		return PAM_PERM_DENIED;
	}

    if (!checktimestamp(atol(response))) {
        free(response);
		return PAM_PERM_DENIED;
    }

    free(response);
	return PAM_SUCCESS;
}

bool checktimestamp(long timestamp) {
    long now = (long)time(NULL);
    return labs(now - timestamp) < 15;
}

#ifdef PAM_STATIC
struct pam_module _pam_currenttime_modstruct = {
    "pam_currenttime",
    pam_sm_authenticate,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
#endif