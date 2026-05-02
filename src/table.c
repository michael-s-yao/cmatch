// Copyright 2026 Michael Yao @michael-s-yao
#include <stdlib.h>
#include <string.h>
#include "src/table.h"

void uuid_table_init(UUIDTable* t) {
  t -> uuids = malloc(sizeof(char*) * INIT_CAP);
  t -> count = 0;
  t -> cap = INIT_CAP;
}

int uuid_table_find(const UUIDTable* t, const char* uuid) {
  for (size_t i = 0; i < t -> count; i++)
    if (strcmp((t -> uuids)[i], uuid) == 0)
      return (int) i;
  return -1;
}

int uuid_table_add(UUIDTable* t, const char* uuid) {
  int idx = uuid_table_find(t, uuid);
  if (idx >= 0)
    return idx;
  if (t -> count == t -> cap) {
    t -> cap *= 2;
    t -> uuids = realloc(t -> uuids, sizeof(char*) * (t -> cap));
  }
  (t -> uuids)[t -> count] = strdup(uuid);
  return (int) ((t -> count)++);
}

void uuid_table_free(UUIDTable* t) {
  for (size_t i = 0; i < t -> count; i++)
    free((t -> uuids)[i]);
  free(t -> uuids);
}
