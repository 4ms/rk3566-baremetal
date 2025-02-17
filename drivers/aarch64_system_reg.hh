#pragma once
#include <cstdint>

// SPSR
inline uint64_t read_spsr_el1() {
	uint64_t spsr_el1;
	asm volatile("mrs %0, SPSR_EL1\n\t" : "=r"(spsr_el1) : : "memory");
	return spsr_el1;
}

inline void write_spsr_el1(uint64_t spsr_el1) {
	asm volatile("msr SPSR_EL1, %0\n\t" : : "r"(spsr_el1) : "memory");
}

inline uint64_t read_spsr_el2() {
	uint64_t spsr_el2;
	asm volatile("mrs %0, SPSR_EL2\n\t" : "=r"(spsr_el2) : : "memory");
	return spsr_el2;
}

inline void write_spsr_el2(uint64_t spsr_el2) {
	asm volatile("msr SPSR_EL2, %0\n\t" : : "r"(spsr_el2) : "memory");
}

//
