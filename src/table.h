// Copyright 2026 Michael Yao @michael-s-yao
#pragma once
#include <stddef.h>

#define INIT_CAP 64

typedef struct {
  char** uuids;
  size_t count;
  size_t cap;
} UUIDTable;

void uuid_table_init(UUIDTable* t);

int uuid_table_find(const UUIDTable* t, const char* uuid);

int uuid_table_add(UUIDTable* t, const char* uuid);

void uuid_table_free(UUIDTable* t);
