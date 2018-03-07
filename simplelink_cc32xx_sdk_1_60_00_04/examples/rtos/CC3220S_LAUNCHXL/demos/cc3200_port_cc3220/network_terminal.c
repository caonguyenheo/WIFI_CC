/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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

/*****************************************************************************

 Application Name     - Network terminal

 Application Overview - Network Terminal is a command line interface (cli) based application,
 used to demonstrate the CC32XX/CC31XX networking capabilities.
 It does that by offering a list of commands, divided into four silos:

    Wlan        : Contains link layer functions like scan, connect, etc.
    NetApp      : Demonstrates the usage of networking applications.
    Socket      : Shows variety of socket API and responsible for sending and receiving packets.
    Transceiver : Gives the user a direct interface to the NWP radio for RF tests, raw sockets (L1) and more.

 Application Details  - Refer to 'Network Terminal' README.html

*****************************************************************************/

/* Standard includes */
#include <stdlib.h>
#include <stdint.h>

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/wlan.h>
#include "nonos.h"
/* Example/Board Header files */
#include "network_terminal.h"
//#include "cmd_parser.h"
#include "wlan_cmd.h"
#include "cc3200_system.h"
#include "osi.h"


#include "system.h"
#include "server_api.h"
#include "p2p_main.h"
#include "userconfig.h"
#include "uart_term.h"

#include "nonos.h"
//#include "netapp_cmd.h"
//#include "socket_cmd.h"
//#include "transceiver_cmd.h"
//#include "common.h"
/* Application defines */
#define SIX_BYTES_SIZE_MAC_ADDRESS  (17)
#define MAX_SERVICE_NAME_LENGTH      (63)
#define CHANNEL_MASK_ALL            (0x1FFF)
#define RSSI_TH_MAX                 (-95)
OsiSyncObj_t g_userconfig_init;
/****************************************************************************
                      LOCAL FUNCTION PROTOTYPES
****************************************************************************/

