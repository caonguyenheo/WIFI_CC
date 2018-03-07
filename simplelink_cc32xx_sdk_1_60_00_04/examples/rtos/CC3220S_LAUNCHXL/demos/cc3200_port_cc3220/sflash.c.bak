//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************


//*****************************************************************************
//
// Application Name     -   File operations 
// Application Overview -   This example demonstate the file operations that 
//                          can be performed by the applications. The 
//                          application use the serial-flash as the storage 
//                          medium.
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_File_Operations
// or
// docs\examples\CC32xx_File_Operations.pdf
//
//*****************************************************************************


//*****************************************************************************
//
//! \addtogroup filesystem_demo
//! @{
//
//*****************************************************************************
#include <stdlib.h>
#include <string.h>

// Simplelink includes
//#include "simplelink.h"

//Driverlib includes
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_ints.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/interrupt.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/drivers/net/wifi/simplelink.h>
//Common interface includes
//#include "gpio_if.h"
//#include "common.h"
#ifndef NOTERM
//#include "uart_if.h"
#endif
//#include "pinmux.h"
#include "sflash.h"
#include "userconfig.h"
#include "network_terminal.h"

#define APPLICATION_NAME        "FILE OPERATIONS"
#define APPLICATION_VERSION     "1.1.1"

#define SL_MAX_FILE_SIZE        4L*1024L       /* 64KB file */
#define BUF_SIZE                2048
#define USER_FILE_NAME          "config.ini"
/* Application specific status/error codes */
typedef enum{
    // Choosing this number to avoid overlap w/ host-driver's error codes
    FILE_ALREADY_EXIST = -0x7D0,
    FILE_CLOSE_ERROR = FILE_ALREADY_EXIST - 1,
    FILE_NOT_MATCHED = FILE_CLOSE_ERROR - 1,
    FILE_OPEN_READ_FAILED = FILE_NOT_MATCHED - 1,
    FILE_OPEN_WRITE_FAILED = FILE_OPEN_READ_FAILED -1,
    FILE_READ_FAILED = FILE_OPEN_WRITE_FAILED - 1,
    FILE_WRITE_FAILED = FILE_READ_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
unsigned char gaucCmpBuf[BUF_SIZE];
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************


int InitFile(unsigned long *ulToken, long *lFileHandle)
{
    long lRetVal = -1;
    int32_t  OpenFlags = 0;
    OpenFlags = SL_FS_CREATE;
    OpenFlags |= SL_FS_CREATE_PUBLIC_WRITE;
    OpenFlags |= SL_FS_CREATE_PUBLIC_READ;

    //
    //  create a user file
    //
    if(sl_FsOpen((unsigned char *)USER_FILE_NAME, SL_FS_READ, ulToken) < 0)
    {
        // Create file if not exist
    	*lFileHandle = sl_FsOpen((unsigned char *)USER_FILE_NAME, OpenFlags | SL_FS_CREATE_MAX_SIZE(SL_MAX_FILE_SIZE), ulToken);

        if(*lFileHandle < 0)
        {
            //
            // File may already be created
            //
            lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
            ASSERT_ON_ERROR(lRetVal,FILE_CLOSE_ERROR);
        }
        lRetVal = sl_FsWrite(*lFileHandle,
                0, 
                (unsigned char *)get_userconfig_pointer(), get_userconfig_size()+1);
        if (lRetVal < 0){
        
            lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
            ASSERT_ON_ERROR(lRetVal,FILE_WRITE_FAILED);
        }
    }
    //
    // close the user file
    //
    lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        ASSERT_ON_ERROR(lRetVal,FILE_CLOSE_ERROR);
    }
    return 0;
    
}

//*****************************************************************************
//
//!  This funtion includes the following steps:
//!  -open a user file for writing
//!  -write "Old MacDonalds" child song 37 times to get just below a 64KB file
//!  -close the user file
//!
//!  /param[out] ulToken : file token
//!  /param[out] lFileHandle : file handle
//!
//!  /return  0:Success, -ve: failure
//
//*****************************************************************************
long WriteFileToDevice(unsigned long *ulToken, long *lFileHandle, unsigned char* data, int len)
{
    long lRetVal = -1;
    //
    //  open a user file for writing
    //
    *lFileHandle = sl_FsOpen((unsigned char *)USER_FILE_NAME,
    					SL_FS_WRITE,
                        ulToken);

    if(*lFileHandle < 0)
    {
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(lRetVal,FILE_OPEN_WRITE_FAILED);
    }
    
    //
    // write the data to file
    //
    
    lRetVal = sl_FsWrite(*lFileHandle,
                0, 
                (unsigned char *)data, len);
    if (lRetVal < 0)
    {
        lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(lRetVal,FILE_WRITE_FAILED);
    }
    
    
    //
    // close the user file
    //
    lRetVal = sl_FsClose(*lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        ASSERT_ON_ERROR(lRetVal,FILE_CLOSE_ERROR);
    }

    return 0;
}

//*****************************************************************************
//
//!  This funtion includes the following steps:
//!    -open the user file for reading
//!    -read the data and compare with the stored buffer
//!    -close the user file
//!
//!  /param[in] ulToken : file token
//!  /param[in] lFileHandle : file handle
//!
//!  /return 0: success, -ve:failure
//
//*****************************************************************************
long ReadFileFromDevice(unsigned long ulToken, long lFileHandle, unsigned char *data, int len)
{
    long lRetVal = -1;
    //
    // open a user file for reading
    //
    lFileHandle = sl_FsOpen((unsigned char *)USER_FILE_NAME,
    					SL_FS_READ,
                        &ulToken);
    if(lFileHandle < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(lRetVal,FILE_OPEN_READ_FAILED);
    }

    //
    // read the data 
    //
    lRetVal = sl_FsRead(lFileHandle,
                0,
                 data, len);
    if ((lRetVal < 0) || (lRetVal != len))
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(lRetVal,FILE_READ_FAILED);
    }
    //
    // close the user file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    if (SL_RET_CODE_OK != lRetVal)
    {
        ASSERT_ON_ERROR(lRetVal,FILE_CLOSE_ERROR);
    }

    return 0;
}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
