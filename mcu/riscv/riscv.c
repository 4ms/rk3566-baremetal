// this code is derived from https://github.com/liamhays/rk3566-mcu/blob/main/examples/hello_mailbox/hello_mailbox.c

#include <stdint.h>
#include <stdbool.h>

// TOGGLE GPIO PORT0 PORT C PIN 5
#define GPIO_0 (0xfdd60000)
#define GPIO_DIR_H_OFFSET (0x0C)
#define GPIO_DATA_HIGH_OFFSET (0x04)
#define GPIO_PIN (5)

static uint32_t gpio_mask(bool level) {
    return (level << GPIO_PIN) | (1 << (GPIO_PIN + 16));
}
static void gpio_put(bool level) {
    volatile uint32_t *gpio_data_high = (uint32_t*)(GPIO_0 + GPIO_DATA_HIGH_OFFSET);
    const uint32_t mask = gpio_mask(level);
    *gpio_data_high = mask;
}

static void gpio_enable() {
    volatile uint32_t *gpio_data_high = (uint32_t*)(GPIO_0 + GPIO_DIR_H_OFFSET);
    const uint32_t mask = gpio_mask(true);
    *gpio_data_high = mask;
}

// we can read these variables from the arm cores if we check their address in the elf file
void delay(uint32_t t) {
    volatile uint32_t d = t;
    while (d--);
}

int main() {
    delay(65536);

    gpio_enable();

  while (1) {
            gpio_put(1);
            gpio_put(0);
        }

  return 0;
}
