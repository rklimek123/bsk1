#!/bin/bash

PAM_DIR=/lib/x86_64-linux-gnu/security
CC=gcc

# Temporary client web server placeholder
gcc loop.c -o loop

# PAM SSH officer app
cp officerapp /etc/pam.d/officerapp

$CC officerssh.c -o officerssh -lpam -lpam_misc
$CC -fPIC -fno-stack-protector -c pam_currenttime.c
ld -x --shared -o $PAM_DIR/pam_currenttime.so pam_currenttime.o
