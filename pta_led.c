// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2015, Linaro Limited
 */
#include <compiler.h>
#include <stdio.h>
#include <trace.h>
#include <kernel/pseudo_ta.h>
#include <mm/tee_pager.h>
#include <mm/tee_mm.h>
#include <string.h>
#include <string_ext.h>
#include <malloc.h>
#include <io.h>
#include <mm/core_mmu.h>
#include <mm/core_memprot.h>

#define TA_NAME		"gpio.ta"

#define GPIO_UUID \
		{ 0xd96a5b40, 0xe2c7, 0xb1af, \
			{ 0x87, 0x98, 0x16, 0x02, 0xc5, 0xff, 0xa8, 0x10 } }
#define GPIO_ON             0
#define GPIO_OFF            1

#define GPIO_HIGH						1
#define GPIO_LOW						0


/* BCM2537 GPIO Register Address */
#define BCM2537_PERI_BASE				0x3F000000
#define BCM2537_GPIO_BASE      	(BCM2537_PERI_BASE + 0x200000)

#define BCM2537_GPIO_GPFSEL0		0x00
#define BCM2537_GPIO_GPFSEL1		0x04
#define BCM2537_GPIO_GPFSEL2		0x08
#define BCM2537_GPIO_GPFSEL3		0x0C
#define BCM2537_GPIO_GPFSEL4		0x10
#define BCM2537_GPIO_GPFSEL5		0x14

#define BCM2537_GPIO_GPSET0			0x1C
#define BCM2537_GPIO_GPSET1			0x20

#define BCM2537_GPIO_GPCLR0			0x28
#define BCM2537_GPIO_GPCLR1			0x2C

#define BCM2537_GPIO_GPLEV0			0x34
#define BCM2537_GPIO_GPLEV1			0x38

#define BCM2537_GPIO_GPEDS0			0x40
#define BCM2537_GPIO_GPEDS1			0x44

#define BCM2537_GPIO_GPREN0			0x4C
#define BCM2537_GPIO_GPREN1			0x50

#define BCM2537_GPIO_GPFEN0			0x58
#define BCM2537_GPIO_GPFEN1			0x5C


/* BCM2537 GPIO Function Selection Options */
#define	BCM2537_GPIO_FSEL_IN  0b000
#define	BCM2537_GPIO_FSEL_OUT  0b001
#define	BCM2537_GPIO_FSEL_ALT1  0b100
#define	BCM2537_GPIO_FSEL_ALT2  0b101
#define	BCM2537_GPIO_FSEL_ALT3  0b110
#define	BCM2537_GPIO_FSEL_ALT4  0b011
#define	BCM2537_GPIO_FSEL_ALT5  0b010



static void gpio_test(void) {
	vaddr_t gpio_va = core_mmu_get_va(BCM2537_GPIO_BASE, MEM_AREA_IO_NSEC);
  IMSG("Get GPIO VA: %x0x", gpio_va);

	const uint8_t pin = 23;

	uint32_t fsel = io_read32(gpio_va + BCM2537_GPIO_GPFSEL2);
	fsel |= BCM2537_GPIO_FSEL_OUT << (pin%10 * 3);
	io_write32(gpio_va + BCM2537_GPIO_GPFSEL2, fsel);
	io_write32(gpio_va + BCM2537_GPIO_GPSET0, GPIO_HIGH << pin);

	return TEE_SUCCESS;
}

static TEE_Result invoke_command(
	void *psess __unused,
	uint32_t cmd,
	uint32_t ptypes, TEE_Param params[TEE_NUM_PARAMS]
) {
	(void)ptypes;
	(void)params;

	IMSG("GPIO PTA");
	gpio_test();

	return TEE_SUCCESS;
	// switch (cmd) {
	// 	case GPIO_ON:
	// 		return testGpioOn();
	// 	case GPIO_OFF:
	// 		return testGpioOff();
	// 	default:
	// 		break;
	// }
	// return TEE_ERROR_BAD_PARAMETERS;
}

pseudo_ta_register(.uuid = GPIO_UUID, .name = TA_NAME,
		   .flags = PTA_DEFAULT_FLAGS,
		   .invoke_command_entry_point = invoke_command);