int32_t sem_wait_timeout(sem_t *sem, uint32_t Timeout);
int Network_IF_provisionAP();
int Network_IF_Connect2AP_static_ip(char* ssid, char* pass, int retry);
int Network_IF_Connect2AP(char* ssid, char* pass, int retry);
static int Network_IF_GetScanRTResult(SlWlanNetworkEntry_t* netEntries);
extern void HttpServerAppTask(void * param);
typedef enum
{
    /* Choosing this number to avoid overlap with host-driver's error codes.  */
    DEVICE_NOT_IN_STATION_MODE = -0x7F0,
    DEVICE_NOT_IN_AP_MODE = DEVICE_NOT_IN_STATION_MODE - 1,
    DEVICE_NOT_IN_P2P_MODE = DEVICE_NOT_IN_AP_MODE - 1,

    STATUS_CODE_MAX = -0xBB8
} e_NetAppStatusCodes;
char router_list[2048] = {0};
static SlWlanNetworkEntry_t g_NetEntries[SCAN_TABLE_SIZE];
//SlWlanConnStatusParam_t type;
extern unsigned long  g_ulStatus;   /* SimpleLink Status */
//uint32_t            gMaxCmd = (sizeof(gCmdList)/sizeof(cmdAction_t));
pthread_t           gSpawn_thread = (pthread_t)NULL;
pthread_t           gHTTP_thread = (pthread_t)NULL;
int write_xml_string( const char *s, char *dest);
const char * mxmlEntityGetName(int val);
appControlBlock     app_CB;
//int32_t ConfigureSimpleLinkToDefaultState()
//{
//     uint8_t                              ucConfigOpt;
//     uint8_t                              ucPower;
//     int32_t                              RetVal = -1;
//     int32_t                              Mode = -1;
//     uint32_t                             IfBitmap = 0;
//     SlWlanScanParamCommand_t             ScanDefault = {0};
//     SlWlanRxFilterOperationCommandBuff_t RxFilterIdMask = {{0}};
//
//     /* Turn NWP on */
//     Mode = sl_Start(0, 0, 0);
//     ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//
//     if(Mode != ROLE_STA)
//     {
//           /* Set NWP role as STA */
//           Mode = sl_WlanSetMode(ROLE_STA);
//           ASSERT_ON_ERROR(Mode, WLAN_ERROR);
//
//         /* For changes to take affect, we restart the NWP */
//         RetVal = sl_Stop(SL_STOP_TIMEOUT);
//         ASSERT_ON_ERROR(RetVal, DEVICE_ERROR);
//
//         Mode = sl_Start(0, 0, 0);
//         ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//     }
//
//     if(Mode != ROLE_STA)
//     {
//         UART_PRINT("Failed to configure device to it's default state");
//         return -1;
//     }
//
//     /* Set policy to auto only */
//     RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION, SL_WLAN_CONNECTION_POLICY(1,0,0,0), NULL ,0);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Disable Auto Provisioning */
//     RetVal = sl_WlanProvisioning(SL_WLAN_PROVISIONING_CMD_STOP, 0xFF, 0, NULL, 0x0);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Delete existing profiles */
//     RetVal = sl_WlanProfileDel(0xFF);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* enable DHCP client */
//     RetVal = sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP, 0, 0);
//     ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
//
//     /* Disable ipv6 */
//     IfBitmap = !(SL_NETCFG_IF_IPV6_STA_LOCAL | SL_NETCFG_IF_IPV6_STA_GLOBAL);
//     RetVal = sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(IfBitmap),(const unsigned char *)&IfBitmap);
//     ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
//
//     /* Configure scan parameters to default */
//     ScanDefault.ChannelsMask = CHANNEL_MASK_ALL;
//     ScanDefault.RssiThreshold = RSSI_TH_MAX;
//
//     RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_SCAN_PARAMS, sizeof(ScanDefault), (uint8_t *)&ScanDefault);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Disable scans */
//     ucConfigOpt = SL_WLAN_SCAN_POLICY(0, 0);
//     RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN , ucConfigOpt, NULL, 0);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Set TX power lvl to max */
//     ucPower = 0;
//     RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (uint8_t *)&ucPower);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Set NWP Power policy to 'normal' */
//     RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_PM, SL_WLAN_NORMAL_POLICY, NULL, 0);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Unregister mDNS services */
//     RetVal = sl_NetAppMDNSUnRegisterService(0, 0, 0);
//     ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
//
//     /* Remove all 64 RX filters (8*8) */
//     memset(RxFilterIdMask.FilterBitmap , 0xFF, 8);
//
//     RetVal = sl_WlanSet(SL_WLAN_RX_FILTERS_ID, SL_WLAN_RX_FILTER_REMOVE, sizeof(SlWlanRxFilterOperationCommandBuff_t),(uint8_t *)&RxFilterIdMask);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* Set NWP role as STA */
//     RetVal = sl_WlanSetMode(ROLE_STA);
//     ASSERT_ON_ERROR(RetVal, WLAN_ERROR);
//
//     /* For changes to take affect, we restart the NWP */
//     RetVal = sl_Stop(SL_STOP_TIMEOUT);
//     ASSERT_ON_ERROR(RetVal, DEVICE_ERROR);
//
//     Mode = sl_Start(0, 0, 0);
//     ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
//
//     if(ROLE_STA != Mode)
//     {
//         UART_PRINT("Failed to configure device to it's default state");
//         return -1 ;
//     }
//     else
//     {
//         app_CB.Role = ROLE_STA;
//         SET_STATUS_BIT(app_CB.Status, STATUS_BIT_NWP_INIT);
//     }
//
//     return 0;
//}
int32_t ConfigureSimpleLinkToDefaultState()
{
    uint8_t                              ucConfigOpt = 0;
    uint8_t                              ucConfigLen = 0;
    uint8_t                              ucPower     = 0;
    int32_t                              RetVal = -1;
    int32_t                              Mode = -1;
    uint32_t                             IfBitmap = 0;
    SlWlanScanParamCommand_t             ScanDefault = {0};
    SlWlanRxFilterOperationCommandBuff_t RxFilterIdMask = {{0}};
    SlDeviceVersion_t      ver = {{0}};

    Mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
    if(Mode != ROLE_STA)
    {
          /* Set NWP role as STA */
          Mode = sl_WlanSetMode(ROLE_STA);
          ASSERT_ON_ERROR(Mode, DEVICE_ERROR);

        /* For changes to take affect, we restart the NWP */
        RetVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(RetVal, DEVICE_ERROR);

        Mode = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(Mode, DEVICE_ERROR);
    }
    if(Mode != ROLE_STA)
    {
        UART_PRINT("Failed to configure device to it's default state");
        return -1;
    }
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    RetVal = sl_DeviceGet(SL_DEVICE_GENERAL, &ucConfigOpt,
                                &ucConfigLen, (_u8 *)(&ver));
    UART_PRINT("Host Driver Version: %s\r\n", SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\r\n",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.FwVersion[0],ver.FwVersion[1],
    ver.FwVersion[2],ver.FwVersion[3],
    ver.PhyVersion[0],ver.PhyVersion[1],
    ver.PhyVersion[2],ver.PhyVersion[3]);

    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION, SL_WLAN_CONNECTION_POLICY(1,0,0,0), NULL ,0);
//    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,SL_WLAN_CONNECTION_POLICY(1,1,0,1),NULL,0);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    RetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

   RetVal = sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE, SL_NETCFG_ADDR_DHCP , 1, 1);//SL_NETCFG_IPV4_STA_ADDR_MODE
   ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);
    /* Disable ipv6 */
    IfBitmap = !(SL_NETCFG_IF_IPV6_STA_LOCAL | SL_NETCFG_IF_IPV6_STA_GLOBAL);
    RetVal = sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(IfBitmap),(const unsigned char *)&IfBitmap);
    ASSERT_ON_ERROR(RetVal, NETAPP_ERROR);


