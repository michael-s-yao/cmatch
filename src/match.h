// Copyright 2026 Michael Yao @michael-s-yao
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NO_APPLICANT ((int64_t) -1)
#define NO_PROGRAM ((int64_t) -1)

typedef struct {
  int64_t* array;
  size_t size;
} Vector;

typedef struct {
  int64_t uuid;
  Vector  rank_list;
  size_t  curr_rank_list_idx;
  int64_t curr_program;
} Applicant;

typedef struct {
  int64_t uuid;
  Vector rank_list;
  Vector matched_list;
} Program;

typedef struct {
  Applicant** applicants;
  size_t num_applicants;
  Program** programs;
  size_t num_programs;
} Match;

Applicant* make_applicant(int64_t uuid, Vector* rank_list);

void free_applicant(Applicant* applicant);

Program* make_program(int64_t uuid, Vector* rank_list, size_t capacity);

void free_program(Program* program);

/**
 * Run the applicant-proposing deferred acceptance (Roth-Peranson) algorithm.
 * On return, each applicant's curr_program holds the matched program UUID
 * (or NO_PROGRAM), and each Program's matched_list holds the accepted
 * applicant UUIDs (NO_APPLICANT for unfilled slots).
 *
 * @match: the set of applicants and programs participating in the match.
 *
 * @return: none.
 */
void run_match(Match* match);
