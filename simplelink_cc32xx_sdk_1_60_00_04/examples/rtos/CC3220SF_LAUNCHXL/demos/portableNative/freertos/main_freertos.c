/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,

 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */
#include <stdint.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

/* Driver header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "Board.h"

extern void *temperatureThread(void *arg0);
extern void *consoleThread(void *arg0);

/* Stack size in 16-bit words */
#define THREADSTACKSIZE    768 / sizeof(portSTACK_TYPE)

/*
 *  ======== main ========
 */
int main(void)
{
    TaskHandle_t consoleHandle;
    TaskHandle_t temperatureHandle;
    BaseType_t   retc;

    /* Call driver init functions */
    Board_initGeneral();

    retc = xTaskCreate((TaskFunction_t)consoleThread,     // pvTaskCode
                                NULL,                     // pcName
                                THREADSTACKSIZE,          // usStackDepth
                                NULL,                     // pvParameters
                                1,                        // uxPriority
                                &consoleHandle);          // pxCreatedTask
    if (retc != pdPASS) {
        /* xTaskCreate() failed */
        while (1);
    }

    retc = xTaskCreate((TaskFunction_t)temperatureThread, // pvTaskCode
                       NULL,                              // pcName
                       THREADSTACKSIZE,                   // usStackDepth
                       NULL,                              // pvParameters
                       2,                                 // uxPriority
                       &temperatureHandle);               // pxCreatedTask
    if (retc != pdPASS) {
        /* xTaskCreate() failed */
        while (1);
    }

    /* Initialize the GPIO since multiple threads are using it */
    GPIO_init();

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
