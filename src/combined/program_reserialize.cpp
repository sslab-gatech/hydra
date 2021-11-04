// SPDX-License-Identifier: MIT

/**
  program_reserialize.cpp
  Purpose: generate C code for a serialized testcase
  Usage:
  ./program_reserialize [serialized testcase path] [new serialized test path] <string>
  */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "Program.hpp"

int main(int argc, char *argv[]) {

	if (argc != 4)
		return 1;

	char *spath = argv[1];
	char *dpath = argv[2];
	const char *keep_list = argv[3];

	Program *program = Program::selective_reserialize(spath, dpath, keep_list);
	if (!program)
		return 1;

	program->show();
	delete program;

	return 0;
}
