#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>

static struct pam_conv login_conv = {
    misc_conv,
    NULL
};
 
int main () {
    pam_handle_t* loginh = NULL;
    int retval;
    char *username = NULL;
  
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

    printf("Hello world!\n");
    
    pam_end(loginh, PAM_SUCCESS);
    return 0;
}
