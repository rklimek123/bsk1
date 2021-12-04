#!/bin/bash

CPPCHECK="cppcheck --enable=all -I . --suppress=missingIncludeSystem"

$CPPCHECK officeractions.c
$CPPCHECK officerfiles.c
$CPPCHECK officerssh.c