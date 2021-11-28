#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>

static struct pam_conv login_conv = {
    misc_conv,
    NULL
};
 
int main () {
    printf("Hello world!\n");
    return 0;
    /*
    pam_handle_t* loginh = NULL;
    int retval;
    char *username = NULL;
  
    retval = pam_start("zad2bsk", username, &login_conv, &loginh);
    if (loginh == NULL || retval != PAM_SUCCESS) {
        fprintf(stderr, "Error when starting: %d\n", retval);
        exit(1);
    }
  
    retval = pam_authenticate(loginh, 0);
    if (retval != PAM_SUCCESS) {
        fprintf(stderr, "Chyba się nie udało!\n");
        exit(2);
    }

    printf("Wykonanie kodu aplikacji\n");
    
    pam_end(loginh, PAM_SUCCESS);*/
    exit(0);
}
