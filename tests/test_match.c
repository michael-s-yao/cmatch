// Copyright 2026 Michael Yao @michael-s-yao
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "src/match.h"

static void test_make_free_applicant(void) {
  int64_t* prefs = (int64_t*) malloc(sizeof(int64_t) * 3);
  prefs[0] = 10;
  prefs[1] = 20;
  prefs[2] = 30;
  Vector v = { .array = prefs, .size = 3 };
  Applicant* a = make_applicant(42, &v);

  assert(a -> uuid == 42);
  assert(a -> rank_list.size == 3);
  assert(a -> rank_list.array[0] == 10);
  assert(a -> rank_list.array[1] == 20);
  assert(a -> rank_list.array[2] == 30);
  assert(a -> curr_rank_list_idx == 0);
  assert(a -> curr_program == NO_PROGRAM);

  free_applicant(a);
  printf("PASS: test_make_free_applicant\n");
}

static void test_make_free_program(void) {
  int64_t* ranks = malloc(sizeof(int64_t) * 3);
  ranks[0] = 1;
  ranks[1] = 2;
  ranks[2] = 3;
  Vector v = { .array = ranks, .size = 3 };

  Program* p = make_program(7, &v, 2);

  assert(p -> uuid == 7);
  assert(p -> rank_list.size == 3);
  assert(p -> rank_list.array[0] == 1);
  assert(p -> matched_list.size == 2);
  assert(p -> matched_list.array[0] == NO_APPLICANT);
  assert(p -> matched_list.array[1] == NO_APPLICANT);

  free_program(p);
  printf("PASS: test_make_free_program\n");
}

static void test_basic_stable_match(void) {
  int64_t* a1_prefs = malloc(sizeof(int64_t) * 2);
  a1_prefs[0] = 1;
  a1_prefs[1] = 2;
  Vector a1v = { .array = a1_prefs, .size = 2 };
  Applicant* a1 = make_applicant(1, &a1v);

  int64_t* a2_prefs = malloc(sizeof(int64_t) * 2);
  a2_prefs[0] = 1;
  a2_prefs[1] = 2;
  Vector a2v = { .array = a2_prefs, .size = 2 };
  Applicant* a2 = make_applicant(2, &a2v);

  int64_t* p1_ranks = malloc(sizeof(int64_t) * 2);
  p1_ranks[0] = 1;
  p1_ranks[1] = 2;
  Vector p1v = { .array = p1_ranks, .size = 2 };
  Program* p1 = make_program(1, &p1v, 1);

  int64_t* p2_ranks = malloc(sizeof(int64_t) * 2);
  p2_ranks[0] = 2;
  p2_ranks[1] = 1;
  Vector p2v = { .array = p2_ranks, .size = 2 };
  Program* p2 = make_program(2, &p2v, 1);

  Applicant* applicants[] = { a1, a2 };
  Program* programs[] = { p1, p2 };
  Match match = {
    .applicants = applicants,
    .num_applicants = 2,
    .programs = programs,
    .num_programs   = 2
  };

  run_match(&match);

  assert(a1 -> curr_program == 1);
  assert(a2 -> curr_program == 2);
  assert((p1 -> matched_list).array[0] == 1);
  assert((p2 -> matched_list).array[0] == 2);

  free_applicant(a1);
  free_applicant(a2);
  free_program(p1);
  free_program(p2);
  printf("PASS: test_basic_stable_match\n");
}

static void test_displacement(void) {
  int64_t* a1_prefs = malloc(sizeof(int64_t) * 1);
  a1_prefs[0] = 1;
  Vector a1v = { .array = a1_prefs, .size = 1 };
  Applicant* a1 = make_applicant(1, &a1v);

  int64_t* a2_prefs = malloc(sizeof(int64_t) * 1);
  a2_prefs[0] = 1;
  Vector a2v = { .array = a2_prefs, .size = 1 };
  Applicant* a2 = make_applicant(2, &a2v);

  int64_t* p1_ranks = malloc(sizeof(int64_t) * 2);
  p1_ranks[0] = 2;
  p1_ranks[1] = 1;
  Vector p1v = { .array = p1_ranks, .size = 2 };
  Program* p1 = make_program(1, &p1v, 1);

  Applicant* applicants[] = { a1, a2 };
  Program* programs[] = { p1 };
  Match match = {
    .applicants = applicants,
    .num_applicants = 2,
    .programs = programs,
    .num_programs = 1
  };

  run_match(&match);

  assert(a2 -> curr_program == 1);
  assert(a1 -> curr_program == NO_PROGRAM);
  assert((p1 -> matched_list.array)[0] == 2);

  free_applicant(a1);
  free_applicant(a2);
  free_program(p1);
  printf("PASS: test_displacement\n");
}

