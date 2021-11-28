#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static struct pam_conv login_conv = {
    misc_conv,
    NULL
};

pam_handle_t* loginh = NULL;

void sigint_handler(int signum) {
    pam_end(loginh, PAM_SUCCESS);
    exit(0);
}

void workflow() {
    
}

int main() {
    int retval;
    char *username = NULL;

    signal(SIGINT, sigint_handler);
  
    retval = pam_start("officerapp", username, &login_conv, &loginh);
    if (loginh == NULL || retval != PAM_SUCCESS) {
        fprintf(stderr, "Error when starting authentication service: %d\n", retval);
        exit(1);
    }
  
    retval = pam_authenticate(loginh, 0);
    if (retval != PAM_SUCCESS) {
        fprintf(stderr, "Access to officer app denied!\n");
        exit(2);
    }

    for (;;) {
        workflow();
    }
    
    pam_end(loginh, PAM_SUCCESS);
    return 0;
}
