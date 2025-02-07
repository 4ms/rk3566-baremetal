#include <stdint.h>

auto UART_THR = reinterpret_cast<volatile uint32_t *>(0xfe660000);

int main() {
  *UART_THR = '\n';
  *UART_THR = 'H';
  *UART_THR = 'e';
  *UART_THR = 'l';
  *UART_THR = 'l';
  *UART_THR = 'o';
  *UART_THR = '\n';

  while (1)
    ;
}
