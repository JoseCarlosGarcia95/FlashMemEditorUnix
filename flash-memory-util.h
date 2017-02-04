/*
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef FLASH_MEMORY_UTIL
#define FLASH_MEMORY_UTIL

/**
    Search a string and then replace it.
*/
void flash_memory_payload_string(char *, char *);

/**
    Search a integer value and then replace it.
*/
void flash_memory_payload_int(int, int);

/**
    Wait until you open a flash executable.
*/
void flash_memory_wait_until_flash();

#endif