//*****************************************************************************
// httpserver_app.c
//
// camera application macro & APIs
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
//! \addtogroup Httpserverapp
//! @{
//
//*****************************************************************************

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
// Driverlib Includes

#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/utils.h>
// SimpleLink include
#include <ti/drivers/net/wifi/simplelink.h>

// Free-RTOS/TI-RTOS include
#include <osi.h>

// HTTP lib includes
#include "HttpCore.h"
#include "HttpRequest.h"

// Common-interface includes
//#include "network_if.h"
//#include "uart_if.h"
#include "network_terminal.h"
//#include "gpio_if.h"
//#include "button_if.h"

#include "websocketapp.h"
#include "httpserverapp.h"
//#include "i2cconfig.h"
// #include "mt9d111.h"
//#include "system.h"
//#include "userconfig.h"
// #include "httpclientapp.h"
// #include "httpcli.h"

#include "uart_term.h"
//#include "FreeRTOS.h"

// extern OsiSyncObj_t g_ConnectedToAP;
//extern OsiSyncObj_t g_registered;

/****************************************************************************/
/*				MACROS										*/
/****************************************************************************/
#define CAMERA_SERVICE_PRIORITY     1
#define STATE_REGISTRATION      0
#define STATE_MQTT              1
#define MAX_AP_SSID_LEN         32
#define MAX_AP_PASS_LEN         MAX_AP_SSID_LEN

/****************************************************************************
                              Global variables
****************************************************************************/
char *g_Buffer;
UINT8 g_success = 0;
int g_close = 0;
UINT16 g_uConnection;
OsiTaskHandle g_iCameraTaskHdl = 0;
volatile unsigned char g_ucProvisioningDone = 0;

char ap_ssid[MAX_AP_SSID_LEN] = {0};
char ap_pass[MAX_AP_PASS_LEN] = {0};
char apiKey[64]                = {0};
char timeZone[32]              = {0};

char topicID[64]               = {0};
char authenToken[64]           = {0};
volatile int system_state      =  STATE_REGISTRATION;                  
/****************************************************************************
                          functions implementation
****************************************************************************/
void WebSocketCloseSessionHandler(void)
{
	g_close = 1;
}

void CameraAppTask(void *param)
{
	UINT8 Opcode = 0x02;
	struct HttpBlob Write;

//	InitCameraComponents(640, 480);

	while(1)
	{
		if(g_close == 0)
		{
//			Write.uLength = StartCamera((char **)&Write.pData);

			if(!sl_WebSocketSend(g_uConnection, Write, Opcode))
			{
				while(1);
			}
		}
	}

}

/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in]  uConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *     					
 */
void WebSocketRecvEventHandler(UINT16 uConnection, char *ReadBuffer)
{
	char *camera = "capture";
//
//	/*
//	 * UINT8 Opcode;
//	 * struct HttpBlob Write;
//	*/
//
	g_uConnection = uConnection;

	g_Buffer = ReadBuffer;
	g_close = 0;
	if (!strcmp(ReadBuffer,camera))
	{
		if(!g_iCameraTaskHdl)
		{
			osi_TaskCreate(CameraAppTask,
								   "CameraApp",
									1024,
									NULL,
									CAMERA_SERVICE_PRIORITY,
									&g_iCameraTaskHdl);
		}

	}
	//Free memory as we are not using anywhere later
	free(g_Buffer);
	g_Buffer = NULL;
//	/* Enter websocket application code here */
	return;
}


/*!
 * 	\brief 						This websocket Event indicates successful handshake with client
 * 								Once this is called the server can start sending data packets over websocket using
 * 								the sl_WebSocketSend API.
 *
 *
 * 	\param[in] uConnection			Websocket Client Id
 *
 * 	\return						none
 */
void WebSocketHandshakeEventHandler(UINT16 uConnection)
{
	g_success = 1;
	g_uConnection = uConnection;
}


#define SW2_GPIO22 22
//static void sw2IntHandler(void)
//{
//  int press_cnt = 0;
//  int cnt;
//  unsigned char pin;
//  unsigned int port;
//
//  GPIO_IF_GetPortNPin(SW2_GPIO22, &port, &pin);
//  for(cnt = 0; cnt < 10; cnt++)
//  {
//    if(GPIO_IF_Get(SW2_GPIO22, port, pin))
//    {
//      press_cnt++;
//      UART_PRINT("sw2 press\r\n");
//      MAP_UtilsDelay(1500000);
//    }
//    else
//    {
//      break;
//    }
//  }
//  if(press_cnt > 8)
//  {
//    UART_PRINT("Device Start Pairing\r\n");
//    GPIO_IF_LedOn(MCU_ALL_LED_IND);
//    system_state = STATE_REGISTRATION;
//    // mqtt_stop();
//  }
//  Button_IF_EnableInterrupt(SW2);
//
//}

//****************************************************************************
//
//! Task function start the device and crete a TCP server showcasing the smart
//! plug
//!
//****************************************************************************
//#include "date_time.h"

void HttpServerAppTask(void * param)
{
	long lRetVal = -1;
    /**< get system information in flash >*/
	UART_PRINT("HttpServerAppTask start\r\n");

    // Check if pair button pressed
//    if(ring_pin_val==1){
//        system_Deregister();
//        system_reboot_1();
//     }
//
//    /**< check if device registered with server >*/
    if(system_IsRegistered() < 0)
    {
      // if not then register with server
      UART_PRINT("xxxxx___UNREGISTER\r\n");
      lRetVal = system_registration();
//      system_init();
//      system_reboot();
    }
    else
    {
      UART_PRINT("xxxxx___REGISTERED\r\n");
//      osi_SyncObjSignal(&g_registered);
    }
//    UART_PRINT("system stop\n");
//    // osi_SyncObjSignal(&g_ConnectedToAP);
//
//end:
//    vTaskDelete(NULL);
    // todo: for sure
    // sl_Stop(200);
    // while (1) {
    //   osi_Sleep(1000*10);
    // }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
