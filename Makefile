
PAM_DIR=/lib/x86_64-linux-gnu/security
CC=gcc


all: webserver officer

# Temporary client web server placeholder
webserver: loop.c
	$(CC) loop.c -o loop

# PAM SSH officer app
pamauth:
	cp officerapp /etc/pam.d/officerapp

pammodule: pam_currenttime.c
	$(CC) -fPIC -fno-stack-protector -c pam_currenttime.c
	ld -x --shared -o $(PAM_DIR)/pam_currenttime.so pam_currenttime.o

officer: officerssh.c officeractions.c officerfiles.c pamauth pammodule
	$(CC) -c -o officerfiles.a officerfiles.c
	$(CC) officeractions.c officerfiles.a -c -o officeractions.a
	$(CC) officerssh.c officerfiles.a officeractions.a -o officerssh -lpam -lpam_misc