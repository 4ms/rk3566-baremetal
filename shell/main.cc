#include "serial.hh"
#include "uart.hh"
#include <cstddef>
#include <cstdio>

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(echo, "echo", CONSOLE_STR_ARG_DEF(str, "What to echo"));
static void echo_command_handler(const echo_args_t *args)
{
	printf(args->str);
	printf("\n");
}

extern "C" int cruntimemain()
{
	auto print_ = [](const char *str) { printf("%s", str); };

	const console_init_t init_console = {
		.write_function = print_,
	};

	console_init(&init_console);
	console_command_register(echo);

	while (1) {
		auto c = rk_getc();
		console_process(reinterpret_cast<uint8_t *>(&c), 1);
	}
}
