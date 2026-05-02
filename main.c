// Copyright 2026 Michael Yao @michael-s-yao
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/io.h"
#include "src/match.h"

int main(int argc, char* argv[]) {
  const char* applicants_file = NULL;
  const char* programs_file = NULL;
  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], "-a") == 0)
      applicants_file = argv[i + 1];
    else if (strcmp(argv[i], "-p") == 0)
      programs_file = argv[i + 1];
  }
  if (!applicants_file || !programs_file) {
    fprintf(stderr, "Usage: %s -a [APPLICANTS] -p [PROGRAMS]\n", argv[0]);
    return EXIT_FAILURE;
  }

  MatchInput* mi = read_match(applicants_file, programs_file);
  if (!mi)
    return EXIT_FAILURE;

  run_match(&(mi -> match));

  int64_t prog_id;
  for (size_t i = 0; i < (mi -> match).num_applicants; i++) {
    prog_id = (mi -> match).applicants[i] -> curr_program;
    if (prog_id == NO_PROGRAM)
      printf("%s: unmatched\n", (mi -> app_table).uuids[i]);
    else
      printf(
        "%s: %s\n", (mi -> app_table).uuids[i], (mi -> prog_names)[prog_id]);
  }

  free_match(mi);
  return EXIT_SUCCESS;
}
