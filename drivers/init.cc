#include <cstring>

extern "C" void clear_bss()
{
	// clear BSS
	extern unsigned char _bss_start;
	extern unsigned char _bss_end;
	memset(&_bss_start, 0, &_bss_end - &_bss_start);
}

extern "C" void init_statics()
{
	// call constructors of static objects
	extern void (*_init_array_start)(void);
	extern void (*_init_array_end)(void);
	for (void (**pFunc)(void) = &_init_array_start; pFunc < &_init_array_end; pFunc++) {
		(**pFunc)();
	}
}
