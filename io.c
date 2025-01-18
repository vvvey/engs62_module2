/*
 * io.h -- switch and button module interface
 *
 */
#pragma once

#include <stdio.h>			/* printf for errors */
#include <stdbool.h>
#include <xgpio.h>		  	/* axi gpio */
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */

#include "gic.h"		/* interrupt controller interface */


#include "io.h"

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

static XGpio btnport;
static XGpio swport;


/*
 * initialize the btns providing a callback
 */
void io_btn_init(void (*btn_callback)(u32 btn)) {
    init_platform();

    /* initialize the gic (c.f. gic.h) */
    if (gic_init() == XST_SUCCESS) {
        printf("gic initialization successful\n");
    } else {
        printf("gic initialization failed\n");
    }

    /* initialize btnport (c.f. module 1) and immediately dissable interrupts */
    XGpio_Initialize(&btnport, XPAR_AXI_GPIO_1_DEVICE_ID);
    XGpio_SetDataDirection(&btnport, 1, 0xF);
    XGpio_InterruptDisable(&btnport, 1);

    /* connect handler to the gic (c.f. gic.h) */
    gic_connect(XPAR_FABRIC_GPIO_1_VEC_ID, btn_callback, &btnport);

    /* enable interrupts on channel (c.f. table 2.1) */
    XGpio_InterruptEnable(&btnport, 1);

    /* enable interrupt to processor (c.f. table 2.1) */
    XGpio_InterruptGlobalEnable(&btnport);
}

/*
 * close the btns
 */
void io_btn_close(void) {
	/* disconnect the interrupts (c.f. gic.h) */
	gic_disconnect(XPAR_FABRIC_GPIO_1_VEC_ID);

	/* close the gic (c.f. gic.h) */
	gic_close();

	/* cleanup the hardware platform */
	cleanup_platform();
}


/*
 * initialize the switches providing a callback
 */
void io_sw_init(void (*sw_callback)(u32 sw)) {
	init_platform();

	if (gic_init() == XST_SUCCESS) {
		printf("gic initialization successful\n");
	} else {
		printf("gic initialization failed\n");
	}

    XGpio_Initialize(&swport, XPAR_AXI_GPIO_2_DEVICE_ID);
    XGpio_SetDataDirection(&swport, 1, 0xF);
    XGpio_InterruptDisable(&swport, 1);

    gic_connect(XPAR_FABRIC_GPIO_2_VEC_ID, sw_callback, &swport);

    XGpio_InterruptEnable(&swport, 1);

    XGpio_InterruptGlobalEnable(&swport);
}

/*
 * close the switches
 */
void io_sw_close(void) {
	gic_disconnect(XPAR_FABRIC_GPIO_2_VEC_ID);

	gic_close();

	cleanup_platform();
}
