FROM debian:buster

### Install
RUN apt-get update
RUN apt install openssh-server -y
RUN apt install acl -y
RUN apt install gcc -y
RUN apt install vim -y
RUN apt install libpam0g-dev -y

### Copy user-data
COPY configure_users.sh /
COPY uzytkownicy.txt /

### Copy sources
# Officer SSH app
COPY officerapp /
COPY pam_currenttime.c /
COPY officerssh.c /

# Client web app
COPY loop.c /

### Compile sources
COPY compile.sh /
RUN ./compile.sh

### SSH Configs
COPY sshd_config /etc/ssh/sshd_config

### Configure users, start ssh, start web server
# Done here, not in RUN, because of some problems,
# the nature of which I don't fully grasp.
CMD ./configure_users.sh uzytkownicy.txt & service ssh start & ./loop
