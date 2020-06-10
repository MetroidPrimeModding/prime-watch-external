#include <cstdlib>
#include <iostream>
#include <cstdio>
#include "common.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

uint8_t* emuRAMAddressStart = 0;

void findAndAttachProcess() {
  int pid = dolphinPid();
  if (pid < 0) {
    cout << "Can't find dolphin! Launch dolphin first!" << endl;
    exit(1);
  }

  cout << "Dolphin found, PID " << pid << endl;
  const string file_name = "/dolphin-emu." + to_string(pid);
  int fd = shm_open(file_name.c_str(), O_RDWR, 0600);

  if (fd < 0) {
    cout << "Failed to open shared memory" << endl;
    exit(2);
  }

  cout << "Opened shmem" << endl;

  size_t size = 0x2040000;
  void *mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) {
    cout << "failed to map shared memory" << endl;
    close(fd);
    exit(3);
  }

  emuRAMAddressStart = reinterpret_cast<uint8_t *>(mem);

  close(fd);

}

uint32_t getRealPtr(uint32_t address) {
  uint32_t masked = address & 0x7FFFFFFFu;
  if (masked > 0x1800000) {
    return 0;
  }
  return masked;
}

void dolphin_memcpy(void *dest, size_t offset, size_t size) {
  if (size > 0x1800000) {
    size = 0x1800000;
  }
  memcpy(dest, emuRAMAddressStart + getRealPtr(offset), size);
}