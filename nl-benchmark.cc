/*
 nl reader benchmark

 Copyright (C) 2014 AMPL Optimization Inc

 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose and without fee is hereby granted,
 provided that the above copyright notice appear in all copies and that
 both that the copyright notice and this permission notice and warranty
 disclaimer appear in supporting documentation.

 The author and AMPL Optimization Inc disclaim all warranties with
 regard to this software, including all implied warranties of
 merchantability and fitness.  In no event shall the author be liable
 for any special, indirect or consequential damages or any damages
 whatsoever resulting from loss of use, data or profits, whether in an
 action of contract, negligence or other tortious action, arising out
 of or in connection with the use or performance of this software.

 Author: Victor Zverovich
 */

#include <cstring>

#include "mp/clock.h"
#include "mp/nl-reader.h"
#include "mp/problem.h"
#include "mp/problem-builder.h"

#include "asl.h"

#undef filename

// Computes time to run f.
template <typename Func>
void Time(Func func, char **filenames) {
  for (; *filenames; ++filenames) {
    const char *filename = *filenames;
    mp::steady_clock::time_point start = mp::steady_clock::now();
    func(filename);
    double time = mp::GetTimeAndReset(start);
    fmt::print("{}\t{}\n", filename, time);
  }
}

// Reads a file and returns the last character.
std::size_t ReadFile(const char *filename) {
  enum {BUFFER_SIZE = 4096};
  char buffer[BUFFER_SIZE];
  std::FILE *f = std::fopen(filename, "r");
  std::size_t size = 0;
  for (;;) {
    std::size_t num_read = std::fread(buffer, 1, BUFFER_SIZE, f);
    size += num_read;
    if (num_read < BUFFER_SIZE) {
      if (!std::feof(f))
        throw std::runtime_error("I/O error");
      break;
    }
  }
  std::fclose(f);
  return size;
}

// Reads an nl file using mp::ReadNLFile.
void ReadNLFileUsingMP(const char *filename) {
  mp::NullNLHandler<int> handler;
  mp::ReadNLFile(filename, handler);
}

// Reads an nl file and build an mp::Problem object using mp::ReadNLFile.
void ReadNLFileAndBuildProblemUsingMP(const char *filename) {
  mp::Problem problem;
  mp::ReadNLFile(filename, problem);
}

// Reads an nl file using ASL functions jac0dim and fg_read.
void ReadNLFileUsingASL(const char *filename) {
  ASL *asl = ASL_alloc(ASL_read_fg);
  FILE *nl = jac0dim(filename, 0);
  fg_read(nl, ASL_allow_missing_funcs);
  ASL_free(&asl);
}

int PrintUsage(char **argv) {
  fmt::print(stderr, "usage: {} [io|mp-read|mp-build|asl] FILE...\n", argv[0]);
  return 0;
}

int main(int argc, char **argv) {
  try {
    if (argc < 3)
      return PrintUsage(argv);
    const char *method = argv[1];
    char **filenames = argv + 2;
    if (std::strcmp(method, "io") == 0)
      Time(ReadFile, filenames);
    else if (std::strcmp(method, "mp-read") == 0)
      Time(ReadNLFileUsingMP, filenames);
    else if (std::strcmp(method, "mp-build") == 0)
      Time(ReadNLFileAndBuildProblemUsingMP, filenames);
    else if (std::strcmp(method, "asl") == 0)
      Time(ReadNLFileUsingASL, filenames);
    else PrintUsage(argv);
  } catch (const std::exception &e) {
    fmt::print(stderr, "Error: {}\n", e.what());
    return 1;
  }
}
