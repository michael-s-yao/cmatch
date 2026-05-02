// Copyright 2026 Michael Yao @michael-s-yao
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "src/match.h"

static bool place(Program* prog, Applicant* a, Applicant** d, Match* match);

static int64_t get_next_preference(Applicant* a);

static Applicant* get_applicant_by_uuid(int64_t uuid, const Match* match);

static Program* get_program_by_uuid(int64_t uuid, const Match* match);

static int64_t rank_of(int64_t applicant_uuid, const Vector* rank_list);

/**
 * run_match - Run the applicant-proposing deferred acceptance algorithm.
 *
 * @match: the set of applicants and programs participating in the match.
 *
 * @return: none.
 */
void run_match(Match* match) {
  size_t max_entries = match -> num_applicants;
  for (size_t i = 0; i < (match -> num_applicants); i++)
    max_entries += ((match -> applicants)[i] -> rank_list).size;

  int64_t* queue = (int64_t*) malloc(sizeof(int64_t) * max_entries);
  size_t head = 0, tail = 0;

  for (size_t i = 0; i < match -> num_applicants; i++)
    queue[tail++] = ((match -> applicants)[i]) -> uuid;

  Applicant* a;
  Applicant* displaced = NULL;
  Program* p;
  int64_t program_uuid, applicant_uuid;
  while (head < tail) {
    applicant_uuid = queue[head++];

    a = get_applicant_by_uuid(applicant_uuid, match);
    if (!a || a -> curr_program != NO_PROGRAM)
      continue;

    if ((program_uuid = get_next_preference(a)) == NO_PROGRAM)
      continue;

    if (!(p = get_program_by_uuid(program_uuid, match))) {
      queue[tail++] = applicant_uuid;
      continue;
    }

    if (place(p, a, &displaced, match)) {
      a -> curr_program = program_uuid;
      if (displaced != NULL)
        queue[tail++] = displaced -> uuid;
    } else {
      queue[tail++] = applicant_uuid;
    }
  }

  free(queue);
}

/**
 * place - Attempt to place an applicant in a program.
 *
 * @prog: the program to attempt to place an applicant into.
 * @a: the applicant to attempt to place into a program.
 * @d: a pointer to a displaced applicant after placement.
 * @match: the participating applicants and programs in the match.
 *
 * @return: true on successful placement, and false if applicant is either
 *     (1) not on the program's rank list at all, or (2) current matches are
 *     all preferred over the applicant.
 */
static bool place(Program* prog, Applicant* a, Applicant** d, Match* match) {
  int64_t applicant_rank = rank_of(a -> uuid, &(prog -> rank_list));
  if (applicant_rank == INT64_MAX)
    return false;

  *d = NULL;

  int64_t worst_rank = -1, worst_uuid = NO_APPLICANT;
  size_t worst_slot = 0;
  int64_t curr;
  for (size_t i = 0; i < prog->matched_list.size; i++) {
    if ((curr = (prog -> matched_list.array)[i]) == NO_APPLICANT) {
      (prog -> matched_list.array)[i] = a -> uuid;
      return true;
    }
    int64_t r = rank_of(curr, &(prog -> rank_list));
    if (r > worst_rank)
      worst_rank = r, worst_uuid = curr, worst_slot = i;
  }

  if (applicant_rank < worst_rank) {
    if ((*d = get_applicant_by_uuid(worst_uuid, match)))
      (*d) -> curr_program = NO_PROGRAM;
    (prog -> matched_list.array)[worst_slot] = a -> uuid;
    return true;
  }
  return false;
}

static int64_t get_next_preference(Applicant* a) {
  if (a -> curr_rank_list_idx >= a -> rank_list.size)
    return NO_PROGRAM;
  return (a -> rank_list.array)[(a -> curr_rank_list_idx)++];
}

static Applicant* get_applicant_by_uuid(int64_t uuid, const Match* match) {
  for (size_t i = 0; i < (match -> num_applicants); i++)
    if ((match -> applicants)[i] -> uuid == uuid)
      return (match -> applicants)[i];
  return NULL;
}

static Program* get_program_by_uuid(int64_t uuid, const Match* match) {
  for (size_t i = 0; i < match -> num_programs; i++)
    if ((match -> programs)[i] -> uuid == uuid)
      return (match -> programs)[i];
  return NULL;
}

static int64_t rank_of(int64_t applicant_uuid, const Vector* rank_list) {
  for (size_t i = 0; i < rank_list -> size; i++)
    if ((rank_list -> array)[i] == applicant_uuid)
      return (int64_t) i;
  return INT64_MAX;
}

Applicant* make_applicant(int64_t uuid, Vector* rank_list) {
  Applicant* a = malloc(sizeof(Applicant));
  a -> uuid = uuid;
  a -> rank_list = *rank_list;
  a -> curr_rank_list_idx = 0;
  a -> curr_program = NO_PROGRAM;
  return a;
}

void free_applicant(Applicant* a) {
  free(a -> rank_list.array);
  free(a);
}

Program* make_program(int64_t uuid, Vector* rank_list, size_t capacity) {
  Program* p = malloc(sizeof(Program));
  p -> uuid = uuid;
  p -> rank_list = *rank_list;
  p -> matched_list.array = malloc(sizeof(int64_t) * capacity);
  p -> matched_list.size = capacity;
  memset(p -> matched_list.array, NO_APPLICANT, sizeof(int64_t) * capacity);
  return p;
}

void free_program(Program* p) {
  free(p -> rank_list.array);
  free(p -> matched_list.array);
  free(p);
}
