#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "officeractions.h"
#include "officerfiles.h"

static struct pam_conv login_conv = {
    misc_conv,
    NULL
};

pam_handle_t* loginh = NULL;


void sigint_handler(int signum) {
    pam_end(loginh, PAM_SUCCESS);
    exit(0);
}

user_t* current_client = NULL;

void print_error(const char* msg) {
    printf("\e[31m[ERROR] ");
    printf(msg);
    printf("\e[0m");
}

void print_highlight(const char* msg) {
    printf("\e[42m");
    printf(msg);
    printf("\e[0m");
}

void print_bar() {
    printf("\n\e[38;5;240m");
    printf("--------------------------------------------------------------------------------\n");
    printf("********************************************************************************\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("\n\e[0m");
}

void print_dashboard() {
    if (current_client) {
        printf("Selected client: ");
        print_highlight(current_client->pw_name);
        printf("\n");
    }
    else {
        printf("No client selected\n");
    }
    printf("\n");
    printf("Actions:\n");
    printf("1. Select a client, for whom you want to take actions\n");
    printf("2. List all deposits and credits of the selected client\n");
    printf("3. Add deposit / add credit\n");
    printf("4. Edit deposit / edit credit\n");
    printf("\n");
}

int prompt_action() {
    int got;

    while (scanf("%d", &got) == EOF || got < 1 || got > 4) {
        printf("Provide a number between 1 and 4, corresponding to the action you want to select\n");
    }

    return got;
}

void workflow() {
    print_bar();
    print_dashboard();
    int action = prompt_action();

    if (action != 1 && current_client == NULL) {
        print_error("Select a client before doing this operation\n");
        return;
    }

    switch (action) {
        case 1:
            if (action1_selectuser(&current_client) != 0) {
                print_error("Client not found\n");
            }
            
            break;

        case 2:
            action2_getfiles(current_client);
            break;

        case 3:
            action3_addfile(current_client);
            break;
        
        case 4:
            action4_editfile(current_client);
            break;
        
        default:
            fprintf(stderr, "Unexpected error!\n");
            exit(1);
    }
}


int main() {
    int retval;
    char *username = NULL;

    signal(SIGINT, sigint_handler);
  /*
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
*/
    // Clear screen
    printf("\e[2J\e[H");
    for (;;) {
        workflow();
    }
    
   // pam_end(loginh, PAM_SUCCESS);
    return 0;
}
