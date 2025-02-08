#include "circle/armv8mmu.h"
#include "circle/synchronize64.h"
#include "memory_layout.h"
#include <cstdint>
#include <cstring>

// index into MAIR_EL1 register
#define ATTRINDX_NORMAL 0
#define ATTRINDX_DEVICE 1
#define ATTRINDX_COHERENT 2

// We create one level 2 (first lookup level) translation table with 3 table
// entries (total 1.5GB) which point to a level 3 (final lookup level) translation
// table each with 8192 page entries a 64KB (total 512MB).
#define PAGE_SIZE 0x10000
#define LEVEL2_TABLE_ENTRIES 3

static char TTBL2[PAGE_SIZE];
TARMV8MMU_LEVEL2_DESCRIPTOR *TTBL2_desc = reinterpret_cast<TARMV8MMU_LEVEL2_DESCRIPTOR *>(&TTBL2);

static char TTBL3[LEVEL2_TABLE_ENTRIES][PAGE_SIZE];

TARMV8MMU_LEVEL3_DESCRIPTOR *create_level3_table(unsigned idx, uintptr base_addr);

void setup_ttb()
{
	memset(&TTBL2, 0, PAGE_SIZE);

	for (unsigned nEntry = 0; nEntry < LEVEL2_TABLE_ENTRIES; nEntry++) // entries a 512MB
	{
		u64 nBaseAddress = (u64)nEntry * ARMV8MMU_TABLE_ENTRIES * ARMV8MMU_LEVEL3_PAGE_SIZE;

		TARMV8MMU_LEVEL3_DESCRIPTOR *pTable = create_level3_table(nEntry, nBaseAddress);
		// assert(pTable != 0);

		TARMV8MMU_LEVEL2_TABLE_DESCRIPTOR *pDesc = &TTBL2_desc[nEntry].Table;

		pDesc->Value11 = 3;
		pDesc->Ignored1 = 0;
		pDesc->TableAddress = ARMV8MMUL2TABLEADDR((u64)pTable);
		pDesc->Reserved0 = 0;
		pDesc->Ignored2 = 0;
		pDesc->PXNTable = 0;
		pDesc->UXNTable = 0;
		pDesc->APTable = AP_TABLE_ALL_ACCESS;
		pDesc->NSTable = 0;
	}

	DataSyncBarrier();
}

TARMV8MMU_LEVEL3_DESCRIPTOR *create_level3_table(unsigned idx, uintptr base_addr)
{

	TARMV8MMU_LEVEL3_DESCRIPTOR *pTable = reinterpret_cast<TARMV8MMU_LEVEL3_DESCRIPTOR *>(&TTBL3[idx]);

	for (unsigned nPage = 0; nPage < ARMV8MMU_TABLE_ENTRIES; nPage++) // 8192 entries a 64KB
	{
		TARMV8MMU_LEVEL3_PAGE_DESCRIPTOR *pDesc = &pTable[nPage].Page;

		pDesc->Value11 = 3;
		pDesc->AttrIndx = ATTRINDX_NORMAL;
		pDesc->NS = 0;
		pDesc->AP = ATTRIB_AP_RW_EL1;
		pDesc->SH = ATTRIB_SH_INNER_SHAREABLE;
		pDesc->AF = 1;
		pDesc->nG = 0;
		pDesc->Reserved0_1 = 0;
		pDesc->OutputAddress = ARMV8MMUL3PAGEADDR(base_addr);
		pDesc->Reserved0_2 = 0;
		pDesc->Continous = 0;
		pDesc->PXN = 0;
		pDesc->UXN = 1;
		pDesc->Ignored = 0;

		if (base_addr >= MEMORY_SIZE) {
			pDesc->AttrIndx = ATTRINDX_DEVICE;
			pDesc->SH = ATTRIB_SH_OUTER_SHAREABLE;
			pDesc->PXN = 1;
		} else if (base_addr >= MEMORY_START_DMA && base_addr < MEMORY_END_DMA) {
			pDesc->AttrIndx = ATTRINDX_COHERENT;
			pDesc->SH = ATTRIB_SH_OUTER_SHAREABLE;
			pDesc->PXN = 1;
		}

		base_addr += ARMV8MMU_LEVEL3_PAGE_SIZE;
	}

	return pTable;
}

void enable_mmu()
{

	setup_ttb();

	uint64_t nMAIR_EL1 = 0xFF << ATTRINDX_NORMAL * 8	// inner/outer write-back non-transient, allocating
					   | 0x04 << ATTRINDX_DEVICE * 8	// Device-nGnRE
					   | 0x00 << ATTRINDX_COHERENT * 8; // Device-nGnRnE
	asm volatile("msr mair_el1, %0" : : "r"(nMAIR_EL1));

	asm volatile("msr ttbr0_el1, %0" : : "r"(&TTBL2));

	uint64_t nTCR_EL1;
	asm volatile("mrs %0, tcr_el1" : "=r"(nTCR_EL1));
	nTCR_EL1 &= ~(TCR_EL1_IPS__MASK | TCR_EL1_A1 | TCR_EL1_TG0__MASK | TCR_EL1_SH0__MASK | TCR_EL1_ORGN0__MASK |
				  TCR_EL1_IRGN0__MASK | TCR_EL1_EPD0 | TCR_EL1_T0SZ__MASK);
	nTCR_EL1 |= TCR_EL1_EPD1 | TCR_EL1_TG0_64KB << TCR_EL1_TG0__SHIFT | TCR_EL1_SH0_INNER << TCR_EL1_SH0__SHIFT |
				TCR_EL1_ORGN0_WR_BACK_ALLOCATE << TCR_EL1_ORGN0__SHIFT |
				TCR_EL1_IRGN0_WR_BACK_ALLOCATE << TCR_EL1_IRGN0__SHIFT | TCR_EL1_IPS_4GB << TCR_EL1_IPS__SHIFT |
				TCR_EL1_T0SZ_4GB << TCR_EL1_T0SZ__SHIFT;
	asm volatile("msr tcr_el1, %0" : : "r"(nTCR_EL1));

	uint64_t nSCTLR_EL1;
	asm volatile("mrs %0, sctlr_el1" : "=r"(nSCTLR_EL1));
	nSCTLR_EL1 &= ~(SCTLR_EL1_WXN | SCTLR_EL1_A);
	nSCTLR_EL1 |= SCTLR_EL1_I | SCTLR_EL1_C | SCTLR_EL1_M;
	asm volatile("msr sctlr_el1, %0" : : "r"(nSCTLR_EL1) : "memory");
}
