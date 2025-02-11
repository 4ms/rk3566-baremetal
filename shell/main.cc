#include "console.hh"
#include "uart.hh"
#include <cstdio>

extern "C" int cruntimemain() {
	Console::init();

	while (1) {
		Console::process();
	}
}
