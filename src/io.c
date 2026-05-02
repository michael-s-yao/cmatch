// Copyright 2026 Michael Yao @michael-s-yao
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/io.h"

static char** read_lines(const char* path, size_t* out_count) {
  FILE* f = fopen(path, "r");
  if (!f) {
    perror(path);
    return NULL;
  }
  size_t cap = INIT_CAP, count = 0;
  char** lines = malloc(sizeof(char*) * cap);
  char buf[1 << 16];
  while (fgets(buf, sizeof(buf), f)) {
    buf[strcspn(buf, "\n")] = '\0';
    if (buf[0] == '\0')
      continue;
    if (count >= cap) {
      cap *= 2;
      lines = realloc(lines, sizeof(char*) * cap);
    }
    lines[count++] = strdup(buf);
  }
  fclose(f);
  *out_count = count;
  return lines;
}

static void free_lines(char** lines, size_t count) {
  for (size_t i = 0; i < count; i++)
    free(lines[i]);
  free(lines);
}

MatchInput* read_match(
  const char* applicants_file, const char* programs_file
) {
  size_t num_app_lines, num_prog_lines;
  char** app_lines = read_lines(applicants_file, &num_app_lines);
  char** prog_lines = read_lines(programs_file, &num_prog_lines);
  if (!app_lines || !prog_lines) {
    if (app_lines)
      free_lines(app_lines, num_app_lines);
    if (prog_lines)
      free_lines(prog_lines, num_prog_lines);
    return NULL;
  }

  MatchInput* mi = malloc(sizeof(MatchInput));

  uuid_table_init(&mi->app_table);
  for (size_t i = 0; i < num_app_lines; i++) {
    const char* line = app_lines[i];
    const char* sp = strchr(line, ' ');
    size_t ulen = sp ? (size_t)(sp - line) : strlen(line);
    char uuid[(1 << 8) + 1];
    size_t copy = ulen < (1 << 8) ? ulen : (1 << 8);
    memcpy(uuid, line, copy);
    uuid[copy] = '\0';
    uuid_table_add(&mi->app_table, uuid);
  }

  Program** programs = malloc(sizeof(Program*) * num_prog_lines);
  char** prog_names = malloc(sizeof(char*) * num_prog_lines);

  for (size_t i = 0; i < num_prog_lines; i++) {
    char* p = prog_lines[i];
    char* endp;

    uint64_t capacity = strtol(p, &endp, 10);
    p = endp;
    while (*p == ' ')
      p++;

    char name[1 << 12] = "";
    if (*p == '"') {
      p++;
      char* eq = strchr(p, '"');
      size_t nlen = eq ? (size_t)(eq - p) : strlen(p);
      size_t copy = nlen < sizeof(name) - 1 ? nlen : sizeof(name) - 1;
      memcpy(name, p, copy);
      name[copy] = '\0';
      p = eq ? eq + 1 : p + strlen(p);
    }
    prog_names[i] = strdup(name);
    while (*p == ' ')
      p++;

    size_t rcap = 16, rcount = 0;
    int64_t* ranks = malloc(sizeof(int64_t) * rcap);
    while (*p) {
      char* start = p;
      while (*p && *p != ' ')
        p++;
      size_t tlen = (size_t)(p - start);
      if (tlen == 0) {
        if (*p)
          p++;
        continue;
      }
      char token[(1 << 8) + 1];
      size_t copy = tlen < (1 << 8) ? tlen : (1 << 8);
      memcpy(token, start, copy);
      token[copy] = '\0';
      while (*p == ' ')
        p++;
      int idx = uuid_table_find(&(mi -> app_table), token);
      if (idx >= 0) {
        if (rcount == rcap) {
          rcap *= 2;
          ranks = realloc(ranks, sizeof(int64_t) * rcap);
        }
        ranks[rcount++] = (int64_t)idx;
      }
    }

    Vector rv = { .array = ranks, .size = rcount };
    programs[i] = make_program((int64_t) i, &rv, (size_t) capacity);
  }

  Applicant** applicants = malloc(sizeof(Applicant*) * num_app_lines);
  for (size_t i = 0; i < num_app_lines; i++) {
    char* p = app_lines[i];
    while (*p && *p != ' ')
      p++;
    while (*p == ' ')
      p++;

    size_t pcap = 16, pcount = 0;
    int64_t* prefs = malloc(sizeof(int64_t) * pcap);
    int64_t idx;
    while (*p) {
      char* endp;
      idx = strtol(p, &endp, 10);
      if (endp == p)
        break;
      if (idx >= 0 && (size_t) idx < num_prog_lines) {
        if (pcount == pcap) {
          pcap *= 2;
          prefs = realloc(prefs, sizeof(int64_t) * pcap);
        }
        prefs[pcount++] = (int64_t)idx;
      }
      p = endp;
      while (*p == ' ')
        p++;
    }

    Vector av = { .array = prefs, .size = pcount };
    applicants[i] = make_applicant((int64_t)i, &av);
  }

  mi -> match.applicants = applicants;
  mi -> match.num_applicants = num_app_lines;
  mi -> match.programs = programs;
  mi -> match.num_programs = num_prog_lines;
  mi -> prog_names = prog_names;

  free_lines(app_lines, num_app_lines);
  free_lines(prog_lines, num_prog_lines);
  return mi;
}

void free_match(MatchInput* mi) {
  for (size_t i = 0; i < mi -> match.num_applicants; i++)
    free_applicant((mi -> match.applicants)[i]);
  free(mi -> match.applicants);

  for (size_t i = 0; i < mi -> match.num_programs; i++) {
    free((mi -> prog_names)[i]);
    free_program((mi -> match.programs)[i]);
  }
  free(mi -> prog_names);
  free((mi -> match).programs);

  uuid_table_free(&(mi -> app_table));
  free(mi);
}