//    /* Configure scan parameters to default */
//    ScanDefault.ChannelsMask = CHANNEL_MASK_ALL;
//    ScanDefault.RssiThershold = RSSI_TH_MAX;
//
//    RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_SCAN_PARAMS, sizeof(ScanDefault), (uint8_t *)&ScanDefault);
//    ASSERT_ON_ERROR(RetVal);

    // Disable scan
    ucConfigOpt = SL_WLAN_SCAN_POLICY(0, 0);
    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN ,  ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    // Set TX power lvl to max
    ucPower = 0;
    RetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&ucPower);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    // Set NWP Power policy to 'normal'
    RetVal = sl_WlanPolicySet(SL_WLAN_POLICY_PM, SL_WLAN_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    // Unregister mDNS services
    RetVal = sl_NetAppMDNSUnRegisterService(0, 0, 0);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    /* Remove all 64 RX filters (8*8) */
    memset(RxFilterIdMask.FilterBitmap, 0xFF, 8);
    RetVal = sl_WlanSet(SL_WLAN_RX_FILTERS_ID,
    		   SL_WLAN_RX_FILTER_REMOVE,
        	   sizeof(SlWlanRxFilterOperationCommandBuff_t),
    		   (_u8 *)&RxFilterIdMask);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    RetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(RetVal, WLAN_ERROR);

    InitializeAppVariables();
	 return 0;

}
/*!
    \brief          Prints IP address.

    This routine prints IP addresses in a dotted decimal
    notation (IPv4) or colon : notation (IPv6)

    \param          ip         -   Points to command line buffer.

    \param          ipv6       -   Flag that sets if the address is IPv4 or IPv6.

    \return         void

*/
void PrintIPAddress(unsigned char ipv6, void *ip)
{
    uint32_t        *pIPv4;
    uint8_t         *pIPv6;
    int32_t          i=0;

    if(!ip)
    {
        return;
    }

    if(ipv6)
    {
        pIPv6 = (uint8_t*) ip;

        for(i = 0; i < 14; i+=2)
        {
            UART_PRINT("%02x%02x:", pIPv6[i], pIPv6[i+1]);
        }

        UART_PRINT("%02x%02x", pIPv6[i], pIPv6[i+1]);
    }
    else
    {
        pIPv4 = (uint32_t*)ip;
        UART_PRINT("%d.%d.%d.%d", SL_IPV4_BYTE(*pIPv4,3), SL_IPV4_BYTE(*pIPv4,2),
                                  SL_IPV4_BYTE(*pIPv4,1), SL_IPV4_BYTE(*pIPv4,0));
    }
    return;
}

/*!
    \brief          ipv6 Address Parse

    This routine converts string representing IPv6 address
    in a colon notation, into an array of bytes,
    representing each IP address octate.

    \param          str         -   Points to string address.

    \param          ipv6ip      -   Points to a 16 byte long array.

    \return         Upon successful completion, the function shall return 0.
                    In case of failure, this function would return -1.

*/
int32_t ipv6AddressParse(char *str, uint8_t *ipv6ip)
{
    int32_t         i;
    int32_t         l;
    int32_t         zeroCompressPos;
    uint8_t        *t;
    uint8_t         tmp[16];
    uint16_t        value;
    uint8_t         hexDigit;

    i = 0;
    t = (uint8_t*)str;
    value = 0;
    hexDigit=0;
    zeroCompressPos=-1;
    memset(tmp, 0, sizeof(tmp));

    if(*t==':')
    {
        if(*++t!=':')
        {
            return -1;
        }
    }

    while(*t && (i < 16))
    {
        if(*t >= '0' && *t <= '9')
        {
            value = (value << 4) | (*t - '0');
            hexDigit = 1;
        }
        else if(*t >= 'a' && *t <= 'f')
        {
            value = (value << 4) | ((*t - 'a') + 10);
            hexDigit = 1;
        }
        else if(*t >= 'A' && *t <= 'F')
        {
            value = (value << 4) | ((*t - 'A') + 10);
            hexDigit = 1;
        }
        else if((*t == ':') && (i < 14))
        {
            if(hexDigit)
            {
                tmp[i++] = (value >> 8) & 0xFF;
                tmp[i++] = (value) & 0xFF;
                hexDigit = 0;
                value = 0;
            }
            else
            {
                if(zeroCompressPos < 0)
                {
                    zeroCompressPos = i;
                }
                else
                {
                    return -1;
                }
            }
        }
        t++;
    }

    if(i > 15)    return -1;
    else if(hexDigit && (zeroCompressPos < 0) && (i < 14))    return -1;
    else if((!hexDigit) && (zeroCompressPos < 0))    return -1;
    else if((!hexDigit) && (zeroCompressPos != i))    return -1;
    else if((zeroCompressPos >= 0) && i >= 14)    return -1;

    if((hexDigit) && (i < 15))
    {
        tmp[i++] = (value >> 8) & 0xFF;
        tmp[i++] = (value) & 0xFF;
        hexDigit = 0;
        value = 0;
    }

    if(zeroCompressPos>=0)
    {
        i--;
        l = 15;
        while(i>=zeroCompressPos)
        {
            if ((l >= 0) && (i >= 0))
            {
                tmp[l] = tmp[i];
                tmp[i] = 0;
                l--;
                i--;
            }
        }
    }

    memcpy(ipv6ip, tmp, sizeof(tmp));

    return 0;
}

