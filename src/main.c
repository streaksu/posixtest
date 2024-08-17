/*
    main.c: Entrypoint of the project
    Copyright (C) 2023 streaksu

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <entrypoints.h>

const char *pass_str = "\e[1;32mPASSED\e[0m";
const char *fail_str = "\e[1;31mFAILED\e[0m";

int main(void) {
    printf("MAP_ANON:                   %s\n", test_mmap_anon()     ? pass_str : fail_str);
    printf("SOCK_STREAM simple message: %s\n", test_stream_simple() ? pass_str : fail_str);
    printf("SOCK_DGRAM simple message:  %s\n", test_dgram_simple()  ? pass_str : fail_str);
    printf("SOCK_DGRAM passing an fd:   %s\n", test_dgram_fd()      ? pass_str : fail_str);
}
