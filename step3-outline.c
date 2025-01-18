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
#include "io.h"

/* hidden private state */
static int pushes=0;	       /* variable used to count interrupts */

#define BTN_MASK XGPIO_IR_CH1_MASK
#define SW_MASK XGPIO_IR_CH2_MASK

static u32 swState = 0b0;

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
	fflush(stdout);

	u32 btn_output = XGpio_DiscreteRead(dev, 1);

	u32 btn0_mask = 0b1;
	u32 btn1_mask = 0b10;
	u32 btn2_mask = 0b100;
	u32 btn3_mask = 0b1000;

	if ((btn_output & btn0_mask) == btn0_mask) {
		pushes++;
		led_toggle(0);
	}

	if ((btn_output & btn1_mask) == btn1_mask) {
			pushes++;
			led_toggle(1);
		}

	if ((btn_output & btn2_mask) == btn2_mask) {
			pushes++;
			led_toggle(2);
		}

	if ((btn_output & btn3_mask) == btn3_mask) {
			pushes++;
			led_toggle(3);
		}


	fflush(stdout);
	XGpio_InterruptEnable(dev, BTN_MASK);
}

void sw_handler(void *devicep) {


	XGpio *dev = (XGpio*)devicep;

	XGpio_InterruptClear(dev, SW_MASK);
	u32 sw_output = XGpio_DiscreteRead(dev, 1);
	fflush(stdout);
	u32 sw0_mask = 0b1;
	u32 sw1_mask = 0b10;
	u32 sw2_mask = 0b100;
	u32 sw3_mask = 0b1000;

	if ((sw_output & sw0_mask) != (swState & sw0_mask)) {
		pushes++;
		led_toggle(0);
	}

	if ((sw_output & sw1_mask) != (swState & sw1_mask)) {
		pushes++;
		led_toggle(1);
	}

	if ((sw_output & sw2_mask) != (swState & sw2_mask)) {
			pushes++;
			led_toggle(2);
		}

	if ((sw_output & sw3_mask) != (swState & sw3_mask)) {
			pushes++;
			led_toggle(3);
		}


		fflush(stdout);

	swState = XGpio_DiscreteRead(dev, 1);
	XGpio_InterruptEnable(dev, SW_MASK);
}


int main() {
  init_platform();				

  led_init();

  printf("[hello]\n"); /* so we are know its alive */
  pushes = 0;

  io_sw_init(sw_handler);
  io_btn_init(btn_handler);


  while (1) {
  	   int ch;

  	   int index = 0;
  	   char input[100];

  	   printf(">");
  	   fflush(stdout);

  	   while ((ch = getchar()) != '\r' && ch != EOF ) {
  		   input[index] = ch;
  		   printf("%c", ch);
  		   fflush(stdout);

  		   index++;
  	   }
  	   input[index] = '\0';
  	   printf("\n");
  	   fflush(stdout);

  	   if (strcmp(input, "q") == 0) {
  		   break;
  	   }

     	   if (index == 1) {

     		   char *ep;
     		   long value = strtol(input, &ep,10);
     		   if (*ep == '\0' && value >=0 && value <= 3) {

     			   switch (value) {
     			   case 0:
     				   led_toggle(0);
     				   break;
     			   case 1:
     				   led_toggle(1);
     				   break;
     			   case 2:
     				   led_toggle(2);
     				   break;
     			   case 3:
     				   led_toggle(3);
     				   break;
     			   }

     			   if (led_get(value) == true) {
     				   printf("[%ld on]\n", value);
     			   } else {
     				   printf("[%ld off]\n", value);
     			   }
     			 fflush(stdout);

     		   }  else {
     			   printf("\n");
     		   }

     	   } else {
     		   printf("\n");
     	   }
     	   fflush(stdout);
     }

  printf("\n[done]\n");

  io_sw_close();
  io_btn_close();

  return 0;
}

