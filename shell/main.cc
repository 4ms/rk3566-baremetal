#include "drivers/console.hh"

extern "C" int cruntimemain() {
	Console::init();

	while (1) {
		Console::process();
	}
}
