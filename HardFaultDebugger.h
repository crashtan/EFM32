/*
 * HardFaultDebugger.h
 *
 *  Created on: May 21, 2018
 *      Author: moiz.hussain
 */

#ifndef HARDFAULTDEBUGGER_H_
#define HARDFAULTDEBUGGER_H_

#include "em_chip.h"
#include "em_device.h"

//=============================================================================
// HARDWARE FAULT DEBUGGER
//=============================================================================
extern "C" {
	void debugHardfault(uint32_t *sp)
	{
		uint32_t cfsr  = SCB->CFSR;
		uint32_t hfsr  = SCB->HFSR;
		uint32_t mmfar = SCB->MMFAR;
		uint32_t bfar  = SCB->BFAR;

		uint32_t r0  = sp[0];
		uint32_t r1  = sp[1];
		uint32_t r2  = sp[2];
		uint32_t r3  = sp[3];
		uint32_t r12 = sp[4];
		uint32_t lr  = sp[5];
		uint32_t pc  = sp[6];
		uint32_t psr = sp[7];

		//Soft Debug loop wait. Use appropriate register in variables view to debug.
		while(1);
	}

	__attribute__( (naked) )
	void HardFault_Handler(void)
	{
		__asm volatile
		(
			"tst lr, #4                                    \n"
			"ite eq                                        \n"
			"mrseq r0, msp                                 \n"
			"mrsne r0, psp                                 \n"
			"ldr r1, debugHardfault_address                \n"
			"bx r1                                         \n"
			"debugHardfault_address: .word debugHardfault  \n"
		);
	}
}
//====================================================================================

#endif /* HARDFAULTDEBUGGER_H_ */