static void test_capacity(void) {
  int64_t* a1_prefs = malloc(sizeof(int64_t) * 1);
  a1_prefs[0] = 1;
  Vector a1v = { .array = a1_prefs, .size = 1 };
  Applicant* a1 = make_applicant(1, &a1v);

  int64_t* a2_prefs = malloc(sizeof(int64_t) * 1);
  a2_prefs[0] = 1;
  Vector a2v = { .array = a2_prefs, .size = 1 };
  Applicant* a2 = make_applicant(2, &a2v);

  int64_t* a3_prefs = malloc(sizeof(int64_t) * 1);
  a3_prefs[0] = 1;
  Vector a3v = { .array = a3_prefs, .size = 1 };
  Applicant* a3 = make_applicant(3, &a3v);

  int64_t* p1_ranks = malloc(sizeof(int64_t) * 3);
  p1_ranks[0] = 1;
  p1_ranks[1] = 2;
  p1_ranks[2] = 3;
  Vector p1v = { .array = p1_ranks, .size = 3 };
  Program* p1 = make_program(1, &p1v, 2);

  Applicant* applicants[] = { a1, a2, a3 };
  Program* programs[] = { p1 };
  Match match = {
    .applicants = applicants,
    .num_applicants = 3,
    .programs = programs,
    .num_programs = 1
  };

  run_match(&match);

  assert(a1 -> curr_program == 1);
  assert(a2 -> curr_program == 1);
  assert(a3 -> curr_program == NO_PROGRAM);

  bool matched_a1 = false, matched_a2 = false;
  for (size_t i = 0; i < (p1 -> matched_list).size; i++) {
    matched_a1 = matched_a1 || (p1 -> matched_list.array[i] == 1);
    matched_a2 = matched_a2 || (p1 -> matched_list.array[i] == 2);
  }
  assert(matched_a1 && matched_a2);

  free_applicant(a1);
  free_applicant(a2);
  free_applicant(a3);
  free_program(p1);
  printf("PASS: test_capacity\n");
}

static void test_not_ranked(void) {
  int64_t* a1_prefs = malloc(sizeof(int64_t) * 2);
  a1_prefs[0] = 1;
  a1_prefs[1] = 2;
  Vector a1v = { .array = a1_prefs, .size = 2 };
  Applicant* a1 = make_applicant(1, &a1v);

  int64_t* p1_ranks = malloc(sizeof(int64_t) * 0);
  Vector p1v = { .array = p1_ranks, .size = 0 };
  Program* p1 = make_program(1, &p1v, 1);

  int64_t* p2_ranks = malloc(sizeof(int64_t) * 1);
  p2_ranks[0] = 1;
  Vector p2v = { .array = p2_ranks, .size = 1 };
  Program* p2 = make_program(2, &p2v, 1);

  Applicant* applicants[] = { a1 };
  Program* programs[] = { p1, p2 };
  Match match = {
    .applicants = applicants,
    .num_applicants = 1,
    .programs = programs,
    .num_programs = 2
  };

  run_match(&match);

  assert(a1 -> curr_program == 2);
  assert((p1 -> matched_list).array[0] == NO_APPLICANT);
  assert((p2 -> matched_list).array[0] == 1);

  free_applicant(a1);
  free_program(p1);
  free_program(p2);
  printf("PASS: test_not_ranked\n");
}

int main(void) {
  test_make_free_applicant();
  test_make_free_program();
  test_basic_stable_match();
  test_displacement();
  test_capacity();
  test_not_ranked();
  printf("All tests passed.\n");
  return EXIT_SUCCESS;
}