/*!
    \brief          ipv4 Address Parse

    This routine converts string representing IPv4 address
    in a dotted decimal notation, into an array of bytes,
    representing each IP address octate.

    \param          str         -   Points to string address.

    \param          ipv4ip      -   Points to a 4 byte long array.

    \return         Upon successful completion, the function shall return 0.
                    In case of failure, this function would return -1.

*/
int32_t ipv4AddressParse(char *str, uint32_t *ipv4ip)
{
    volatile int32_t i = 0;
    uint32_t         n;
    uint32_t         ipv4Address = 0;
    char             *token;

    token = strtok(str, ".");
    if (token)
    {
        n = (int)strtoul(token, 0, 10);
    }
    else
    {
        return -1;
    }

    while(i < 4)
    {
       /* Check Whether IP is valid */
       if((token != NULL) && (n < 256))
       {
           ipv4Address |= n;
           if(i < 3)
           {
               ipv4Address = ipv4Address << 8;
           }
           token=strtok(NULL,".");
           if (token)
           {
               n = (int)strtoul(token, 0, 10);
           }
           i++;
       }
       else
       {
           return -1;
       }
    }

    *ipv4ip = ipv4Address;

    return 0;
}

/*!
    \brief          hex byte string to ASCII

    This routine converts a hexadecimal base byte represented
    as a string, into the equivalent ASCII character.

    \param          str         -   Points to string Hex.

    \param          ascii       -   Points to a buffer containing the converted ASCII char.

    \return         Upon successful completion, the function shall return 0.
                    In case of failure, this function would return -1.

*/
int32_t hexbyteStrtoASCII(char *str, uint8_t *ascii)
{
    char        *t = NULL;
    uint8_t     hexchar = 0;


    /* Skip 0x if present */
    if(str[1] == 'x')
        t = str + 2;
    else
        t = str;

    while((hexchar < 2) && ((*t) != '\0'))
    {
        if(*t >= '0' && *t <= '9')
        {
            *ascii = (*ascii << (hexchar * 4)) | (*t -'0');
        }
        else if(*t >= 'a' && *t <= 'f')
        {
            *ascii = (*ascii << (hexchar * 4)) | ((*t - 'a') + 10);
        }
        else if(*t >= 'A' && *t <= 'F')
        {
            *ascii = (*ascii << (hexchar * 4)) | ((*t - 'A') + 10);
        }
        else
        {
            /* invalid entry */
            return -1;
        }
        hexchar++;
        t++;
    }

    if(hexchar == 0)
        return -1;

    return 0;
}

/*!
    \brief          Parse MAC address.

    This routine converts a MAC address given in a colon separated
    format in a string form, and converts it to six byte number format,
    representing the MAC address.

    \param          str       -   Points to string Hex.

    \param          mac       -   Points to a buffer containing the converted ASCII char.

    \return         Upon successful completion, the function shall return 0.
                    In case of failure, this function would return -1.

*/
int32_t macAddressParse(char *str, uint8_t *mac)
{

    int32_t        count = 0;
    char           *t = NULL;
    uint8_t        tmp[3];
    uint8_t        byte = 0;
    size_t         MAC_length;

    t = (char*)str;

    MAC_length = strlen(t);

    if (MAC_length > SIX_BYTES_SIZE_MAC_ADDRESS)
    {
        /* invalid MAC size */
        return -1;
    }

    memset(tmp, 0, sizeof(tmp));

    while(byte < 6)
    {
        count  = 0;
        while(*t != ':' && count < 2)
        {
            tmp[count] = *t;
            count++;
            t++;
        }

        tmp[count] = 0;

        if(hexbyteStrtoASCII((char*)&tmp[0], &mac[byte]) < 0)
            return -1;

        byte++;

        if(*t != ':' && byte < 6)
        {
            /* invalid entry */
            return -1;
        }

        /* Skip ':' */
        t++;
    }

    return 0;
}

int32_t sem_wait_timeout(sem_t *sem, uint32_t Timeout)
{
    struct timespec abstime;
    abstime.tv_nsec = 0 ;
    abstime.tv_sec = 0 ;

    /* Since POSIX timeout are relative and not absolute,
     * take the current timestamp. */
    clock_gettime(CLOCK_REALTIME, &abstime);
    if ( abstime.tv_nsec < 0 )
    {
        abstime.tv_sec = Timeout;
        return (sem_timedwait(sem , &abstime));
    }

    /* Add the amount of time to wait */
    abstime.tv_sec += Timeout / 1000 ;
    abstime.tv_nsec += (Timeout % 1000)*1000000;

    abstime.tv_sec += (abstime.tv_nsec / 1000000000);
    abstime.tv_nsec = abstime.tv_nsec % 1000000000;

    /* Call the semaphore wait API */
    return sem_timedwait(sem , &abstime);
}

