FROM debian:buster

RUN apt-get update
RUN apt install openssh-server -y
RUN apt install acl
RUN apt install gcc -y

COPY configure_users.sh /
COPY uzytkownicy.txt /

COPY hello_world.c /
COPY loop.c /
COPY compile.sh /

RUN ./compile.sh

CMD ./configure_users.sh uzytkownicy.txt & service ssh start & ./loop
