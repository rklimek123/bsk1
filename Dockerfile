FROM debian:buster

### Install
RUN apt-get update
RUN apt install openssh-server -y
RUN apt install acl -y
RUN apt install gcc -y
RUN apt install vim -y
RUN apt install make -y
RUN apt install libpam0g-dev -y
RUN apt install sudo -y

### Install code checkers
RUN apt install apt-utils -y
RUN apt install cppcheck -y

### Copy user-data
COPY configure_users.sh /
COPY uzytkownicy.txt /

### Copy sources
# Officer SSH app
COPY officerapp /
COPY pam_currenttime.c /
COPY officerssh.c /
COPY officeractions.* /
COPY officerfiles.* /

# Officer sudo
COPY sudoers /etc/sudoers

# Client web app
COPY loop.c /

### Compile sources
COPY Makefile /
RUN make

### Code checking script
COPY code_check.sh /
RUN ./code_check.sh

### SSH Configs
COPY sshd_config /etc/ssh/sshd_config

### Lock used when adding credits/deposits,
### to atomically check for the next document number
### and add a document with that number.
COPY add_credit.lck /
COPY add_deposit.lck /

### Configure users, start ssh, start web server
# Done here, not in RUN, because of some problems,
# the nature of which I don't fully grasp.
CMD ./configure_users.sh uzytkownicy.txt & service ssh start & ./loop
