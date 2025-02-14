#include "rk_gic.h"
#include <stdint.h>

#define __IM volatile const			  //!< \brief Defines 'read only' structure member permissions
#define __OM volatile				  //!< \brief Defines 'write only' structure member permissions
#define __IOM volatile				  //!< \brief Defines 'read / write' structure member permissions
#define RESERVED(N, T) T RESERVED##N; // placeholder struct members used for "reserved" areas

using IRQn_Type = uint32_t;

// GICD
struct GICD_t {
	__IOM uint32_t CTLR;			// Offset: 0x000 (R/W) Distributor Control Register
	__IM uint32_t TYPER;			// Offset: 0x004 (R/ ) Interrupt Controller Type Register
	__IM uint32_t IIDR;				// Offset: 0x008 (R/ ) Distributor Implementer Identification Register
	RESERVED(0, uint32_t)			// TYPER2
	__IOM uint32_t STATUSR;			// Offset: 0x010 (R/W) Error Reporting Status Register, optional
	RESERVED(1 [11], uint32_t)		//
	__OM uint32_t SETSPI_NSR;		// Offset: 0x040 ( /W) Set SPI Register
	RESERVED(2, uint32_t)			//
	__OM uint32_t CLRSPI_NSR;		// Offset: 0x048 ( /W) Clear SPI Register
	RESERVED(3, uint32_t)			//
	__OM uint32_t SETSPI_SR;		// Offset: 0x050 ( /W) Set SPI, Secure Register
	RESERVED(4, uint32_t)			//
	__OM uint32_t CLRSPI_SR;		// Offset: 0x058 ( /W) Clear SPI, Secure Register
	RESERVED(5 [9], uint32_t)		//
	__IOM uint32_t IGROUPR[32];		// Offset: 0x080 (R/W) Interrupt Group Registers
	__IOM uint32_t ISENABLER[32];	// Offset: 0x100 (R/W) Interrupt Set-Enable Registers
	__IOM uint32_t ICENABLER[32];	// Offset: 0x180 (R/W) Interrupt Clear-Enable Registers
	__IOM uint32_t ISPENDR[32];		// Offset: 0x200 (R/W) Interrupt Set-Pending Registers
	__IOM uint32_t ICPENDR[32];		// Offset: 0x280 (R/W) Interrupt Clear-Pending Registers
	__IOM uint32_t ISACTIVER[32];	// Offset: 0x300 (R/W) Interrupt Set-Active Registers
	__IOM uint32_t ICACTIVER[32];	// Offset: 0x380 (R/W) Interrupt Clear-Active Registers
	__IOM uint32_t IPRIORITYR[255]; // Offset: 0x400 (R/W) Interrupt Priority Registers
	RESERVED(6, uint32_t)			//
	__IOM uint32_t ITARGETSR[255];	// Offset: 0x800 (R/W) Interrupt Targets Registers
	RESERVED(7, uint32_t)			//
	__IOM uint32_t ICFGR[64];		// Offset: 0xC00 (R/W) Interrupt Configuration Registers
	__IOM uint32_t IGRPMODR[32];	// Offset: 0xD00 (R/W) Interrupt Group Modifier Registers
	RESERVED(8 [32], uint32_t)		//
	__IOM uint32_t NSACR[64];		// Offset: 0xE00 (R/W) Non-secure Access Control Registers
	__OM uint32_t SGIR;				// Offset: 0xF00 ( /W) Software Generated Interrupt Register
	RESERVED(9 [3], uint32_t)		//
	__IOM uint32_t CPENDSGIR[4];	// Offset: 0xF10 (R/W) SGI Clear-Pending Registers
	__IOM uint32_t SPENDSGIR[4];	// Offset: 0xF20 (R/W) SGI Set-Pending Registers
	RESERVED(10 [5236], uint32_t)	//
	__IOM uint64_t IROUTER[988];	// Offset: 0x6100(R/W) Interrupt Routing Registers
};