/*!
    \brief          Main thread

    This routine starts the application:
    It initializes the SPI interface with the NWP.
    Starts the GPIO drivers and UART console.
    Sets the Start time for the realtime clock.
    It starts the driver's internal Spawn thread (to receive asynchronous events).
    and launch the command prompt handler.

    \param          arg

    \return         This function doesn't return.

    \sa             sl_Task, Board_initSPI, InitTerm, cmd_prompt.

*/



int Network_IF_WifiSetMode(int iMode)
{
  int lRetVal = -1;
  char str[10] = "EU";
  lRetVal = sl_Start(NULL, NULL, NULL);
  // desire mode duplicate with previous mode
  if(lRetVal == iMode)
  {
    UART_PRINT("Current mode available\n\r");
    return 0;
  }

  sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, SL_WLAN_GENERAL_PARAM_OPT_COUNTRY_CODE, 2, str);
  /**< set device in AP mode >*/
  if(ROLE_AP == iMode)
  {
    UART_PRINT("Switching to AP mode on application request\r\n");
    // Switch to AP role and restart
    lRetVal = sl_WlanSetMode(iMode);
    UART_PRINT("lRetVal:%d\r\n",lRetVal);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    hl_ap_config();
    /**< end config ssid name > */
    // restart
    lRetVal = sl_Stop(0xFF);
    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);
    lRetVal = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    // Check if the device is up in AP Mode
    if (ROLE_AP == lRetVal)
    {
      // If the device is in AP mode, we need to wait for this event
      // before doing anything
      while(!IS_IP_ACQUIRED(g_ulStatus))
      {
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsHandleSpawnTask();
#else
        osi_Sleep(1);
#endif
      }
    }
    else
    {
      // We don't want to proceed if the device is not coming up in AP-mode
    	ASSERT_ON_ERROR(lRetVal,DEVICE_NOT_IN_AP_MODE);

    }
    UART_PRINT("Re-started SimpleLink Device: AP Mode\n\r");

  }

  /**< set device in STA mode >*/
  else if(ROLE_STA == iMode)
  {
    UART_PRINT("Switching to STA mode on application request\r\n");
    // Switch to AP role and restart
    lRetVal = sl_WlanSetMode(iMode);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    // restart
    lRetVal = sl_Stop(0xFF);
    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);
    lRetVal = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    // Check if the device is in station again
    if (ROLE_STA != lRetVal)
    {
      // We don't want to proceed if the device is not coming up in STA-mode
    	ASSERT_ON_ERROR(lRetVal,DEVICE_NOT_IN_STATION_MODE);
    }
    UART_PRINT("Re-started SimpleLink Device: STA Mode\r\n");
  }

  /**< set device in P2P mode >*/
  else if(ROLE_P2P == iMode)
  {
    UART_PRINT("Switching to P2P mode on application request\r\n");
    // Switch to AP role and restart
    lRetVal = sl_WlanSetMode(iMode);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    // restart
    lRetVal = sl_Stop(0xFF);
    // reset status bits
    CLR_STATUS_BIT_ALL(g_ulStatus);
    lRetVal = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lRetVal,DEVICE_ERROR);
    // Check if the device is in station again
    if (ROLE_P2P != lRetVal)
    {
      // We don't want to proceed if the device is not coming up in P2P-mode
    	ASSERT_ON_ERROR(lRetVal,DEVICE_NOT_IN_P2P_MODE);
    }
    UART_PRINT("Re-started SimpleLink Device: P2P Mode\r\n");
  }

  else
  {
    UART_PRINT("Invalid Wifi mode\r\n");
    return -1;
  }
  return 0;
}
const char *				/* O - Entity name or NULL */
mxmlEntityGetName(int val)		/* I - Character value */
{
  switch (val)
  {
    case '&' :
        return ("amp");

    case '<' :
        return ("lt");

    case '>' :
        return ("gt");

    case '\"' :
        return ("quot");
	case '\'':
		return ("apos");
    default :
        return (NULL);
  }
}
int				/* O - 0 on success, -1 on failure */
write_xml_string(
    const char      *s,			/* I - String to write */
    char            *dest)			/* I - Write pointer */
{
  const char	*name;			/* Entity name, if any */
  char* d = dest;
    if(s == NULL || dest == NULL)
        return -1;
  while (*s != 0)
  {
    if ((name = mxmlEntityGetName(*s)) != NULL)
    {
      *d++ = '&';

      while (*name)
      {
		*d++ = *name;
        name ++;
      }

	  *d++ = ';';
    }
    else
	{
	   *d++ = *s;
	}
    s ++;
  }
	*d=0;
  return (0);
}
static int
Network_IF_GetScanRTResult(SlWlanNetworkEntry_t* netEntries)
{
  unsigned char   policyOpt;
  unsigned long IntervalVal = 10;
  int lRetVal;

  policyOpt = SL_WLAN_CONNECTION_POLICY(0, 0, 0, 0);
  lRetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION, policyOpt, NULL, 0);
  if(lRetVal < 0)
  {
    UART_PRINT("set connect policy failed\n");
    return lRetVal;
  }
  // enable scan
  policyOpt = SL_WLAN_SCAN_POLICY(1,0);

  // set scan policy - this starts the scan
  lRetVal = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN, policyOpt,
                             (unsigned char *)(IntervalVal), sizeof(IntervalVal));
  if(lRetVal < 0)
  {
    UART_PRINT("set scan policy failed\n");
    return lRetVal;
  }

