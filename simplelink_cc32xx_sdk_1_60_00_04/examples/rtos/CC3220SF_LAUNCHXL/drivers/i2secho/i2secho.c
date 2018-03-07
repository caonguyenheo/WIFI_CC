/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== i2secho.c ========
 */
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2S.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2s/I2SCC32XXDMA.h>

/* Example/Board Header files */
#include "Board.h"
#include "ti_codec.h"

#define THREADSTACKSIZE   2048

/* I2S buffer size */
#define BUFSIZE 1024

/* Total number of buffers to loop through */
#define NUMBUFS 10

/* Number of I2S buffer descriptors */
#define NUMDESC 3

/*
 *  Make rawSoundData array of uint32_t to ensure 32-bit alignment for DMA.
 *  It will hold NUMBUFS buffers of size BUFSIZE (MAUs).
 */
uint32_t rawSoundData[(BUFSIZE / sizeof(uint32_t)) * NUMBUFS];

sem_t semWaterMark;

I2S_Handle i2sHandle;
I2C_Handle i2cHandle;

/*
 *  ======== rxEchoThread ========
 */
void *rxEchoThread(void *arg0)
{
    uint32_t *ptr = rawSoundData;
    unsigned int i = 0;
    I2SCC32XXDMA_SerialPinParams I2SCC32XXDMA_serialParams;
    I2C_Params i2cParams;
    I2S_Params i2sParams;
    I2S_BufDesc bufReadDesc[NUMDESC];
    I2S_BufDesc *pReadDesc = NULL;

    /*
     *  Initialize the I2S buffer descriptors to the first NUMDESC
     *  buffers in rawSoundData.
     */
    for (i = 0; i < NUMDESC; i++) {
        bufReadDesc[i].bufPtr = ptr;
        bufReadDesc[i].bufSize = BUFSIZE;
        ptr += BUFSIZE / sizeof(uint32_t);
    }

    sem_init(&semWaterMark, 0 /* PTHREAD_PROCESS_PRIVATE */, 0);

    /*
     *  Open an I2C object which is used to initialize the codec in the
     *  function ConfigureAudioCodec().
     */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cHandle = I2C_open(Board_I2C0, &i2cParams);

    if (i2cHandle == NULL) {
        /* Error Initializing I2C */
        while (1);
    }

    /* Configure Audio Codec */
    ConfigureAudioCodec(CODEC_I2S_WORD_LEN_24);

    I2S_Params_init(&i2sParams);
    I2SCC32XXDMA_Params_init(&I2SCC32XXDMA_serialParams);
    i2sParams.customParams = (void *) &I2SCC32XXDMA_serialParams;
    i2sParams.operationMode = I2S_OPMODE_TX_RX_SYNC;
    i2sHandle = I2S_open(Board_I2S0, &i2sParams);

    for (i = 0; i < NUMDESC; i++) {
        /*
         *  Note:  I2S_readIssue() returns 0 on success and non-zero on
         *  failure, however, we will not check the return code for this
         *  simple example.  However, a real application may need to
         *  handle errors appropriately.
         */
        I2S_readIssue(i2sHandle, &bufReadDesc[i]);
    }

    sem_post(&semWaterMark);

    i = NUMDESC;  /* The next buffer in rawSoundData */

    while (1) {
        if (i == NUMBUFS) {
            ptr = rawSoundData;
            i = 0;
        }

        /*
         *  Note:  I2S_readReclaim() returns the number of bytes read.
         *  We will not check the return value for this simple example.
         */
        I2S_readReclaim(i2sHandle,&pReadDesc);

        pReadDesc->bufPtr = ptr;
        pReadDesc->bufSize = BUFSIZE;

        I2S_readIssue(i2sHandle, pReadDesc);
        ptr += BUFSIZE / sizeof(uint32_t);
        i++;
    }
}

/*
 *  ======== txEchoThread ========
 */
void *txEchoThread(void *arg0)
{
    uint32_t *ptr = rawSoundData;
    unsigned int i = 0;
    I2S_BufDesc bufDesc[NUMDESC];
    I2S_BufDesc *pDesc = NULL;
    size_t bytesWritten;

    for (i = 0; i < NUMDESC; i++) {
        bufDesc[i].bufPtr = ptr;
        bufDesc[i].bufSize = BUFSIZE;
        ptr += BUFSIZE / sizeof(uint32_t);
    }

    sem_wait(&semWaterMark);

    for (i = 0; i < NUMDESC; i++) {
        /*
         *  Note:  I2S_writeIssue() returns 0 on success and non-zero on
         *  failure, however, we will not check the return code for this
         *  simple example.
         */
        I2S_writeIssue(i2sHandle, &bufDesc[i]);
    }

    i = NUMDESC;
    while (1) {
        if (i == NUMBUFS) {
            ptr = rawSoundData;
            i = 0;
        }

        bytesWritten = I2S_writeReclaim(i2sHandle, &pDesc);
        if (bytesWritten != BUFSIZE) {
            /* I2S_writeReclaim() failed */
            while (1);
        }
        pDesc->bufPtr = ptr;
        pDesc->bufSize = BUFSIZE;
        I2S_writeIssue(i2sHandle, pDesc);
        i++;
        ptr += BUFSIZE / sizeof(uint32_t);
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    pthread_t           thread0, thread1;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    /* Call driver init functions */
    I2C_init();
    I2S_init();

#if 0 /* Disable GPIO due to an LED/I2S pin conflict */
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(Board_GPIO_LED0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(Board_GPIO_LED0, Board_GPIO_LED_ON);
#endif

    /* Set priority and stack size attributes */
    pthread_attr_init(&attrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    pthread_attr_setschedparam(&attrs, &priParam);

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create receive thread */
    retc = pthread_create(&thread0, &attrs, rxEchoThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    /* Create transmit thread */
    retc = pthread_create(&thread1, &attrs, txEchoThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    return (NULL);
}