// GICC
// typedef struct {
// 	__IOM uint32_t CTLR;		// Offset: 0x000 (R/W) CPU Interface Control Register
// 	__IOM uint32_t PMR;			// Offset: 0x004 (R/W) Interrupt Priority Mask Register
// 	__IOM uint32_t BPR;			// Offset: 0x008 (R/W) Binary Point Register
// 	__IM uint32_t IAR;			// Offset: 0x00C (R/ ) Interrupt Acknowledge Register
// 	__OM uint32_t EOIR;			// Offset: 0x010 ( /W) End Of Interrupt Register
// 	__IM uint32_t RPR;			// Offset: 0x014 (R/ ) Running Priority Register
// 	__IM uint32_t HPPIR;		// Offset: 0x018 (R/ ) Highest Priority Pending Interrupt Register
// 	__IOM uint32_t ABPR;		// Offset: 0x01C (R/W) Aliased Binary Point Register
// 	__IM uint32_t AIAR;			// Offset: 0x020 (R/ ) Aliased Interrupt Acknowledge Register
// 	__OM uint32_t AEOIR;		// Offset: 0x024 ( /W) Aliased End Of Interrupt Register
// 	__IM uint32_t AHPPIR;		// Offset: 0x028 (R/ ) Aliased Highest Priority Pending Interrupt Register
// 	__IOM uint32_t STATUSR;		// Offset: 0x02C (R/W) Error Reporting Status Register, optional
// 	RESERVED(1 [40], uint32_t)	//
// 	__IOM uint32_t APR[4];		// Offset: 0x0D0 (R/W) Active Priority Register
// 	__IOM uint32_t NSAPR[4];	// Offset: 0x0E0 (R/W) Non-secure Active Priority Register
// 	RESERVED(2 [3], uint32_t)	//
// 	__IM uint32_t IIDR;			// Offset: 0x0FC (R/ ) CPU Interface Identification Register
// 	RESERVED(3 [960], uint32_t) //
// 	__OM uint32_t DIR;			// Offset: 0x1000( /W) Deactivate Interrupt Register
// } GICC_t;

struct GICR_t {
	uint32_t CTLR;
	uint32_t IIDR;
	uint32_t TYPER;
	uint32_t STATUSR;
	uint32_t WAKER;

	// pad
	uint32_t skip1[26];
	uint32_t IGROUPR[32];
	uint32_t ISENABLER[32];
	uint32_t ICENABLER[32];
	uint32_t ISPENDR[32];
	uint32_t ICPENDR[32];
	uint32_t ISACTIVER[32]; // 0x300
	uint32_t ICACTIVER[32];

	// TODO...

	void Wake() volatile {
		enum : uint32_t { ProcSleep = (1 << 1), ChildrenAsleep = (1 << 2) };

		WAKER &= ~ProcSleep;
		while (WAKER & (1 << ChildrenAsleep))
			;
	}
};

namespace HW
{

static inline volatile GICR_t *const GICRedist = reinterpret_cast<GICR_t *>(GIC_REDISTRIBUTOR_BASE);
static inline volatile GICD_t *const GICDistributor = reinterpret_cast<GICD_t *>(GIC_DISTRIBUTOR_BASE);
// static inline volatile GICC_t *const GICInterface = reinterpret_cast<GICC_t *>(GIC_INTERFACE_BASE);

} // namespace HW

/** \brief  Enable the interrupt distributor using the GIC's CTLR register.
 */
