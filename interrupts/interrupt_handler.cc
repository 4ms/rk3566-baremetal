#include "drivers/interrupt.hh"
#include <cstdio>

extern "C" void __attribute__((used)) ISRHandler(unsigned irqnum) {

	printf("IRQ %u\n", irqnum);
	mdrivlib::InterruptManager::callISR(irqnum);
}