// extern long ConfigureSimpleLinkToDefaultState();
// extern void InitializeAppVariables();

  // delay 1 second to verify scan is started
  ClockP_sleep(2);

  // lRetVal indicates the valid number of entries
  // The scan results are occupied in netEntries[]
  lRetVal = sl_WlanGetNetworkList(0, SCAN_TABLE_SIZE, netEntries);

  if(lRetVal < 0)
  {
    UART_PRINT("get ap list failed\n");
    return lRetVal;
  }
  // Disable scan
  policyOpt = SL_WLAN_SCAN_POLICY(0,0);

  // set scan policy - this stops the scan
  sl_WlanPolicySet(SL_WLAN_POLICY_SCAN , policyOpt,
                   (unsigned char *)(IntervalVal), sizeof(IntervalVal));
  if(lRetVal < 0)
  {
    UART_PRINT("disable scan policy failed\n");
    return lRetVal;
  }

  return lRetVal;

}
int
Network_IF_provisionAP()
{
	UART_PRINT("<<------ List of Access Points (AP) ------>>\r\n");
  long lCountSSID;
  long ssid_cnt,i,j;
  unsigned char temp_var;
  char mac_address[32] = "\0";
  //  static char router_list_resp[1024] = {0};
  char router_list_data[2048] = {0};
  int l_scantotalnum=0;

  char router_info_fm[200] = "<w>\n" \
    "<s>\"%s\"</s>\n" \
      "<b>%s</b>\n" \
        "<a>%s</a>\n" \
          "<q>%d</q>\n" \
            "<si>%d</si>\n" \
              "<nl>%d</nl>\n" \
                "<ch>%d</ch>\n" \
                  "</w>\n";

  char xml_header[100] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"\
    "<wl v=\"2.0\">\n"\
      "<n>%d</n>\n";
//  while(!g_ucProvisioningDone)
  for(j=0;j<5;j++){//j loop for number of rescan
  {
    #if 1
    //Get Scan Result
    lCountSSID = Network_IF_GetScanRTResult(&g_NetEntries[0]);

    if(lCountSSID < 0)
    {
      return lCountSSID;
    }
    #else
    lCountSSID = 0;
    #endif
//    UART_PRINT("<<------ List of Access Points (%d AP) ------>>\n", lCountSSID);
//    UART_PRINT("number of AP: %d\n", lCountSSID);
//    for(ssid_cnt = 0; ssid_cnt < lCountSSID; ssid_cnt++)
//    {
//      UART_PRINT("- AP ssid: %s\n", g_NetEntries[ssid_cnt].Ssid);
//    }
    // assing header file for response data

    #if 1
    for(ssid_cnt = 0; ssid_cnt < lCountSSID; ssid_cnt++)
    {
      for(i = 0; i < 6; i++)
      {
        temp_var = g_NetEntries[ssid_cnt].Bssid[i];
        //            UART_PRINT("%x ", temp_var);
        if(temp_var<0x0f)
        {
          sprintf(mac_address + strlen(mac_address), "0%x", temp_var);
        }else
        {
          sprintf(mac_address + strlen(mac_address), "%x", temp_var);
        }
        if(i<5)
        {
          sprintf(mac_address + strlen(mac_address), ":");
        }
      }
      char raw_ssid[32];
      char encode_ssid[32];
      memset(raw_ssid, 0x00, sizeof(raw_ssid));
      memset(encode_ssid, 0x00, sizeof(encode_ssid));
      sprintf(raw_ssid, "%.*s", g_NetEntries[ssid_cnt].SsidLen, g_NetEntries[ssid_cnt].Ssid);
      write_xml_string(raw_ssid, encode_ssid);
      if ((l_scantotalnum>=SCAN_TABLE_SIZE) || (strstr(router_list_data,encode_ssid)!=NULL)){
          memset(mac_address, 0, sizeof(mac_address));
          continue;
      }
      UART_PRINT("ssid %d,%d: %s\r\n", ssid_cnt, l_scantotalnum, encode_ssid);
      // assign format for rounter info
      sprintf(router_list_data + strlen(router_list_data), router_info_fm,
              encode_ssid,
              mac_address,
              (g_NetEntries[ssid_cnt].SecurityInfo== 0)? "Open":
              (g_NetEntries[ssid_cnt].SecurityInfo== 1)? "WEP":
               (g_NetEntries[ssid_cnt].SecurityInfo == 2)? "WPA":
                    "WPA2",
                    ((g_NetEntries[ssid_cnt].Rssi+255)*100/255),
                    ((g_NetEntries[ssid_cnt].Rssi+255)*100/255),
                    0,
                    0
                    );
      memset(mac_address, 0, sizeof(mac_address));
      l_scantotalnum++;
    }
    #endif

    }//end for j
    memset(router_list, 0, sizeof(router_list));
    sprintf(router_list, xml_header, l_scantotalnum);
    memcpy(router_list + strlen(router_list), router_list_data, strlen(router_list_data));
    memcpy(router_list + strlen(router_list), "</wl>", strlen("</wl>"));
  }
}
int Network_IF_Connect2AP(char* ssid, char* pass, int retry)
{
	SlWlanSecParams_t secParams = {0};
	long lRetVal = -1;
	int i,j;
	//
	// Connect to the Access Point
	//
	while((i<2)&&(lRetVal<0)){
		secParams.Key = (signed char*)pass;
		secParams.KeyLen = strlen(pass);
		secParams.Type = sec_string_to_num();
		lRetVal = Network_IF_ConnectAP(ssid,secParams);
		UART_PRINT("Connection to AP:%s, pass:%s, sec:%d, ret %d\r\n", ssid, secParams.Key, secParams.Type, lRetVal);
		i++;
	}
	return lRetVal;
}
int Network_IF_Connect2AP_static_ip(char* ssid, char* pass, int retry)
{
    long lRetVal = 0;
    if (ssid == NULL || pass == NULL)
    {
//        g_input_state=LED_STATE_NONE;
        return -1;
    }

//    sl_WlanProfileDel(0xFF);
//    ConfigureSimpleLinkToDefaultState();
    InitializeAppVariables();
    lRetVal = Network_IF_WifiSetMode(ROLE_STA);
    if (lRetVal < 0) {
        return lRetVal;
    }

    lRetVal = Network_IF_Connect2AP(ssid, pass, 0);
    if (lRetVal < 0) {
        return lRetVal;
    }
    // Set connect policy
    // Auto + SmartConfig + fast connect
    lRetVal = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
    		                    SL_WLAN_CONNECTION_POLICY(1, 1, 0, 0), NULL, 0);
    if (lRetVal < 0) {
        return lRetVal;
    }
    /*
    * Save connection profile
    */
    lRetVal = sl_WlanProfileDel(0xFF);
    if (lRetVal < 0) {
        return lRetVal;
    }

    // Set nearly max priority
    // Initialize AP security params
    SlWlanSecParams_t SecurityParams = {0}; // AP Security Parameters
    SecurityParams.Key = (signed char*)pass;
    SecurityParams.KeyLen = strlen(pass);
    // FIXME: DkS
    // SecurityParams.Type = SECURITY_TYPE;
    //SecurityParams.Type = SL_SEC_TYPE_WPA_WPA2;
    SecurityParams.Type = sec_string_to_num();
    lRetVal = sl_WlanProfileAdd((signed char*)ssid,
                      strlen(ssid), 0, &SecurityParams, 0, 6, 0);
    if (lRetVal < 0) {
        return lRetVal;
    }

    #if (USING_STATIC_IP)
    /*
    * Set static ip
    */
    // Get current ip
    uint8_t len = sizeof(SlNetCfgIpV4Args_t);
    uint8_t dhcpIsOn = 0;
    SlNetCfgIpV4Args_t ipV4 = {0};
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO,&dhcpIsOn,&len,(_u8 *)&ipV4);

    UART_PRINT("DHCP is %s IP %d.%d.%d.%d MASK %d.%d.%d.%d GW %d.%d.%d.%d DNS %d.%d.%d.%d\n",
            (dhcpIsOn > 0) ? "ON" : "OFF",
            SL_IPV4_BYTE(ipV4.ipV4,3),SL_IPV4_BYTE(ipV4.ipV4,2),SL_IPV4_BYTE(ipV4.ipV4,1),SL_IPV4_BYTE(ipV4.ipV4,0),
            SL_IPV4_BYTE(ipV4.ipV4Mask,3),SL_IPV4_BYTE(ipV4.ipV4Mask,2),SL_IPV4_BYTE(ipV4.ipV4Mask,1),SL_IPV4_BYTE(ipV4.ipV4Mask,0),
            SL_IPV4_BYTE(ipV4.ipV4Gateway,3),SL_IPV4_BYTE(ipV4.ipV4Gateway,2),SL_IPV4_BYTE(ipV4.ipV4Gateway,1),SL_IPV4_BYTE(ipV4.ipV4Gateway,0),
            SL_IPV4_BYTE(ipV4.ipV4DnsServer,3),SL_IPV4_BYTE(ipV4.ipV4DnsServer,2),SL_IPV4_BYTE(ipV4.ipV4DnsServer,1),SL_IPV4_BYTE(ipV4.ipV4DnsServer,0));

    // Change to STATIC
    sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE,IPCONFIG_MODE_ENABLE_IPV4,sizeof(SlNetCfgIpV4Args_t),(_u8 *)&ipV4);
    ASSERT_ON_ERROR(lRetVal);
  #endif
    return lRetVal;
}
void *mainThread(void *arg)
{
    int32_t             RetVal ;
    int32_t             RetVal1 ;
    pthread_attr_t      pAttrs_spawn;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    struct timespec     ts = {0};

    /* Initializes the SPI interface to the Network Processor and peripheral SPI (if defined in the board file) */
    Board_initSPI();
    Board_initGPIO();

    /* Init Application variables */
//    RetVal = initAppVariables();

    /* Init Terminal UART */
    InitTerm();

    /* initilize the realtime clock */
    clock_settime(CLOCK_REALTIME, &ts);

    /* Switch off all LEDs on boards */
    GPIO_write(Board_LED0, Board_LED_OFF);
    GPIO_write(Board_LED1, Board_LED_OFF);
    GPIO_write(Board_LED2, Board_LED_OFF);

    /* Create the sl_Task internal spawn thread */
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = 9;   //SPAWN_TASK_PRIORITY
    RetVal = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_spawn, 2*TASK_STACK_SIZE);
    /* The SimpleLink host driver architecture mandate spawn thread to be created prior to calling Sl_start (turning the NWP on). */
    /* The purpose of this thread is to handle asynchronous events sent from the NWP.
     * Every event is classified and later handled by the Host driver event handlers. */



    RetVal = pthread_create(&gSpawn_thread, &pAttrs_spawn, sl_Task, NULL);
    if(RetVal < 0)
    {
        /* Handle Error */
         UART_PRINT("Network Terminal - Unable to create spawn thread\r\n");
         return(NULL);
    }
    /* Before turning on the NWP on, reset any previously configured parameters */
    /*
         IMPORTANT NOTE - This is an example reset function, user must update this function
                          to match the application settings.
    */
