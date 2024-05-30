#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 32
#define TELEPHONE_LENGTH 16

struct abonent {
    char name[NAME_LENGTH];
    char second_name[NAME_LENGTH];
    char tel[TELEPHONE_LENGTH];
};

struct directory {
    struct abonent *entries;
    int size;
    int capacity;
};

void initialize_directory(struct directory *dir);
void resize_directory(struct directory *dir);
void add_subscr(struct directory *dir, const char *name,
                const char *second_name, const char *tel);
void delete_subscr(struct directory *dir, const char *name);
void find_subscr(const struct directory *dir, const char *name);
void print_all(const struct directory *dir);
void free_directory(struct directory *dir);