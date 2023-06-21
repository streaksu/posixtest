/*
    mmap.c: mmap-related tests.
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

#include <sys/mman.h>
#include <entrypoints.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

int test_mmap_anon(void) {
    size_t pagesize = getpagesize();
    for (int i = 0; i < 100; i++) {
        void *test_ptr  = mmap(NULL, pagesize * 10, PROT_WRITE | PROT_READ,
                               MAP_ANON | MAP_PRIVATE, 0, 0);
        if (test_ptr == MAP_FAILED) {
            perror("Could not MAP_ANON");
            return 0;
        }

        if (munmap(test_ptr + (pagesize * 5), pagesize)) {
            perror("Could not partially munmap");
            return 0;
        }

        // Writing to the beginning and end should still work.
        *((char *)test_ptr)                    = 'a';
        *(((char *)test_ptr) + (pagesize * 9)) = 'a';

        if (munmap(test_ptr, pagesize * 10)) {
            perror("Could not fully munmap");
            return 0;
        }
    }

    return 1;
}
