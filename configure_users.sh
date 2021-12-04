#!/bin/bash

USERS=$1

groupadd officer
groupadd client

mkdir deposits
mkdir credits

# Basic access rules
setfacl -d -m user::rw- deposits
setfacl -d -m user::rw- credits
setfacl -d -m group::--- deposits
setfacl -d -m group::--- credits
setfacl -d -m other::--- deposits
setfacl -d -m other::--- credits

# Clients
setfacl -m group:client:r-x deposits
setfacl -m group:client:r-x credits

# Making sure clients cannot access any files (except own)
setfacl -d -m group:client:--- deposits
setfacl -d -m group:client:--- credits

# Officers
setfacl -m group:officer:rwx deposits
setfacl -m group:officer:rwx credits

setfacl -d -m group:officer:rw- deposits
setfacl -d -m group:officer:rw- credits

# Locks
setfacl -m group:officer:rw- add_credit.lck
setfacl -m group:officer:rw- add_deposit.lck


while read line; do
    user_id=$(echo $line | awk '{print $1}');
    user_group=$(echo $line | awk '{print $2}');
    name=$(echo $line | awk '{print $3}');
    surname=$(echo $line | awk '{print $4}');
    useradd $user_id -g $user_group -c "$name $surname"

    # Remove for production!
    # chpasswd <<< $user_id:123

done < $USERS
