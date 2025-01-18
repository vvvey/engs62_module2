/*
 * main.c -- A program to print a dot each time button 0 is pressed.
 *
 *  Some useful values:
 *  -- XPAR_AXI_GPIO_1_DEVICE_ID -- xparameters.h
 *  -- XPAR_FABRIC_GPIO_1_VEC_ID -- xparameters.h
 *  -- XGPIO_IR_CH1_MASK         -- xgpio_l.h (included by xgpio.h)
 */
#include <stdio.h>		/* getchar,printf */
#include <stdlib.h>		/* strtod */
#include <stdbool.h>		/* type bool */
#include <unistd.h>		/* sleep */
#include <string.h>

#include "platform.h"		/* ZYBO board interface */
#include "xil_types.h"		/* u32, s32 etc */
#include "xparameters.h"	/* constants used by hardware */

#include "gic.h"		/* interrupt controller interface */
#include "xgpio.h"		/* axi gpio interface */

/* hidden private state */
static XGpio btnport;	       /* btn GPIO port instance */
static XGpio swport;
static int pushes=0;	       /* variable used to count interrupts */


#define BTN_GPIO_DEVICE XPAR_AXI_GPIO_1_DEVICE_ID
#define SW_GPIO_DEVICE XPAR_AXI_GPIO_2_DEVICE_ID
#define BTN_INTERRUPT_ID XPAR_FABRIC_GPIO_1_VEC_ID
#define SW_INTERRUPT_ID XPAR_FABRIC_GPIO_2_VEC_ID
#define BTN_MASK XGPIO_IR_CH1_MASK
#define SW_MASK XGPIO_IR_CH2_MASK

#define OUTPUT 0
/*
 * controll is passed to this function when a button is pushed
 *
 * devicep -- ptr to the device that caused the interrupt
 */
void btn_handler(void *devicep) {
	/* coerce the generic pointer into a gpio */
	XGpio *dev = (XGpio*)devicep;

	XGpio_InterruptClear(dev, BTN_MASK);
	u32 interruptStatus = XGpio_InterruptGetStatus(dev);
	printf("the interrupt is %d ", interruptStatus);
	fflush(stdout);

	u32 btn_output = XGpio_DiscreteRead(dev, 1);

	u32 btn0_mask = 0b1;
	u32 btn1_mask = 0b10;
	u32 btn2_mask = 0b100;
	u32 btn3_mask = 0b1000;

	if ((btn_output & btn0_mask) == btn0_mask) {
		pushes++;
		led_toggle(0);
		printf(".");
	}

	if ((btn_output & btn1_mask) == btn1_mask) {
			pushes++;
			led_toggle(1);
			printf(".");
		}

	if ((btn_output & btn2_mask) == btn2_mask) {
			pushes++;
			led_toggle(2);
			printf(".");
		}

	if ((btn_output & btn3_mask) == btn3_mask) {
			pushes++;
			led_toggle(3);
			printf(".");
		}


	fflush(stdout);
	XGpio_InterruptEnable(dev, BTN_MASK);
}

void sw_handler(void *devicep) {
	XGpio *dev = (XGpio*)devicep;

	XGpio_InterruptClear(dev, SW_MASK);
	u32 sw_output = XGpio_DiscreteRead(dev, 1);
	printf("hello");
	fflush(stdout);
	u32 sw0_mask = 0b1;
	u32 sw1_mask = 0b10;
	u32 sw2_mask = 0b100;
	u32 sw3_mask = 0b1000;

	if ((sw_output & sw0_mask) == sw0_mask) {
		pushes++;
		led_toggle(0);
		printf(".");
	}

	if ((sw_output & sw1_mask) == sw1_mask) {
			pushes++;
			led_toggle(1);
			printf(".");
		}

	if ((sw_output & sw2_mask) == sw2_mask) {
			pushes++;
			led_toggle(2);
			printf(".");
		}

	if ((sw_output & sw3_mask) == sw3_mask) {
			pushes++;
			led_toggle(3);
			printf(".");
		}


		fflush(stdout);


	XGpio_InterruptEnable(dev, SW_MASK);
}


int main() {
  init_platform();				

  /* initialize the gic (c.f. gic.h) */
  if (gic_init() == XST_SUCCESS) {
	  printf("gic init success \n");
  } else {
	  printf("gic init failed \n");
  }

  /* initialize btnport (c.f. module 1) and immediately dissable interrupts */
  XGpio_Initialize(&btnport, BTN_GPIO_DEVICE);
  XGpio_Initialize(&swport, SW_GPIO_DEVICE);

  XGpio_SetDataDirection(&btnport, 1, 0xFF);
  XGpio_SetDataDirection(&swport, 1, 0xFF);
  XGpio_InterruptDisable(&btnport, 0b11);
  XGpio_InterruptDisable(&swport, SW_MASK);


  /* connect handler to the gic (c.f. gic.h) */
  gic_connect(BTN_INTERRUPT_ID, btn_handler, &btnport);
  //gic_connect(SW_INTERRUPT_ID, sw_handler, &swport);

  /* enable interrupts on channel (c.f. table 2.1) */
  XGpio_InterruptEnable(&btnport, 0b11);
  //XGpio_InterruptEnable(&swport, SW_MASK);

  /* enable interrupt to processor (c.f. table 2.1) */
  XGpio_InterruptGlobalEnable(&btnport);
  //XGpio_InterruptGlobalEnable(&swport);

  led_init();

  printf("[hello]\n"); /* so we are know its alive */
  pushes=0;



  while(pushes<10) /* do nothing and handle interrupts */
	  ;

  printf("\n[done]\n");

  /* disconnect the interrupts (c.f. gic.h) */
  gic_disconnect(BTN_INTERRUPT_ID);
  //gic_disconnect(SW_INTERRUPT_ID);
  /* close the gic (c.f. gic.h) */
  gic_close();
  cleanup_platform();					/* cleanup the hardware platform */
  return 0;
}