#if 1
//    ClockP_sleep(1);
	pthread_attr_init(&pAttrs);
	priParam.sched_priority = 1;
	RetVal = pthread_attr_setschedparam(&pAttrs, &priParam);
	RetVal |= pthread_attr_setstacksize(&pAttrs, 8*TASK_STACK_SIZE);

	if(RetVal)
	{
        /* Handle Error */
        UART_PRINT("Unable to configure http thread parameters\r\n");
        while(1);
	}

	RetVal = pthread_create(&gHTTP_thread, &pAttrs, HttpServerAppTask, NULL);
	if(RetVal)
	{
        /* Handle Error */
        UART_PRINT("Unable to create HttpServerAppTask thread\r\n");
        while(1);
	}
#endif

#if 0
    UART_PRINT("CC3200 version=%s\r\n", SYSTEM_VERSION);
	sl_Start(0,0,0);
	ClockP_sleep(2);
    userconfig_init();
    userconfig_flash_read();
    get_url_default();
    ClockP_sleep(2);
//    osi_SyncObjSignal(&g_userconfig_init);
    sl_WlanProfileDel(0xFF);
//    RetVal = ConfigureSimpleLinkToDefaultState();
//    if(RetVal < 0)
//    {
//        /* Handle Error */
//        UART_PRINT("Network Terminal - Couldn't configure Network Processor\n");
//        return(NULL);
//    }
    InitializeAppVariables();