static inline void GIC_EnableGroup0(void) {
	HW::GICDistributor->CTLR |= 1U;
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN0_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EnableGroup1NS(void) {
	HW::GICDistributor->CTLR |= (1 << 1);
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN1_EL1, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_EnableGroup1S(void) {
	HW::GICDistributor->CTLR |= (1 << 2);
	uint64_t reg = 0b1;
	asm volatile("msr ICC_IGRPEN1_EL1, %0\n\t" : : "r"(reg) : "memory");
}

/** \brief Disable the interrupt distributor using the GIC's CTLR register.
 */
static inline void GIC_DisableGroup0(void) {
	HW::GICDistributor->CTLR &= ~1U;
}
static inline void GIC_DisableGroup1NS(void) {
	HW::GICDistributor->CTLR &= ~(1 << 1);
}
static inline void GIC_DisableGroup1S(void) {
	HW::GICDistributor->CTLR &= ~(1 << 2);
}

/** \brief Read the GIC's TYPER register.
 * \return GICD_t::TYPER
 */
static inline uint32_t GIC_DistributorInfo(void) {
	return (HW::GICDistributor->TYPER);
}

/** \brief Reads the GIC's IIDR register.
 * \return GICD_t::IIDR
 */
static inline uint32_t GIC_DistributorImplementer(void) {
	return (HW::GICDistributor->IIDR);
}

/** \brief Sets the GIC's ITARGETSR register for the given interrupt.
 * \param [in] IRQn Interrupt to be configured.
 * \param [in] cpu_target CPU interfaces to assign this interrupt to.
 */
static inline void GIC_SetTarget(IRQn_Type IRQn, uint32_t cpu_target) {
	uint32_t mask = HW::GICDistributor->ITARGETSR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
	HW::GICDistributor->ITARGETSR[IRQn / 4U] = mask | ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the GIC's ITARGETSR register.
 * \param [in] IRQn Interrupt to acquire the configuration for.
 * \return GICD_t::ITARGETSR
 */
static inline uint32_t GIC_GetTarget(IRQn_Type IRQn) {
	return (HW::GICDistributor->ITARGETSR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

// static inline void GIC_EnableInterface(void) {
// 	HW::GICInterface->CTLR |= 1U; // enable interface
// 	// asm volatile("msr ICC_CTLR_EL1, %0\n\t" : : "r"(reg) : "memory");
// }

// static inline void GIC_DisableInterface(void) {
// 	HW::GICInterface->CTLR &= ~1U; // disable distributor
// }

/** \brief Enables the given interrupt using GIC's ISENABLER register.
 * \param [in] IRQn The interrupt to be enabled.
 */
static inline void GIC_EnableIRQ(IRQn_Type IRQn) {
	HW::GICDistributor->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
	HW::GICRedist->ISENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt enable status using GIC's ISENABLER register.
 * \param [in] IRQn The interrupt to be queried.
 * \return 0 - interrupt is not enabled, 1 - interrupt is enabled.
 */
static inline uint32_t GIC_GetEnableIRQ(IRQn_Type IRQn) {
	return (HW::GICDistributor->ISENABLER[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

/** \brief Disables the given interrupt using GIC's ICENABLER register.
 * \param [in] IRQn The interrupt to be disabled.
 */
static inline void GIC_DisableIRQ(IRQn_Type IRQn) {
	HW::GICDistributor->ICENABLER[IRQn / 32U] = 1U << (IRQn % 32U);
}

/** \brief Get interrupt pending status from GIC's ISPENDR register.
 * \param [in] IRQn The interrupt to be queried.
 * \return 0 - interrupt is not pending, 1 - interrupt is pendig.
 */
static inline uint32_t GIC_GetPendingIRQ(IRQn_Type IRQn) {
	uint32_t pend;

	if (IRQn >= 16U) {
		pend = (HW::GICDistributor->ISPENDR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
	} else {
		// INTID 0-15 Software Generated Interrupt
		pend = (HW::GICDistributor->SPENDSGIR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
		// No CPU identification offered
		if (pend != 0U) {
			pend = 1U;
		} else {
			pend = 0U;
		}
	}

	return (pend);
}

/** \brief Sets the given interrupt as pending using GIC's ISPENDR register.
 * \param [in] IRQn The interrupt to be enabled.
 */
static inline void GIC_SetPendingIRQ(IRQn_Type IRQn) {
	if (IRQn >= 16U) {
		HW::GICDistributor->ISPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
		HW::GICDistributor->SPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
	}
}

/** \brief Clears the given interrupt from being pending using GIC's ICPENDR register.
 * \param [in] IRQn The interrupt to be enabled.
 */
static inline void GIC_ClearPendingIRQ(IRQn_Type IRQn) {
	if (IRQn >= 16U) {
		HW::GICDistributor->ICPENDR[IRQn / 32U] = 1U << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
		HW::GICDistributor->CPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
	}
}

/** \brief Sets the interrupt configuration using GIC's ICFGR register.
 * \param [in] IRQn The interrupt to be configured.
 * \param [in] int_config Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
 *                                           Bit 1: 0 - level sensitive, 1 - edge triggered
 */
static inline void GIC_SetConfiguration(IRQn_Type IRQn, uint32_t int_config) {
	uint32_t icfgr = HW::GICDistributor->ICFGR[IRQn / 16U];
	uint32_t shift = (IRQn % 16U) << 1U;

	icfgr &= (~(3U << shift));
	icfgr |= (int_config << shift);

	HW::GICDistributor->ICFGR[IRQn / 16U] = icfgr;
}

/** \brief Get the interrupt configuration from the GIC's ICFGR register.
 * \param [in] IRQn Interrupt to acquire the configuration for.
 * \return Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
 *                                 Bit 1: 0 - level sensitive, 1 - edge triggered
 */
static inline uint32_t GIC_GetConfiguration(IRQn_Type IRQn) {
	return (HW::GICDistributor->ICFGR[IRQn / 16U] >> ((IRQn % 16U) >> 1U));
}

/** \brief Set the priority for the given interrupt in the GIC's IPRIORITYR register.
 * \param [in] IRQn The interrupt to be configured.
 * \param [in] priority The priority for the interrupt, lower values denote higher priorities.
 */
static inline void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
	uint32_t mask = HW::GICDistributor->IPRIORITYR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
	HW::GICDistributor->IPRIORITYR[IRQn / 4U] = mask | ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Read the current interrupt priority from GIC's IPRIORITYR register.
 * \param [in] IRQn The interrupt to be queried.
 */
static inline uint32_t GIC_GetPriority(IRQn_Type IRQn) {
	return (HW::GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

static inline IRQn_Type GIC_AcknowledgePending(void) {
	uint64_t reg;
	__asm__ __volatile__("mrs %0, ICC_IAR_EL2\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline void GIC_EndInterrupt(IRQn_Type IRQn) {
	uint64_t reg = IRQn;
	asm volatile("msr ICC_EOIR_EL2, %0\n\t" : : "r"(reg) : "memory");
}

static inline void GIC_SetInterfacePriorityMask(uint64_t priority) {
	asm volatile("msr ICC_PMR_EL1, %0\n\t" : : "r"(priority) : "memory");
}

static inline uint32_t GIC_GetInterfacePriorityMask(void) {
	uint64_t reg;
	asm volatile("mrs %0, ICC_PMR_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline void GIC_SetBinaryPoint(uint64_t binary_point) {
	asm volatile("msr ICC_BPR0_EL1, %0\n\t" : : "r"(binary_point) : "memory");
}

static inline void GIC_SetBinaryPointGroup1(uint64_t binary_point) {
	asm volatile("msr ICC_BPR1_EL1, %0\n\t" : : "r"(binary_point) : "memory");
}

static inline uint32_t GIC_GetBinaryPoint() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_BPR0_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

static inline uint32_t GIC_GetBinaryPointGroup1() {
	uint64_t reg;
	asm volatile("mrs %0, ICC_BPR1_EL1\n\t" : "=r"(reg) : : "memory");
	return reg;
}

/** \brief Get the status for a given interrupt.
 * \param [in] IRQn The interrupt to get status for.
 * \return 0 - not pending/active, 1 - pending, 2 - active, 3 - pending and active
 */
static inline uint32_t GIC_GetIRQStatus(IRQn_Type IRQn) {
	uint32_t pending, active;

	active = ((HW::GICDistributor->ISACTIVER[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;
	pending = ((HW::GICDistributor->ISPENDR[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;

	return ((active << 1U) | pending);
}

/** \brief Generate a software interrupt using GIC's SGIR register.
 * \param [in] IRQn Software interrupt to be generated.
 * \param [in] target_list List of CPUs the software interrupt should be forwarded to.
 * \param [in] filter_list Filter to be applied to determine interrupt receivers.
 */
static inline void GIC_SendSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list) {
	HW::GICDistributor->SGIR = ((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
 * \return GICInterface_Type::HPPIR
 */
// static inline uint32_t GIC_GetHighPendingIRQ(void) {
// 	return HW::GICInterface->HPPIR;
// }

// /** \brief Provides information about the implementer and revision of the CPU interface.
//  * \return GICInterface_Type::IIDR
//  */
// static inline uint32_t GIC_GetInterfaceId(void) {
// 	return HW::GICInterface->IIDR;
// }

/** \brief Set the interrupt group from the GIC's IGROUPR register.
 * \param [in] IRQn The interrupt to be queried.
 * \param [in] group Interrupt group number: 0 - Group 0, 1 - Group 1
 */
static inline void GIC_SetGroup(IRQn_Type IRQn, uint32_t group) {
	uint32_t igroupr = HW::GICDistributor->IGROUPR[IRQn / 32U];
	uint32_t shift = (IRQn % 32U);

	igroupr &= (~(1U << shift));
	igroupr |= ((group & 1U) << shift);

	HW::GICDistributor->IGROUPR[IRQn / 32U] = igroupr;
}
#define GIC_SetSecurity GIC_SetGroup

/** \brief Get the interrupt group from the GIC's IGROUPR register.
 * \param [in] IRQn The interrupt to be queried.
 * \return 0 - Group 0, 1 - Group 1
 */
static inline uint32_t GIC_GetGroup(IRQn_Type IRQn) {
	return (HW::GICDistributor->IGROUPR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}

/** \brief Initialize the interrupt distributor.
 */
// static inline void GIC_DistInit(void) {
// 	uint32_t i;
// 	uint32_t num_irq = 0U;
// 	uint32_t priority_field;

// 	// A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
// 	// configuring all of the interrupts as Secure.

// 	// Disable interrupt forwarding
// 	GIC_DisableDistributor();
// 	// Get the maximum number of interrupts that the GIC supports
// 	num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

// 	/* Priority level is implementation defined.
// 	 To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
// 	 priority field and read back the value stored.*/
// 	GIC_SetPriority((IRQn_Type)0U, 0xFFU);
// 	priority_field = GIC_GetPriority((IRQn_Type)0U);

// 	for (i = 32U; i < num_irq; i++) {
// 		// Disable the SPI interrupt
// 		GIC_DisableIRQ((IRQn_Type)i);
// 		// Set level-sensitive (and N-N model)
// 		GIC_SetConfiguration((IRQn_Type)i, 0U);
// 		// Set priority
// 		GIC_SetPriority((IRQn_Type)i, priority_field / 2U);
// 		// Set target list to CPU0
// 		GIC_SetTarget((IRQn_Type)i, 1U);
// 	}
// 	// Enable distributor
// 	GIC_EnableDistributor();
// }

// /** \brief Initialize the CPU's interrupt interface
//  */
// static inline void GIC_CPUInterfaceInit(void) {
// 	uint32_t i;
// 	uint32_t priority_field;

// 	// A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
// 	// configuring all of the interrupts as Secure.

// 	// Disable interrupt forwarding
// 	GIC_DisableInterface();

// 	/* Priority level is implementation defined.
// 	 To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
// 	 priority field and read back the value stored.*/
// 	GIC_SetPriority((IRQn_Type)0U, 0xFFU);
// 	priority_field = GIC_GetPriority((IRQn_Type)0U);

// 	// SGI and PPI
// 	for (i = 0U; i < 32U; i++) {
// 		if (i > 15U) {
// 			// Set level-sensitive (and N-N model) for PPI
// 			GIC_SetConfiguration((IRQn_Type)i, 0U);
// 		}
// 		// Disable SGI and PPI interrupts
// 		GIC_DisableIRQ((IRQn_Type)i);
// 		// Set priority
// 		GIC_SetPriority((IRQn_Type)i, priority_field / 2U);
// 	}
// 	// Enable interface
// 	GIC_EnableInterface();
// 	// Set binary point to 0
// 	GIC_SetBinaryPoint(0U);
// 	// Set priority mask
// 	GIC_SetInterfacePriorityMask(0xFFU);
// }

// /** \brief Initialize and enable the GIC
//  */
// static inline void GIC_Enable(void) {
// 	GIC_DistInit();
// 	GIC_CPUInterfaceInit(); // per CPU
// }
