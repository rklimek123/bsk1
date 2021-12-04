
PAM_DIR=/lib/x86_64-linux-gnu/security
CC=gcc
CCFLAGS=-Wconversion -fstack-protector


all: webserver officer

# Temporary client web server placeholder
webserver: loop.c
	$(CC) loop.c -o loop

# PAM SSH officer app
pamauth:
	cp officerapp /etc/pam.d/officerapp

pammodule: pam_currenttime.c
	$(CC) $(CCFLAGS) -fPIC -fno-stack-protector -c pam_currenttime.c
	ld -x --shared -o $(PAM_DIR)/pam_currenttime.so pam_currenttime.o

officer: officerssh.c officeractions.c officerfiles.c pamauth pammodule
	$(CC) $(CCFLAGS) -c -o officerfiles.a officerfiles.c
	$(CC) $(CCFLAGS) officeractions.c officerfiles.a -c -o officeractions.a
	$(CC) $(CCFLAGS) officerssh.c officerfiles.a officeractions.a -o officerssh -lpam -lpam_misc