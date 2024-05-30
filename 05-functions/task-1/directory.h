#pragma once

#define DIRECTORY_LENGTH 100

struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

extern struct abonent handbook[DIRECTORY_LENGTH];

int add_subscr(char name[10], char second_name[10], char tel[10]);
int delete_subscr(char name[10]);
int find_subscr(char name[10]);
int print_all();
