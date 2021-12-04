#!/bin/bash

CPPCHECK="cppcheck --enable=all -I . --suppress=missingIncludeSystem"

$CPPCHECK officeractions.c
$CPPCHECK officerfiles.c
$CPPCHECK officerssh.c

#-Wconversion i -fstack-protector użyty w kompilacji

# Uruchomienie z valgrindem i korzystanie z aplikacji nie powoduje wypisania ostrzeżeń
