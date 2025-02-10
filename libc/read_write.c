#include "serial.hh"

int _read (int fd, char *buf, int count) {
    *buf = rk_getc();
    return 1;
}


int _write(int file, char *ptr, int len) {
  (void)file;

  for (int i = 0; i < len; i++) {
      rk_putc(ptr[i]);
  }
  return len;
}
