#include "serial.hh"

int _read (int fd, char *buf, int count) {
    if (!rk_hasc()) {
        return 0;
    }
    *buf = rk_getc_raw();
    return 1;
}

int _write(int file, char *ptr, int len) {
  (void)file;

  for (int i = 0; i < len; i++) {
      rk_putc(ptr[i]);
  }
  return len;
}