//    Network_IF_InitDriver(ROLE_STA);
    Network_IF_WifiSetMode(ROLE_STA);
    UART_PRINT("\nNetwork init done\r\n");
    Network_IF_provisionAP();
    UART_PRINT("Scan wifi done\r\n");

    RetVal = sl_Stop(0xFF);
    RetVal = Network_IF_WifiSetMode(ROLE_AP);
    if (RetVal < 0)
    {
        return -51;
    }

    RetVal = sl_NetAppStop(SL_NETAPP_HTTP_SERVER_ID);
    if (RetVal < 0)
    {
    	 return -52;
    }
    //Run APPS HTTP Server
    get_url_default();
    get_cam_default();
    HttpServerInitAndRun(NULL);
    myCinApiRegister_t mycinRegister;
    UART_PRINT("User API key: %s\r\n", apiKey);
    int api_key_len = strlen(apiKey);
    if (api_key_len > MAX_USER_TOKEN)
    {
        api_key_len = MAX_USER_TOKEN - 1;
    }
    memset(g_mycinUser.user_token, 0x00, MAX_USER_TOKEN);
    strncpy(g_mycinUser.user_token, apiKey, api_key_len);
    if (myCinRegister(&g_mycinUser, &mycinRegister) >= 0)
    {
           //UART_PRINT("RegPostdone: device_id: %s, device_token %s\r\n", mycinRegister.registration_id,g_mycinUser.authen_token);
           // RESET VARIABLE
//           memset(ps_ip, 0x00, P2P_PS_IP_LEN);
//           ps_port = 0;
//           memset(p2p_key_char, 0x00, P2P_KEY_CHAR_LEN);
//           memset(p2p_rn_char, 0x00, P2P_RN_CHAR_LEN);

//           while (p2p_init(1)<0){
//               osi_Sleep(5000);
    }
#endif
    return(0);
}

