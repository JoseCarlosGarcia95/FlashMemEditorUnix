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
#include "flash-memory-util.h"

int flash_memory_util_search_pid_by_name(char *pname) {
  char cmd[100];
  char path[10];

  sprintf(cmd, "pgrep %s --full", pname);

  FILE *fp = popen(cmd, "r");
  if (fp == NULL) {
    printf("Failed to run command\n");
    exit(1);
  }

  fgets(path, sizeof(path) - 1, fp);

  pclose(fp);

  if (strcmp(path, "") == 0)
    return -1;
  return atoi(path);
}

void flash_memory_read_maps(int pid, long **addresses, int *mapSize) {
  char filepath[50], line[256];
  unsigned long baddr, eaddr;

  FILE *f;

  *mapSize = 0;

  sprintf(filepath, "/proc/%d/maps", pid);

  f = fopen(filepath, "r");

  while (fgets(line, 256, f) != NULL) {

    int lsize = strlen(line);
    if (line[lsize - 2] == 'r') {
      break;
    }

    (*mapSize)++;

    sscanf(line, "%lX-%lX", &baddr, &eaddr);

    *addresses = (long *)realloc(*addresses, (*mapSize) * sizeof(long) * 2);

    (*addresses)[((*mapSize) - 1) * 2] = baddr;
    (*addresses)[((*mapSize) - 1) * 2 + 1] = eaddr;
  }
  fclose(f);
}

void flash_memory_search_and_replace(int pid, long base_addr, long memory_size,
                                     char *search, char *payload) {
  long i;
  int j, match, mem_fd, res;
  char mem_file_name[50];
  char *buf = malloc(memory_size + 1);
  sprintf(mem_file_name, "/proc/%d/mem", pid);
  mem_fd = open(mem_file_name, O_RDONLY);
  ptrace(PTRACE_ATTACH, pid, NULL, NULL);
  waitpid(pid, NULL, 0);
  lseek(mem_fd, base_addr, SEEK_SET);
  read(mem_fd, buf, memory_size);

  close(mem_fd);

  int ssize = strlen(search);
  for (i = 0; i < memory_size; i++) {
    match = 1;

    for (j = 0; j < ssize && match; j++) {
      if (buf[i + j] != search[j])
        match = 0;
    }
    for (j = 0; j < ssize && match; j++)
      res = ptrace(PTRACE_POKEDATA, pid, base_addr + i + j,
                   *((long *)(payload + j)));
  }

  ptrace(PTRACE_DETACH, pid, NULL, NULL);

  free(buf);
}

void flash_memory_payload_int(int search, int payload) {
  char search_str[4], payload_str[4];

  search_str[0] = (search >> 24) & 0xFF;
  search_str[1] = (search >> 16) & 0xFF;
  search_str[2] = (search >> 8) & 0xFF;
  search_str[3] = search & 0xFF;

  payload_str[0] = (payload >> 24) & 0xFF;
  payload_str[1] = (payload >> 16) & 0xFF;
  payload_str[2] = (payload >> 8) & 0xFF;
  payload_str[3] = payload & 0xFF;

  flash_memory_payload_string(search_str, payload_str);
}

void flash_memory_wait_until_flash() {
  while (flash_memory_util_search_pid_by_name("plugin-container") == -1)
    ;
}
void flash_memory_payload_string(char *search, char *payload) {

  if (strlen(search) != strlen(payload)) {
    puts("Search and payload needs the same size!");
    return;
  }
  int pid, mapSize, i;
  long *addresses, base_addr, mem_size;

  // Get pid.
  pid = flash_memory_util_search_pid_by_name("plugin-container");
  printf("%d\n", pid);
  mapSize = 0;

  addresses = (long *)malloc(0);

  // Get base address and end address.
  flash_memory_read_maps(pid, &addresses, &mapSize);

  for (i = 0; i < mapSize; i++) {
    base_addr = addresses[2 * i];
    mem_size = addresses[2 * i + 1] - base_addr;

    printf("%lX - %lX\n", base_addr, mem_size);
    flash_memory_search_and_replace(pid, base_addr, mem_size, search, payload);
  }

  free(addresses);
}