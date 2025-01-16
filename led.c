
#include <stdio.h>
#include <stdbool.h>
#include <xgpio.h>		  	/* axi gpio */
#include <xgpiops.h>		/* processor gpio */
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"
#include "sleep.h"

/* led states */
#define LED_ON true
#define LED_OFF false

/* led colors */
#define RED 0
#define BLUE 1
#define GREEN 2
#define YELLOW 3

#define ALL 0xFFFFFFFF		/* A value designating ALL leds */

#define OUTPUT 0x0							/* setting GPIO direction to output */
#define CHANNEL1 1

#define LED0 0x1;
#define LED1 0b10;
#define LED2 0b100;
#define LED3 0b1000;
#define LED4 7



static XGpio port; // 4-bits leds
static XGpio port1; // For RGB led

static XGpioPs_Config* gpio_config;
static XGpioPs gpio;
/*
 * Initialize the led module
 */
void led_init(void) {
	XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);	/* initialize device AXI_GPIO_0 */
	XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);	    /* set tristate buffer to output */

	XGpio_Initialize(&port1, XPAR_AXI_GPIO_1_DEVICE_ID);	/* initialize device AXI_GPIO_1 */
	XGpio_SetDataDirection(&port1, CHANNEL1, OUTPUT);	    /* set tristate buffer to output */


	gpio_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&gpio, gpio_config, gpio_config->BaseAddr);

	XGpioPs_SetDirectionPin(&gpio, 7, 1);
	XGpioPs_SetOutputEnablePin(&gpio, 7, 1);
}

/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate) {

	if (led == ALL) {
		if (tostate == LED_OFF) {
			XGpio_DiscreteWrite(&port, CHANNEL1, 0x0); // Turn off all led
		} else {
			XGpio_DiscreteWrite(&port, CHANNEL1, 0xF); // Turn on all led
		}

		return;
	}

	u32 led_bin;

	switch (led) {
	case 0:
		led_bin = LED0;
		break;
	case 1:
		led_bin = LED1;
		break;
	case 2:
		led_bin = LED2;
		break;
	case 3:
		led_bin = LED3;
		break;
	case 4:
		if (tostate == LED_ON) {
			XGpioPs_WritePin(&gpio, LED4, 1);
		} else {
			XGpioPs_WritePin(&gpio, LED4, 0);
		}
		return;
	case 5: // RED
		if (tostate == LED_ON) {
			XGpio_DiscreteWrite(&port1, CHANNEL1, 0b100);
		} else {
			XGpio_DiscreteWrite(&port1, CHANNEL1, 0b000);
		}

		return;
	case 6: // BLUE
		XGpio_DiscreteWrite(&port1, CHANNEL1, 0b001);
		return;
	case 7: // GREEN
		XGpio_DiscreteWrite(&port1, CHANNEL1, 0b010);
		return;
	case 8: // YELLOW
		XGpio_DiscreteWrite(&port1, CHANNEL1, 0b110);
		return;

	default:
		return; // invalid
	}

	// 4-bit LEDs
	u32 current = XGpio_DiscreteRead(&port, CHANNEL1);

	if (tostate == LED_ON) {
		u32 result = led_bin | current;
		XGpio_DiscreteWrite(&port, CHANNEL1, result); // Turn on
	} else {
		u32 result = led_bin ^ current;
		XGpio_DiscreteWrite(&port, CHANNEL1, result); // Turn off
	}

}

/*
 * Get the status of <led>
 *
 * <led> is a number >= 0
 * returns {LED_ON,LED_OFF,...}; LED_OFF if <led> is invalid
 */
bool led_get(u32 led) {
	u32 led_bin;

	switch (led) {
	case 0:
		led_bin = LED0;
		break;
	case 1:
		led_bin = LED1;
		break;
	case 2:
		led_bin = LED2;
		break;
	case 3:
		led_bin = LED3;
		break;
	default:
		return LED_OFF; // invalid
	}

	u32 led_status = XGpio_DiscreteRead(&port, CHANNEL1);

	if ((led_bin & led_status) == led_bin) {
		return LED_ON;
	} else {
		return LED_OFF;
	}

}

/*
 * Toggle <led>
 *
 * <led> is a number >= 0
 * Does nothing if <led> is invalid
 */
void led_toggle(u32 led) {
	bool led_status = led_get(led);
	if (led_status == LED_OFF) {
		led_set(led, LED_ON);
	} else {
		led_set(led, LED_OFF);
	}
}
