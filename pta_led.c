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

#define TA_NAME			"gpio.ta"

#define GPIO_UUID 	{ 0xd96a5b40, 0xe2c7, 0xb1af, { 0x87, 0x98, 0x16, 0x02, 0xc5, 0xff, 0xa8, 0x10 } }
#define GPIO_ON			0
#define GPIO_OFF		1


/* BCM2537 GPIO Register Address */
#define BCM2537_GPIO_BASE     0x3F200000

#define BCM2537_GPIO_GPFSEL0	0x00
#define BCM2537_GPIO_GPFSEL1	0x04
#define BCM2537_GPIO_GPFSEL2	0x08
#define BCM2537_GPIO_GPFSEL3	0x0C
#define BCM2537_GPIO_GPFSEL4	0x10
#define BCM2537_GPIO_GPFSEL5	0x14

#define BCM2537_GPIO_GPSET0		0x1C
#define BCM2537_GPIO_GPSET1		0x20

#define BCM2537_GPIO_GPCLR0		0x28
#define BCM2537_GPIO_GPCLR1		0x2C

#define BCM2537_GPIO_GPLEV0		0x34
#define BCM2537_GPIO_GPLEV1		0x38

#define BCM2537_GPIO_GPEDS0		0x40
#define BCM2537_GPIO_GPEDS1		0x44

#define BCM2537_GPIO_GPREN0		0x4C
#define BCM2537_GPIO_GPREN1		0x50

#define BCM2537_GPIO_GPFEN0		0x58
#define BCM2537_GPIO_GPFEN1		0x5C


/* BCM2537 GPIO Function Selection Options */
#define	BCM2537_GPIO_FSEL_IN		0b000
#define	BCM2537_GPIO_FSEL_OUT		0b001
#define	BCM2537_GPIO_FSEL_ALT1  0b100
#define	BCM2537_GPIO_FSEL_ALT2  0b101
#define	BCM2537_GPIO_FSEL_ALT3  0b110
#define	BCM2537_GPIO_FSEL_ALT4  0b011
#define	BCM2537_GPIO_FSEL_ALT5  0b010

volatile struct bm2837_gpio {
	uint32_t GPFSEL[6];
	uint32_t : 32;
	uint32_t GPSET[2];
	uint32_t : 32;
	uint32_t GPCLR[2];
	uint32_t : 32;
	uint32_t GPLEV[2];
	uint32_t : 32;
	uint32_t GPEDS[2];
	uint32_t : 32;
	uint32_t GPREN[2];
	uint32_t : 32;
	uint32_t GPFEN[2];
	uint32_t : 32;
	uint32_t GPHEN[2];
	uint32_t : 32;
	uint32_t GPLEN[2];
	uint32_t : 32;
	uint32_t GPAREN[2];
	uint32_t : 32;
	uint32_t GPAFEN[2];
	uint32_t : 32;
	uint32_t GPPUD;
	uint32_t GPPUDCLK[2];
} *gpio;

#define fsel_grp(pin) 									pin/10
#define pin_grp(pin)										pin/32
#define make_fsel_config(pin, option) 	option << (pin%10 * 3)
#define make_pin_set_config(pin) 				1 << pin%32
#define make_pin_clr_config(pin) 				1 << pin%32

static void gpio_init(void) {
	gpio = core_mmu_get_va(BCM2537_GPIO_BASE, MEM_AREA_IO_NSEC);
}
static void gpio_test(unsigned int pin) {
	gpio->GPFSEL[fsel_grp(pin)] |= make_fsel_config(pin, BCM2537_GPIO_FSEL_OUT);
	gpio->GPSET[pin_grp(pin)] = make_pin_set_config(pin);
}

static TEE_Result invoke_command(void *session_context __unused, uint32_t cmd_id, uint32_t param_types, TEE_Param params[TEE_NUM_PARAMS]) {				
	gpio_init();
	gpio_test(23);
	gpio_test(18);
	return TEE_SUCCESS;
}

pseudo_ta_register(.uuid = GPIO_UUID, .name = TA_NAME, .flags = PTA_DEFAULT_FLAGS, .invoke_command_entry_point = invoke_command);
