// Copyright 2026 Michael Yao @michael-s-yao
#pragma once
#include "src/match.h"
#include "src/table.h"

typedef struct {
  Match match;
  UUIDTable app_table;
  char** prog_names;
} MatchInput;

MatchInput* read_match(const char* applicants_file, const char* programs_file);

void free_match(MatchInput* input);
