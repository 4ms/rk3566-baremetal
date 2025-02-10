#include "serial.hh"
#include "uart.hh"
#include <cstdio>
#include <string.h>

extern "C" {
#include "anchor/console/console.h"
}

CONSOLE_COMMAND_DEF(echo, "echo", CONSOLE_STR_ARG_DEF(str, "What to echo"));
static void echo_command_handler(const echo_args_t *args) { printf("%s\n", args->str); }

CONSOLE_COMMAND_DEF(rr, "Read register", CONSOLE_INT_ARG_DEF(reg, "Reg to read"));
static void rr_command_handler(const rr_args_t *args)
{
	auto val = *reinterpret_cast<volatile uint32_t *>(args->reg);
	printf("0x%08x\n", val);
}

CONSOLE_COMMAND_DEF(wr,
					"Write register",
					CONSOLE_INT_ARG_DEF(reg, "Reg to write"),
					CONSOLE_INT_ARG_DEF(val, "Value to write"));
static void wr_command_handler(const wr_args_t *args)
{

	*reinterpret_cast<volatile uint32_t *>(args->reg) = args->val;
	rr_args_t a;
	a.reg = args->reg;
	rr_command_handler(&a);
}

CONSOLE_COMMAND_DEF(wrm,
					"Write register masked",
					CONSOLE_INT_ARG_DEF(reg, "Reg to write"),
					CONSOLE_INT_ARG_DEF(val, "Value to write"),
					CONSOLE_INT_ARG_DEF(mask, "Write mask"));
static void wrm_command_handler(const wrm_args_t *args)
{
	wr_args_t a;
	a.val = args->val & args->mask;
	a.reg = args->reg;
	wr_command_handler(&a);
}

extern "C" int cruntimemain()
{
	auto print_ = [](const char *str) {
		fwrite(str, 1, strlen(str), stdout);
		fflush(stdout);
	};

	const console_init_t init_console = {
		.write_function = print_,
	};

	printf("%x", 1024);

	console_init(&init_console);
	console_command_register(echo);
	console_command_register(rr);
	console_command_register(wr);
	console_command_register(wrm);

	while (1) {
		auto c = rk_getc();
		console_process(reinterpret_cast<uint8_t *>(&c), 1);
	}
}
