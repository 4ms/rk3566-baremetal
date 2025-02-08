#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

char rk_getc();
bool rk_hasc();
char rk_getc_raw();
void rk_putc(char);

#ifdef __cplusplus
}
#endif
