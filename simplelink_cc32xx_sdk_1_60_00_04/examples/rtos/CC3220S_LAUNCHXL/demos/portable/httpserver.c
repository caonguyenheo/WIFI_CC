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


//*****************************************************************************
//
//! \addtogroup out_of_box
//! @{
//
//*****************************************************************************


/* standard includes */
#include <stdlib.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#ifdef CC32XX
/* driverlib Header files */
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/driverlib/rom.h>
#include <ti/devices/cc32xx/driverlib/rom_map.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#endif
/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Example/Board Header files */
#include <Board.h>
#include <uart_term.h>
#include <httpserver.h>

#define NETAPP_MAX_RX_FRAGMENT_LEN      SL_NETAPP_REQUEST_MAX_DATA_LEN
#define NETAPP_MAX_METADATA_LEN         (100)
#define NETAPP_MAX_ARGV_TO_CALLBACK     SL_FS_MAX_FILE_NAME_LENGTH+50
#define NUMBER_OF_URI_SERVICES          (3)

#define LOOP_FOREVER() \
            {\
                while(1); \
            }
/* check the error code and handle it */
#define ASSERT_ON_ERROR(error_code)\
            {\
                 if(error_code < 0) \
                   {\
                        ERR_PRINT(error_code);\
                        return error_code;\
                 }\
            }

#define APPLICATION_NAME        "Portable + Wi-Fi HTTP Server"
#define APPLICATION_VERSION     "1.00"

/* Temperature written by the temperature thread and read by Http server thread */
extern float temperatureC;
extern float temperatureF;

/* Mutex to protect the reading/writing of the float temperature */
extern pthread_mutex_t temperatureMutex;

extern int sprintf(char *str, const char *format, ...);

/****************************************************************************
                      LOCAL FUNCTION PROTOTYPES
****************************************************************************/

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to AP
//!           - Configures connection policy to Auto
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!          - Disable IPV6
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//!           IMPORTANT NOTE - This is an example reset function, user must
//!           update this function to match the application settings.
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static int32_t ConfigureSimpleLinkToDefaultState();

 //*****************************************************************************
 //
 //! \brief This function configures the HTTP server
 //!
 //! \param[in]  none
 //!
 //! \return NetApp error codes or 0 upon success.
 //!
 //*****************************************************************************
 static int32_t ConfigureHttpServer();

//*****************************************************************************
//
//! \brief This function prepare error netapp response in case memory could not be allocated
//!
//! \param[in]  pNetAppResponse	netapp response structure
//!
//! \return none
//!
//****************************************************************************
void NetAppRequestErrorResponse(SlNetAppResponse_t *pNetAppResponse);

//*****************************************************************************
//
//! \brief this function composes an element type from metadata/payload (TLV structure)
//!
//! \param[in]  isAnswer            states whether this is a value or a parameter
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  elementVal          value of element
//!
//! \return element type
//!
//****************************************************************************
uint16_t setElementType(uint8_t isValue, uint8_t requestIdx, uint8_t elementVal);

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP GET requests
//!
//! \param[in] parsingStatus        validity of HTTP GET request
//!
//! \param[in] contentLen           content length in respond to  HTTP GET request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t prepareGetMetadata(int32_t parsingStatus, uint32_t contentLen, HttpContentTypeList contentTypeId);

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP POST/PUT requests
//!
//! \param[in] parsingStatus		validity of HTTP POST/PUT request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t preparePostMetadata(int32_t parsingStatus);

//*****************************************************************************
//
//! \brief This function defines the HTTP request characteristics and callbacks
//!
//! \param[in]  None
//!
//! \return 0 on success or -ve on error
//!
//****************************************************************************
void initHttpserverDB();

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx         request index to indicate the message
//!
//! \param[in]  pPhrase            pointer to HTTP metadata payload
//!
//! \param[in]  payloadLen         HTTP metadata or payload length
//!
//! \param[out]  argcCallback      count of input params to the service callback
//!
//! \param[out]  argvCallback      set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseUrlEncoded(uint8_t requestIdx, uint8_t * pPhrase, uint16_t phraseLen, uint8_t *argcCallback, uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function maps header type to its string value
//!
//! \param[in]  httpHeaderType        http header type
//!
//! \param[out]  httpHeaderText       http header text
//!
//! \return none
//!
//****************************************************************************
void convertHeaderType2Text (uint8_t httpHeaderType, uint8_t **httpHeaderText);

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the metadata
//!
//! \param[in]  requestType          HTTP method (GET, POST, PUT or DEL)
//!
//! \param[in]  pMetadata            pointer to HTTP metadata
//!
//! \param[in]  metadataLen          HTTP metadata length
//!
//! \param[out]  requestIdx          request index to indicate the message
//!
//! \param[out]  argcCallback        count of input params to the service callback
//!
//! \param[out]  argvCallback        set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestMetadata(uint8_t requestType, uint8_t * pMetadata, uint16_t metadataLen, uint8_t *requestIdx, uint8_t *argcCallback, uint8_t **argvCallback);

//*****************************************************************************
//
//! \brief This function checks that the content requested via HTTP message exists
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \param[out]  requestIdx         request index to indicate the message
//!
//! \param[out]  argcCallback       count of input params to the service callback
//!
//! \param[out]  argvCallback       set of input params to the service callback
//!
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t httpCheckContentInDB(SlNetAppRequest_t *netAppRequest, uint8_t *requestIdx, uint8_t *argcCallback, uint8_t **argvCallback);

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void DisplayBanner(char * AppName, char * AppVer);

//*****************************************************************************
//
//! \brief This function parse and execute HTTP GET requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpGetHandler(SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This function parse and execute HTTP POST/PUT requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpPostHandler(SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP POST
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightPostCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a sensors service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t tempGetCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest);

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightGetCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest);


/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/

const uint8_t pageNotFound[] = "<html>404 - Sorry page not found</html>";

/* metadata and content buffer are allocated static as these are shared in all use cases        */
/* however, it is possible to set those dynamically as well                                */
uint8_t     gMetadataBuffer[NETAPP_MAX_METADATA_LEN];
uint8_t     gPayloadBuffer[NETAPP_MAX_RX_FRAGMENT_LEN];

uint8_t     gHttpPostBuffer[NETAPP_MAX_ARGV_TO_CALLBACK];
uint8_t     gHttpGetBuffer[NETAPP_MAX_ARGV_TO_CALLBACK];

int8_t      xVal, yVal, zVal;
float       temperatureVal;
I2C_Handle  i2cHandle;

App_CB App_ControlBlock;

/* message queue for http messages between server and client */
mqd_t httpserverMQueue;
pthread_mutex_t sensorLockObj;    /* Lock Object for sensor readings */

http_RequestObj_t httpRequest[NUMBER_OF_URI_SERVICES] =
{
	/*  TODO: add httpRequests  */
		 
};

http_headerFieldType_t g_HeaderFields [] =
{
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_VERSION, WEB_SERVER_VERSION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI, WEB_SERVER_REQUEST_URI},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING, WEB_SERVER_QUERY_STRING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE, WEB_SERVER_HEADER_CONTENT_TYPE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN, WEB_SERVER_HEADER_CONTENT_LEN},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_LOCATION, WEB_SERVER_HEADER_LOCATION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SERVER, WEB_SERVER_HEADER_SERVER},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_USER_AGENT, WEB_SERVER_HEADER_USER_AGENT},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_COOKIE, WEB_SERVER_HEADER_COOKIE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_SET_COOKIE, WEB_SERVER_HEADER_SET_COOKIE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_UPGRADE, WEB_SERVER_HEADER_UPGRADE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REFERER, WEB_SERVER_HEADER_REFERER},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ACCEPT, WEB_SERVER_HEADER_ACCEPT},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_ENCODING, WEB_SERVER_HEADER_CONTENT_ENCODING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_DISPOSITION, WEB_SERVER_HEADER_CONTENT_DISPOSITION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONNECTION, WEB_SERVER_HEADER_CONNECTION},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_ETAG, WEB_SERVER_HEADER_ETAG},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_DATE, WEB_SERVER_HEADER_DATE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_HEADER_HOST, WEB_SERVER_HEADER_HOST},
    {SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_ENCODING, WEB_SERVER_HEADER_ACCEPT_ENCODING},
    {SL_NETAPP_REQUEST_METADATA_TYPE_ACCEPT_LANGUAGE, WEB_SERVER_HEADER_ACCEPT_LANGUAGE},
    {SL_NETAPP_REQUEST_METADATA_TYPE_CONTENT_LANGUAGE, WEB_SERVER_HEADER_CONTENT_LANGUAGE}
};

http_contentTypeMapping_t g_ContentTypes [] =
{
    {HttpContentTypeList_TextHtml, TEXT_HTML, TEXT_HTML_MIME},
    {HttpContentTypeList_TextCSS, TEXT_CSS, TEXT_CSS_MIME},
    {HttpContentTypeList_TextXML, TEXT_XML, TEXT_XML_MIME},
    {HttpContentTypeList_ApplicationJson, APPLICATION_JSON, APPLICATION_JSON_MIME},
    {HttpContentTypeList_ImagePNG, IMAGE_PNG, IMAGE_PNG_MIME},
    {HttpContentTypeList_ImageGIF, IMAGE_GIF, IMAGE_GIF_MIME},
    {HttpContentTypeList_TextPlain, TEXT_PLAIN, TEXT_PLAIN_MIME},
    {HttpContentTypeList_TextCSV, TEXT_CSV, TEXT_CSV_MIME},
    {HttpContentTypeList_ApplicationJavascript, APPLICATION_JAVASCRIPT, APPLICATION_JAVASCRIPT_MIME},
    {HttpContentTypeList_ImageJPEG, IMAGE_JPEG, IMAGE_JPEG_MIME},
    {HttpContentTypeList_ApplicationPDF, APPLICATION_PDF, APPLICATION_PDF_MIME},
    {HttpContentTypeList_ApplicationZIP, APPLICATION_ZIP, APPLICATION_ZIP_MIME},
    {HttpContentTypeList_ShokewaveFlash, SHOCKWAVE_FLASH, SHOCKWAVE_FLASH_MIME},
    {HttpContentTypeList_AudioXAAC, AUDIO_X_AAC, AUDIO_X_AAC_MIME},
    {HttpContentTypeList_ImageXIcon, IMAGE_X_ICON, IMAGE_X_ICON_MIME},
    {HttpContentTypeList_TextVcard, TEXT_VCARD, TEXT_VCARD_MIME},
    {HttpContentTypeList_ApplicationOctecStream, APPLICATION_OCTEC_STREAM, APPLICATION_OCTEC_STREAM_MIME},
    {HttpContentTypeList_VideoAVI, VIDEO_AVI, VIDEO_AVI_MIME},
    {HttpContentTypeList_VideoMPEG, VIDEO_MPEG, VIDEO_MPEG_MIME},
    {HttpContentTypeList_VideoMP4, VIDEO_MP4, VIDEO_MP4_MIME},
    {HttpContentTypeList_UrlEncoded, FORM_URLENCODED, URL_ENCODED_MIME}
};


/*****************************************************************************
                  Callback Functions
*****************************************************************************/

//*****************************************************************************
//
//! The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Id)
    {
        case SL_WLAN_EVENT_CONNECT:
        {
            SET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Connection);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

            /*
             Information about the connected AP (like name, MAC etc) will be
             available in 'slWlanConnectAsyncResponse_t'-Applications
             can use it if required:

              slWlanConnectAsyncResponse_t *pEventData = NULL;
              pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            */

            /* Copy new connection SSID and BSSID to global parameters */
            memcpy(App_ControlBlock.connectionSSID,
                    pWlanEvent->Data.Connect.SsidName,
                    pWlanEvent->Data.Connect.SsidLen);

            App_ControlBlock.ssidLen = pWlanEvent->Data.Connect.SsidLen;

            memcpy(App_ControlBlock.connectionBSSID,
                    pWlanEvent->Data.Connect.Bssid,
                    SL_WLAN_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s ,"
                        "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                            App_ControlBlock.connectionSSID,
                            App_ControlBlock.connectionBSSID[0], App_ControlBlock.connectionBSSID[1],
                            App_ControlBlock.connectionBSSID[2], App_ControlBlock.connectionBSSID[3],
                            App_ControlBlock.connectionBSSID[4], App_ControlBlock.connectionBSSID[5]);
        }
        break;

        case SL_WLAN_EVENT_DISCONNECT:
        {
            SlWlanEventDisconnect_t*    pEventData = NULL;

            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Connection);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_IpAcquired);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6gAcquired);

            pEventData = &pWlanEvent->Data.Disconnect;

            /*  If the user has initiated 'Disconnect' request,
                'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED.
             */
            if(SL_WLAN_DISCONNECT_USER_INITIATED == pEventData->ReasonCode)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the "
                            "AP: %s, BSSID: %x:%x:%x:%x:%x:%x "
                            "on application's request \n\r",
                            App_ControlBlock.connectionSSID,
                            App_ControlBlock.connectionBSSID[0], App_ControlBlock.connectionBSSID[1],
                            App_ControlBlock.connectionBSSID[2], App_ControlBlock.connectionBSSID[3],
                            App_ControlBlock.connectionBSSID[4], App_ControlBlock.connectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
                            "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                            App_ControlBlock.connectionSSID,
                            App_ControlBlock.connectionBSSID[0], App_ControlBlock.connectionBSSID[1],
                            App_ControlBlock.connectionBSSID[2], App_ControlBlock.connectionBSSID[3],
                            App_ControlBlock.connectionBSSID[4], App_ControlBlock.connectionBSSID[5]);
            }
            memset(App_ControlBlock.connectionSSID, 0, sizeof(App_ControlBlock.connectionSSID));
            memset(App_ControlBlock.connectionBSSID, 0, sizeof(App_ControlBlock.connectionBSSID));
        }
        break;

     case SL_WLAN_EVENT_STA_ADDED:
        {
            UART_PRINT("[WLAN EVENT] External Station connected to SimpleLink AP\r\n");

            UART_PRINT("[WLAN EVENT] STA BSSID: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                                   pWlanEvent->Data.STAAdded.Mac[0],
                                   pWlanEvent->Data.STAAdded.Mac[1],
                                   pWlanEvent->Data.STAAdded.Mac[2],
                                   pWlanEvent->Data.STAAdded.Mac[3],
                                   pWlanEvent->Data.STAAdded.Mac[4],
                                   pWlanEvent->Data.STAAdded.Mac[5]);

        }
        break;

        case SL_WLAN_EVENT_STA_REMOVED:
        {
            UART_PRINT("[WLAN EVENT] External Station disconnected from SimpleLink AP\r\n");
        }
        break;

        default:
        {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                                                            pWlanEvent->Id);
        }
        break;
    }
}

//*****************************************************************************
//
//! The Function Handles the Fatal errors
//!
//! \param[in]  slFatalErrorEvent - Pointer to Fatal Error Event info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkFatalErrorEventHandler(SlDeviceFatal_t *slFatalErrorEvent)
{
    //uint8_t msg = 4;
    //int32_t msgqRetVal;

    switch (slFatalErrorEvent->Id)
    {
        case SL_DEVICE_EVENT_FATAL_DEVICE_ABORT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Abort NWP event detected: "
                        "AbortType=%d, AbortData=0x%x\n\r",
                        slFatalErrorEvent->Data.DeviceAssert.Code,
                        slFatalErrorEvent->Data.DeviceAssert.Value);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_DRIVER_ABORT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Driver Abort detected. \n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_NO_CMD_ACK:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: No Cmd Ack detected "
                       "[cmd opcode = 0x%x] \n\r",
                       slFatalErrorEvent->Data.NoCmdAck.Code);
        }
        break;

        case SL_DEVICE_EVENT_FATAL_SYNC_LOSS:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Sync loss detected n\r");
        }
        break;

        case SL_DEVICE_EVENT_FATAL_CMD_TIMEOUT:
        {
            UART_PRINT("[ERROR] - FATAL ERROR: Async event timeout detected "
                       "[event opcode =0x%x]  \n\r",
                       slFatalErrorEvent->Data.CmdTimeout.Code);
        }
        break;

        default:
            UART_PRINT("[ERROR] - FATAL ERROR: Unspecified error detected \n\r");
            break;
    }

//    msgqRetVal = mq_send(controlMQueue, (char *)&msg, 1, 0);
//    if(msgqRetVal < 0)
//    {
//        UART_PRINT("[Control task] could not send element to msg queue\n\r");
//        while (1);
//    }
}

//*****************************************************************************
//
//! This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    SlNetAppEventData_u *pNetAppEventData = NULL;

    if (NULL == pNetAppEvent)
    {
        return;
    }

    pNetAppEventData = &pNetAppEvent->Data;

     switch(pNetAppEvent->Id)
    {
        case SL_NETAPP_EVENT_IPV4_ACQUIRED:
        {
            SlIpV4AcquiredAsync_t   *pEventData = NULL;

            SET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_IpAcquired);

            /* Ip Acquired Event Data */
            pEventData = &pNetAppEvent->Data.IpAcquiredV4;

            /* Gateway IP address */
            App_ControlBlock.gatewayIP = pEventData->Gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                        "Gateway=%d.%d.%d.%d\n\r",
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Ip,0),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,3),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,2),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,1),
                        SL_IPV4_BYTE(pNetAppEvent->Data.IpAcquiredV4.Gateway,0));
        }
        break;

        case SL_NETAPP_EVENT_IPV6_ACQUIRED:
        {
            if(!GET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6lAcquired))
            {
                SET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6lAcquired);
                UART_PRINT("[NETAPP EVENT] Local IPv6 Acquired\n\r");
            }
            else
            {
                SET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_Ipv6gAcquired);
                UART_PRINT("[NETAPP EVENT] Global IPv6 Acquired\n\r");
            }
        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_LEASED:
        {
            SET_STATUS_BIT(App_ControlBlock.status, AppStatusBits_IpLeased);

            UART_PRINT("[NETAPP EVENT] IPv4 leased %d.%d.%d.%d for device %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,3),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,2),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,1),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpLeased.IpAddress,0),\
                pNetAppEventData->IpLeased.Mac[0],\
                pNetAppEventData->IpLeased.Mac[1],\
                pNetAppEventData->IpLeased.Mac[2],\
                pNetAppEventData->IpLeased.Mac[3],\
                pNetAppEventData->IpLeased.Mac[4],\
                pNetAppEventData->IpLeased.Mac[5]);

        }
        break;

        case SL_NETAPP_EVENT_DHCPV4_RELEASED:
        {
            CLR_STATUS_BIT(App_ControlBlock.status, AppStatusBits_IpLeased);

            UART_PRINT("[NETAPP EVENT] IPv4 released %d.%d.%d.%d for device %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,3),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,2),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,1),\
                (uint8_t)SL_IPV4_BYTE(pNetAppEventData->IpReleased.IpAddress,0),\
                pNetAppEventData->IpReleased.Mac[0],\
                pNetAppEventData->IpReleased.Mac[1],\
                pNetAppEventData->IpReleased.Mac[2],\
                pNetAppEventData->IpReleased.Mac[3],\
                pNetAppEventData->IpReleased.Mac[4],\
                pNetAppEventData->IpReleased.Mac[5]);

            UART_PRINT("Reason: ");
            switch(pNetAppEventData->IpReleased.Reason)
            {
                case SL_IP_LEASE_PEER_RELEASE: UART_PRINT("Peer released\n\r");
                break;

                case SL_IP_LEASE_PEER_DECLINE: UART_PRINT("Peer declined\n\r");
                break;

                case SL_IP_LEASE_EXPIRED: UART_PRINT("Lease expired\n\r");
                break;
            }
        }
        break;

        case SL_NETAPP_EVENT_DHCP_IPV4_ACQUIRE_TIMEOUT:
        {
            UART_PRINT("[NETAPP EVENT] DHCP IPv4 Acquire timeout\n\r");
        }
        break;

        default:
        {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Id);
        }
        break;
    }
}

//*****************************************************************************
//
//! This function handles HTTP server events
//!
//! \param[in]  pServerEvent    - Contains the relevant event information
//! \param[in]  pServerResponse - Should be filled by the user with the
//!                               relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerEventHandler(SlNetAppHttpServerEvent_t *pHttpEvent,
                                SlNetAppHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application  - legacy event handler */
    UART_PRINT("[HTTP SERVER EVENT] Unexpected HTTP server event \n\r");
}

//*****************************************************************************
//
//! This function handles General Events
//!
//! \param[in]  pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    //uint8_t msg = 4;
    //int32_t msgqRetVal;

    /*
        Most of the general errors are not FATAL are are to be handled
        appropriately by the application.
    */
    if(NULL == pDevEvent) return;
    switch(pDevEvent->Id)
    {
        case SL_DEVICE_EVENT_RESET_REQUEST:
        {
            UART_PRINT("[GENERAL EVENT] Reset Request Event\r\n");
        }
        break;

        default:
        {
            UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
                                                pDevEvent->Data.Error.Code,
                                                pDevEvent->Data.Error.Source);
        }
        break;
    }
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]  pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(SL_SOCKET_ASYNC_EVENT == pSock->Event)
    {
        UART_PRINT("[SOCK ERROR] an event received on socket %d\r\n",pSock->SocketAsyncEvent.SockAsyncData.Sd);
        switch(pSock->SocketAsyncEvent.SockAsyncData.Type)
        {
        case SL_SSL_NOTIFICATION_CONNECTED_SECURED:
        UART_PRINT("[SOCK ERROR] SSL handshake done");
            break;
        case SL_SSL_NOTIFICATION_HANDSHAKE_FAILED:
        UART_PRINT("[SOCK ERROR] SSL handshake failed with error %d\r\n", pSock->SocketAsyncEvent.SockAsyncData.Val);
            break;
        case SL_SSL_ACCEPT:
            UART_PRINT("[SOCK ERROR] Recoverable error occurred during the handshake %d\r\n",pSock->SocketAsyncEvent.SockAsyncData.Val);
            break;
        case SL_OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED:
            UART_PRINT("[SOCK ERROR] Other peer terminated the SSL layer.\r\n");
            break;
        case SL_SSL_NOTIFICATION_WRONG_ROOT_CA:
            UART_PRINT("[SOCK ERROR] Used wrong CA to verify the peer.\r\n");

            break;
        default:
            break;
        }
    }

    /* This application doesn't work w/ socket - Events are not expected */
     switch( pSock->Event )
     {
       case SL_SOCKET_TX_FAILED_EVENT:
           switch( pSock->SocketAsyncEvent.SockTxFailData.Status)
           {
              case SL_ERROR_BSD_ECLOSE:
                   UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\r",
                                pSock->SocketAsyncEvent.SockTxFailData.Sd);
               break;
               default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , "
                                "reason (%d) \n\n",
                                pSock->SocketAsyncEvent.SockTxFailData.Sd,
                                pSock->SocketAsyncEvent.SockTxFailData.Status);
               break;
           }
           break;

       default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
            break;
     }
}

void SimpleLinkNetAppRequestMemFreeEventHandler (uint8_t *buffer)
{
    /* Unused in this application */
}

//*****************************************************************************
//
//! \brief This function is registered as netapp request callback
//!
//! \param[in]  pNetAppRequest        netapp request structure
//!
//! \param[out]  pNetAppResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
void SimpleLinkNetAppRequestEventHandler(SlNetAppRequest_t *pNetAppRequest, SlNetAppResponse_t *pNetAppResponse)
{
    SlNetAppRequest_t *netAppRequest;
    int32_t msgqRetVal;

    DEBUG_PRINT("[Http server task] NetApp Request Received - AppId = %d, Type = %d, Handle = %d\n\r", pNetAppRequest->AppId, pNetAppRequest->Type, pNetAppRequest->Handle);

    if ((pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET) || (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_DELETE) ||
        (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST) || (pNetAppRequest->Type == SL_NETAPP_REQUEST_HTTP_PUT))
    {
        /* Prepare pending response */
        pNetAppResponse->Status = SL_NETAPP_RESPONSE_PENDING;
        pNetAppResponse->ResponseData.pMetadata = NULL;
        pNetAppResponse->ResponseData.MetadataLen = 0;
        pNetAppResponse->ResponseData.pPayload = NULL;
        pNetAppResponse->ResponseData.PayloadLen = 0;
        pNetAppResponse->ResponseData.Flags = 0;
    }
    else
    {
        NetAppRequestErrorResponse(pNetAppResponse);

        return;
    }

    netAppRequest = (SlNetAppRequest_t *) malloc (sizeof(SlNetAppRequest_t));
    if (NULL == netAppRequest)
    {
        NetAppRequestErrorResponse(pNetAppResponse);

        return;
    }

    netAppRequest->AppId = pNetAppRequest->AppId;
    netAppRequest->Type = pNetAppRequest->Type;
    netAppRequest->Handle = pNetAppRequest->Handle;
    netAppRequest->requestData.Flags = pNetAppRequest->requestData.Flags;

    /* Copy Metadata */
    if (pNetAppRequest->requestData.MetadataLen > 0)
    {
        netAppRequest->requestData.pMetadata = (uint8_t *) malloc (pNetAppRequest->requestData.MetadataLen);
        if (NULL == netAppRequest->requestData.pMetadata)
        {
            NetAppRequestErrorResponse(pNetAppResponse);

            return;
        }
        sl_Memcpy(netAppRequest->requestData.pMetadata, pNetAppRequest->requestData.pMetadata, pNetAppRequest->requestData.MetadataLen);
        netAppRequest->requestData.MetadataLen = pNetAppRequest->requestData.MetadataLen;
    }
    else
    {
        netAppRequest->requestData.MetadataLen = 0;
    }

    /* Copy the payload */
    if (pNetAppRequest->requestData.PayloadLen > 0)
    {
        netAppRequest->requestData.pPayload = (uint8_t *) malloc (pNetAppRequest->requestData.PayloadLen);
        if (NULL == netAppRequest->requestData.pPayload)
        {
            NetAppRequestErrorResponse(pNetAppResponse);

            return;
        }
        sl_Memcpy (netAppRequest->requestData.pPayload, pNetAppRequest->requestData.pPayload, pNetAppRequest->requestData.PayloadLen);
        netAppRequest->requestData.PayloadLen = pNetAppRequest->requestData.PayloadLen;
    }
    else
    {
        netAppRequest->requestData.PayloadLen = 0;
    }

    msgqRetVal = mq_send(httpserverMQueue, (char *)&netAppRequest, 1, 0);
    if(msgqRetVal < 0)
    {
        UART_PRINT("[Http server task] could not send element to msg queue\n\r");
        while (1);
    }
}


//*****************************************************************************
//                 Local Functions
//*****************************************************************************

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to AP
//!           - Configures connection policy to Auto
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disable IPV6
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//!           IMPORTANT NOTE - This is an example reset function, user must
//!           update this function to match the application settings.
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static int32_t ConfigureSimpleLinkToDefaultState()
{
    SlWlanRxFilterOperationCommandBuff_t  RxFilterIdMask;

    uint8_t     ucConfigOpt = 0;
    uint16_t    ifBitmap  = 0;
    uint8_t     ucPower = 0;

    int32_t     ret = -1;
    int32_t     mode = -1;

    memset(&RxFilterIdMask,0,sizeof(SlWlanRxFilterOperationCommandBuff_t));

    /* Start Simplelink - Blocking mode */
     mode = sl_Start(0, 0, 0);
    if (SL_ERROR_CALIB_FAIL == mode)    /* when calibration fails, reboot is required */
    {
        sl_Stop(SL_STOP_TIMEOUT);
    }
    else
    {
        if (SL_RET_CODE_DEV_ALREADY_STARTED != mode)
        {
            ASSERT_ON_ERROR(mode);
        }
    }

    /* If the device is not in AP mode, try configuring it in AP mode
     in case device is already started (got SL_RET_CODE_DEV_ALREADY_STARTED error code), then mode would remain -1 and in this case we do not know the role. Move to AP role anyway    */
    if (ROLE_AP != mode)
    {

        /* Switch to AP role and restart */
        ret = sl_WlanSetMode(ROLE_AP);
        ASSERT_ON_ERROR(ret);

        ret = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(ret);

        ret = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(ret);

        /* Check if the device is in AP again */
        if (ROLE_AP != ret)
        {
            /* We don't want to proceed if the device is not coming up in AP mode */
            return -1;
        }
    }

    /* Set connection policy to Auto (no AutoProvisioning)  */
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_CONNECTION,
                                SL_WLAN_CONNECTION_POLICY(1, 0, 0, 0), NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Remove all profiles */
    ret = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(ret);

    /* Enable DHCP client */
    ret = sl_NetCfgSet(SL_NETCFG_IPV4_STA_ADDR_MODE,SL_NETCFG_ADDR_DHCP,0,0);
    ASSERT_ON_ERROR(ret);

    /* Disable IPV6 */
    ifBitmap = 0;
    ret = sl_NetCfgSet(SL_NETCFG_IF, SL_NETCFG_IF_STATE, sizeof(ifBitmap), (uint8_t *)&ifBitmap);
    ASSERT_ON_ERROR(ret);

    /* Disable scan */
    ucConfigOpt = SL_WLAN_SCAN_POLICY(0, 0);
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Set Tx power level for station mode
     Number between 0-15, as dB offset from max power - 0 will set max power */
    ucPower = 0;
    ret = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
                        SL_WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (uint8_t *)&ucPower);
    ASSERT_ON_ERROR(ret);

    /* Set PM policy to normal */
    ret = sl_WlanPolicySet(SL_WLAN_POLICY_PM , SL_WLAN_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(ret);

    /* Unregister mDNS services */
    ret = sl_NetAppMDNSUnRegisterService(0, 0, 0);
    ASSERT_ON_ERROR(ret);

    /* Re-activate HTTP server as secured/non-secured */
    ret = ConfigureHttpServer();
    ASSERT_ON_ERROR(ret);

    /* Remove  all 64 filters (8*8) */
    memset(RxFilterIdMask.FilterBitmap, 0xFF, 8);
    ret = sl_WlanSet(SL_WLAN_RX_FILTERS_ID,
               SL_WLAN_RX_FILTER_REMOVE,
               sizeof(SlWlanRxFilterOperationCommandBuff_t),
               (uint8_t *)&RxFilterIdMask);
    ASSERT_ON_ERROR(ret);

    ret = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(ret);

    return ret;
}

//*****************************************************************************
//
//! \brief This function configures the HTTP server
//!
//! \param[in]  none
//!
//! \return NetApp error codes or 0 upon success.
//!
//*****************************************************************************
static int32_t ConfigureHttpServer()
{
    int32_t retVal = 0;

    retVal = sl_NetAppStop(SL_NETAPP_HTTP_SERVER_ID);
    UART_PRINT("[Http server task] HTTP Server Stopped\n\r");
    if(retVal >= 0)
        {
            retVal = sl_NetAppStart(SL_NETAPP_HTTP_SERVER_ID);
            UART_PRINT("[Http server task] HTTP Server Re-started\n\r");
        }
    return retVal;
}

//*****************************************************************************
//
//! \brief This function prepare error netapp response in case memory could not be allocated
//!
//! \param[in]  pNetAppResponse    netapp response structure
//!
//! \return none
//!
//****************************************************************************
void NetAppRequestErrorResponse(SlNetAppResponse_t *pNetAppResponse)
{
    UART_PRINT("[Http server task] could not allocate memory for netapp request\n\r");

    /* Prepare error response */
    pNetAppResponse->Status = SL_NETAPP_RESPONSE_NONE;
    pNetAppResponse->ResponseData.pMetadata = NULL;
    pNetAppResponse->ResponseData.MetadataLen = 0;
    pNetAppResponse->ResponseData.pPayload = NULL;
    pNetAppResponse->ResponseData.PayloadLen = 0;
    pNetAppResponse->ResponseData.Flags = 0;
}

//*****************************************************************************
//
//! \brief this function composes an element type from metadata/payload (TLV structure)
//!
//! \param[in]  isAnswer            states whether this is a value or a parameter
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  elementVal          value of element
//!
//! \return element type
//!
//****************************************************************************
uint16_t setElementType(uint8_t isValue, uint8_t requestIdx, uint8_t elementVal)
{
    uint16_t elementType;

    elementType = elementVal;
    elementType |= (((isValue<<7) | (requestIdx & 0x7F)) << 8);

    return elementType;
}

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP GET requests
//!
//! \param[in] parsingStatus        validity of HTTP GET request
//!
//! \param[in] contentLen           content length in respond to  HTTP GET request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t prepareGetMetadata(int32_t parsingStatus, uint32_t contentLen, HttpContentTypeList contentTypeId)
{
    char *contentType;
    uint8_t *pMetadata;
    uint16_t metadataLen;

    contentType = g_ContentTypes[contentTypeId].contentTypeText;

    pMetadata = gMetadataBuffer;

    /* http status */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_STATUS;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 2;
    pMetadata+=2;

    if (parsingStatus < 0)
    {
        *(uint16_t *)pMetadata = (uint16_t) SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND;
    }
    else
    {
        *(uint16_t *)pMetadata = (uint16_t) SL_NETAPP_HTTP_RESPONSE_200_OK;
    }

    pMetadata+=2;

    /* Content type */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_TYPE;
    pMetadata++;
     (*(uint16_t *)pMetadata) = (uint16_t) strlen ((const char *)contentType);
    pMetadata+=2;
    sl_Memcpy (pMetadata, contentType, strlen((const char *)contentType));
    pMetadata+=strlen((const char *)contentType);

    /* Content len */
    *pMetadata = SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 4;
    pMetadata+=2;
    *(uint32_t *)pMetadata = (uint32_t) contentLen;

    metadataLen = 5 + 7 + strlen ((const char *)contentType) + 3;

    return metadataLen;
}

//*****************************************************************************
//
//! \brief This function prepares metadata for HTTP POST/PUT requests
//!
//! \param[in] parsingStatus        validity of HTTP POST/PUT request
//!
//! \return metadataLen
//!
//****************************************************************************
uint16_t preparePostMetadata(int32_t parsingStatus)
{
    uint8_t *pMetadata;
    uint16_t metadataLen;

    pMetadata = gMetadataBuffer;

    /* http status */
    *pMetadata = (uint8_t) SL_NETAPP_REQUEST_METADATA_TYPE_STATUS;
    pMetadata++;
    *(uint16_t *)pMetadata = (uint16_t) 2;
    pMetadata+=2;

    if (parsingStatus < 0)
    {
        *(uint16_t *)pMetadata = (uint16_t) SL_NETAPP_HTTP_RESPONSE_404_NOT_FOUND;
    }
    else
    {
        *(uint16_t *)pMetadata = (uint16_t) SL_NETAPP_HTTP_RESPONSE_204_OK_NO_CONTENT;        /* no need for content so browser stays on the same page */
    }

    pMetadata+=2;

    metadataLen = 5;

    return metadataLen;
}

//*****************************************************************************
//
//! \brief This function defines the HTTP request characteristics and callbacks
//!
//! \param[in]  None
//!
//! \return 0 on success or -ve on error
//!
//****************************************************************************
void initHttpserverDB()
{
    /*  TODO: add httpRequest characteristics and service callbacks */

}

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the payload
//!
//! \param[in]  requestIdx         request index to indicate the message
//!
//! \param[in]  pPhrase            pointer to HTTP metadata payload
//!
//! \param[in]  payloadLen         HTTP metadata or payload length
//!
//! \param[out]  argcCallback      count of input params to the service callback
//!
//! \param[out]  argvCallback      set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseUrlEncoded(uint8_t requestIdx, uint8_t * pPhrase, uint16_t phraseLen, uint8_t *argcCallback, uint8_t **argvCallback)
{
    uint8_t *token;
    uint8_t characteristic, value, isValueExpected, loopIdx;
    int32_t status;
    uint8_t *argvArray;
    uint8_t remainingLen, actualLen;
    uint16_t elementType;

    argvArray = *argvCallback;

    if (*argcCallback > 0)        /* it means parameters already exist - fast forward to the end of argv */
    {
        loopIdx = *argcCallback;
        while (loopIdx > 0)
        {
            argvArray += ARGV_LEN_OFFSET;        /* skip the type */
            argvArray += *argvArray;    /* add the length */
            argvArray++;                /* skip the length */

            loopIdx--;
        }
    }

    /* check if values are expected per characteristic     */
    /* it is a 2 steps procedure:                        */
    /*     1) check that '=' sign exists                    */
    /*     2) check that the value is not NULL                */
    /* if not, return                                    */
    isValueExpected = 0;
    token = (uint8_t *)strchr((char *)pPhrase, '=');
    if (token != NULL)
    {
        if ( (*(token + 1) == '&') || (*(token + 1) ==  '\0') )    /* it means no value supplied */
        {
            return -1;
        }
        else
        {
            isValueExpected = 1;
        }
    }

    /* Parse payload list */
    token = (uint8_t *)strtok((char *)pPhrase, "=&");

    if (NULL == token)        /* it means there is no url encoded data */
    {
        return 0;
    }
        
    while(token && ((pPhrase + phraseLen) > token))
    {
        status = -1;
        characteristic = 0;

        /* run over all possible characteristics, if exist */
        while (httpRequest[requestIdx].charValues[characteristic].characteristic != NULL)
        {
            if(!strncmp((const char *)token, (const char *)httpRequest[requestIdx].charValues[characteristic].characteristic, strlen((const char *)httpRequest[requestIdx].charValues[characteristic].characteristic)))
            {
                status = 0;

                /* found a characteristic. save its index number */
                (*argcCallback)++;
                elementType = setElementType(0, requestIdx, characteristic);
                sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType, ARGV_LEN_OFFSET);
                argvArray += ARGV_LEN_OFFSET;
                *argvArray++ = 1;        /* length field */
                *argvArray++ = characteristic;
                remainingLen = (uint8_t)(phraseLen - (uint8_t)(token - pPhrase) - strlen((const char *)token) - 1);    /* remaining length is for cases where the last value is of string type */
                
                UART_PRINT ("[Http server task] characteristic is: %s\n\r", (int8_t *)httpRequest[requestIdx].charValues[characteristic].characteristic);
                
                break;
            }
            else
            {
                characteristic++;
            }
        }

        if (-1 == status)        /* it means the characteristics is not valid/known */
        {
            return status;
        }

        token = (uint8_t *)strtok(NULL, "=&");

        if (isValueExpected)
        {
            status = -1;
            value = 0;

            if (NULL == httpRequest[requestIdx].charValues[characteristic].value[value])        /* it means any value is OK */
            {
                status = 0;

                /* found a string value. copy its content */
                (*argcCallback)++;
                elementType = setElementType(1, requestIdx, value);
                sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType, ARGV_LEN_OFFSET);
                argvArray += ARGV_LEN_OFFSET;
                if (strlen((const char *)token) > remainingLen)
                    actualLen = remainingLen;
                else
                    actualLen = strlen((const char *)token);

                *argvArray++ = (actualLen + 1);
                sl_Memcpy(argvArray, token, actualLen);
                argvArray += actualLen;
                *argvArray++ =  '\0';

                UART_PRINT ("[Http server task] value is: %s\n\r", (int8_t *)(argvArray - actualLen - 1));
            }
            else
            {

                /* run over all possible values, if exist */
                while (httpRequest[requestIdx].charValues[characteristic].value[value] != NULL)
                {
                    if(!strncmp((const char *)token, (const char *)httpRequest[requestIdx].charValues[characteristic].value[value], strlen((const char *)httpRequest[requestIdx].charValues[characteristic].value[value])))
                    {
                        status = 0;

                        /* found a value. save its index number */
                        (*argcCallback)++;
                        elementType = setElementType(1, requestIdx, value);
                        sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType, ARGV_LEN_OFFSET);
                        argvArray += ARGV_LEN_OFFSET;
                        *argvArray++ = 1;            /* length field */
                        *argvArray++ = value;
                
                        UART_PRINT ("[Http server task] value is: %s\n\r", (int8_t *)httpRequest[requestIdx].charValues[characteristic].value[value]);
                        
                        break;
                    }
                    else
                    {
                        value++;
                    }
                }

                if (-1 == status)        /* it means the value is not valid/known */
                {
                    return status;
                }
            }

            token = (uint8_t *)strtok(NULL, (const char *)"=&");
        }
    }

    return status;
}

//*****************************************************************************
//
//! \brief This function maps header type to its string value
//!
//! \param[in]  httpHeaderType        http header type
//!
//! \param[out]  httpHeaderText       http header text
//!
//! \return none
//!
//****************************************************************************
void convertHeaderType2Text (uint8_t httpHeaderType, uint8_t **httpHeaderText)
{
    int i;
    *httpHeaderText = NULL;

    for (i = 0; i < sizeof (g_HeaderFields)/sizeof(http_headerFieldType_t); i++)
    {
        if (g_HeaderFields[i].headerType == httpHeaderType)
        {
            *httpHeaderText = (uint8_t *)(g_HeaderFields[i].headerText);
            break;
        }
    }
}

//*****************************************************************************
//
//! \brief This function scan netapp request and parse the metadata
//!
//! \param[in]  requestType          HTTP method (GET, POST, PUT or DEL)
//!
//! \param[in]  pMetadata            pointer to HTTP metadata
//!
//! \param[in]  metadataLen          HTTP metadata length
//!
//! \param[out]  requestIdx          request index to indicate the message
//!
//! \param[out]  argcCallback        count of input params to the service callback
//!
//! \param[out]  argvCallback        set of input params to the service callback
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t parseHttpRequestMetadata(uint8_t requestType, uint8_t * pMetadata, uint16_t metadataLen, uint8_t *requestIdx, uint8_t *argcCallback, uint8_t **argvCallback)
{
    uint8_t *pTlv;
    uint8_t *pEnd;

    int32_t status = -1;
    uint8_t    loopIdx;
    uint8_t type;
    uint16_t len;
    uint32_t value;
    uint8_t *typeText;
    uint8_t    nullTerminator;
    uint8_t *argvArray;
    uint16_t elementType;

    argvArray = *argvCallback;

    *requestIdx = 0xFF;
    pTlv = pMetadata;
    pEnd = pMetadata + metadataLen;

    if (metadataLen < 3)
    {
        UART_PRINT("[Http server task] Metadata parsing error\n\r");

        return -1;
    }

    DEBUG_PRINT("[Http server task] Metadata:\n\r");

    while (pTlv < pEnd)
    {
        type = *pTlv;
        pTlv++;
        len = *(uint16_t *)pTlv;
        pTlv+=2;

        convertHeaderType2Text(type, &typeText);

        if (typeText != NULL)
        {
            DEBUG_PRINT("[Http server task] %s \n\r", typeText);
        }

        switch (type)
        {
            case SL_NETAPP_REQUEST_METADATA_TYPE_STATUS:
                /* there are browsers that seem to send many 0 type for no reason */
                /* in this case, do not print anything */
                break;

            case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_CONTENT_LEN:
                if (0 == status)    /* it means there is a content length and URI is OK. Add it to the argv */
                {
                    if (*argcCallback > 0)        /* it means parameters already exist from query type */
                    {
                        loopIdx = *argcCallback;
                        while (loopIdx > 0)
                        {
                            argvArray += ARGV_LEN_OFFSET;        /* skip the type */
                            argvArray += *argvArray;    /* add the length */
                            argvArray++;                /* skip the length */

                            loopIdx--;
                        }
                    }

                    (*argcCallback)++;
                    elementType = setElementType(1, *requestIdx, CONTENT_LEN_TYPE);    /* add content type */
                    sl_Memcpy ((uint8_t*)argvArray, (uint8_t*)&elementType, ARGV_LEN_OFFSET);
                    argvArray += ARGV_LEN_OFFSET;
                    *argvArray++ = len;    /* add content length */
                    sl_Memcpy ((uint8_t*)argvArray, pTlv, len);
                    sl_Memcpy ((uint8_t*)&value, pTlv, len);

                    DEBUG_PRINT("%d\n\r", (uint32_t)value);
                }

                break;

            case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_REQUEST_URI:
                *argcCallback = 0;    /* this is the 1st stop in every http method. zero out the character counter argument */

                for (loopIdx = 0; loopIdx < NUMBER_OF_URI_SERVICES; loopIdx++)
                {
                    if ((strncmp((const char *)pTlv, (const char *)httpRequest[loopIdx].service, strlen((const char *)httpRequest[loopIdx].service))) == 0)
                    {
                        if (requestType == httpRequest[loopIdx].httpMethod)
                        {
                            status = 0;
                            *requestIdx = httpRequest[loopIdx].requestIdx;
                            DEBUG_PRINT("%s\n\r", httpRequest[loopIdx].service);

                            break;
                        }
                    }
                }

                if (status != 0)
                {
                    UART_PRINT("Unknown service\n\r");
                }

                break;

            case SL_NETAPP_REQUEST_METADATA_TYPE_HTTP_QUERY_STRING:
                if (0 == status)
                {
                    status = parseUrlEncoded(*requestIdx, pTlv, len, argcCallback, argvCallback);

                    if (status != 0)
                    {
                        UART_PRINT("Query string in metadata section is not valid/known\n\r");
                    }
                }

                break;
                
            default:
                nullTerminator = *(pTlv + len);
                *(pTlv + len) =  '\0';
                DEBUG_PRINT("%s\n\r", pTlv);
                *(pTlv + len) = nullTerminator;

                break;
        }   
        pTlv+=len;
    }

    return status;
}

//*****************************************************************************
//
//! \brief This function checks that the content requested via HTTP message exists
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \param[out]  requestIdx         request index to indicate the message
//!
//! \param[out]  argcCallback       count of input params to the service callback
//!
//! \param[out]  argvCallback       set of input params to the service callback
//!
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t httpCheckContentInDB(SlNetAppRequest_t *netAppRequest, uint8_t *requestIdx, uint8_t *argcCallback, uint8_t **argvCallback)
{
    int32_t status = -1;

    if (netAppRequest->AppId != SL_NETAPP_HTTP_SERVER_ID)
    {
        return status;
    }

    status = parseHttpRequestMetadata(netAppRequest->Type, netAppRequest->requestData.pMetadata, netAppRequest->requestData.MetadataLen, requestIdx, argcCallback, argvCallback);

    if((*requestIdx) == 2)
    {
        status = 0 ;
    }

    if ((0 == status) && (netAppRequest->requestData.PayloadLen != 0) && (netAppRequest->Type != SL_NETAPP_REQUEST_HTTP_PUT))   /* PUT does not contain parseable data - only POST does */
    {
        status = parseUrlEncoded(*requestIdx, netAppRequest->requestData.pPayload, netAppRequest->requestData.PayloadLen, argcCallback, argvCallback);
        if (status != 0)
        {
            UART_PRINT("[Http server task] query string in payload section is not valid/known\n\r");
        }
    }

    return status;
}

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void DisplayBanner(char * AppName, char * AppVer)
{

    UART_PRINT("\n\n\n\r");
    UART_PRINT("\t\t =================================================\n\r");
    UART_PRINT("\t\t   %s Example Ver. %s      \n\r", AppName, AppVer);
    UART_PRINT("\t\t =================================================\n\r");
    UART_PRINT("\n\n\n\r");
}

//*****************************************************************************
//
//! \brief This function parse and execute HTTP GET requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpGetHandler(SlNetAppRequest_t *netAppRequest)
{
    uint16_t metadataLen;
    int32_t status;
    uint8_t requestIdx;

    uint8_t    argcCallback;
    uint8_t     *argvArray;
    uint8_t     **argvCallback = &argvArray;

    argvArray = gHttpGetBuffer;

    status = httpCheckContentInDB(netAppRequest, &requestIdx, &argcCallback, argvCallback);

    if (status < 0)
    {
        metadataLen = prepareGetMetadata(status, strlen ((const char *)pageNotFound), HttpContentTypeList_TextHtml);

        sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer, (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION | SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
        DEBUG_PRINT("[Http server task] Metadata Sent, len = %d \n\r", metadataLen);

        sl_NetAppSend (netAppRequest->Handle, strlen ((const char *)pageNotFound), (uint8_t *)pageNotFound, 0); /* mark as last segment */
        DEBUG_PRINT("[Http server task] Data Sent, len = %d\n\r", strlen ((const char *)pageNotFound));
    }
    else
    {
        httpRequest[requestIdx].serviceCallback(requestIdx, &argcCallback, argvCallback, netAppRequest);
    }
}

//*****************************************************************************
//
//! \brief This function parse and execute HTTP POST/PUT requests
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return None
//!
//****************************************************************************
void httpPostHandler(SlNetAppRequest_t *netAppRequest)
{
    uint16_t metadataLen;
    int32_t status;
    uint8_t requestIdx;

    uint8_t    argcCallback;
    uint8_t     *argvArray;
    uint8_t     **argvCallback = &argvArray;

    argvArray = gHttpPostBuffer;

    status = httpCheckContentInDB(netAppRequest,&requestIdx,&argcCallback, argvCallback);

    if (status < 0)
    {
        metadataLen = preparePostMetadata(status);

        sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer, SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA);
    }
    else
    {
        httpRequest[requestIdx].serviceCallback(requestIdx, &argcCallback, argvCallback, netAppRequest);
    }
}

//*****************************************************************************
//
//! \brief This is a temperature service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t tempGetCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    uint16_t metadataLen, elementType;
    int16_t value;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;

    while (*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        if ( *((uint16_t *)argvArray) != elementType)       /* content length is irrelevant for GET */
        {
            switch (*(argvArray + ARGV_VALUE_OFFSET))
            {
                /* TODO: set cases */
				
            }

            // Prepares payload
            sl_Memcpy (pPayload, httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic, strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic));
            pPayload += strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic);
            *pPayload++ = '=';

            // Copies value to payload
            sprintf((char *)pPayload, "%d", value);
            pPayload += strlen((const char *)pPayload); /* add the value length */
            *pPayload++ = '&';
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;       /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;                /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload-1) = '\0';

    metadataLen = prepareGetMetadata(0, strlen((const char *)gPayloadBuffer), HttpContentTypeList_UrlEncoded);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer, (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION | SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    DEBUG_PRINT("[Http server task] Metadata Sent, len = %d \n\r", metadataLen);

    sl_NetAppSend (netAppRequest->Handle, strlen ((const char *)gPayloadBuffer), gPayloadBuffer, 0); /* mark as last segment */
    DEBUG_PRINT("[Http server task] Data Sent, len = %d\n\r", strlen ((const char *)gPayloadBuffer));

    return 0;
}

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP GET
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightGetCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, *pPayload;
    uint8_t ledIdx;
    uint16_t ledState, metadataLen, elementType;

    argvArray = *argvCallback;
    pPayload = gPayloadBuffer;

    while (*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        if ( *((uint16_t *)argvArray) != elementType)       /* content length is irrelevant for GET */
        {
            switch (*(argvArray + ARGV_VALUE_OFFSET))
            {
                case LedIdx_RedLed:

                    ledIdx = Board_LED0;

                    break;
            }

            ledState = GPIO_read(ledIdx);
            sl_Memcpy (pPayload, httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic, strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic));
            pPayload += strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].characteristic);
            *pPayload++ = '=';
            sl_Memcpy (pPayload, httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].value[ledState], strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].value[ledState]));
            pPayload += strlen((const char *)httpRequest[requestIdx].charValues[*(argvArray + ARGV_VALUE_OFFSET)].value[ledState]);
            *pPayload++ = '&';
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;       /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;                /* skip the length */
    }

    /* NULL terminate the payload */
    *(pPayload-1) = '\0';

    metadataLen = prepareGetMetadata(0, strlen((const char *)gPayloadBuffer), HttpContentTypeList_UrlEncoded);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer, (SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION | SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA));
    DEBUG_PRINT("[Http server task] Metadata Sent, len = %d \n\r", metadataLen);

    sl_NetAppSend (netAppRequest->Handle, strlen ((const char *)gPayloadBuffer), gPayloadBuffer, 0); /* mark as last segment */
    DEBUG_PRINT("[Http server task] Data Sent, len = %d\n\r", strlen ((const char *)gPayloadBuffer));

    return 0;
}

//*****************************************************************************
//
//! \brief This is a light service callback function for HTTP POST
//!
//! \param[in]  requestIdx          request index to indicate the message
//!
//! \param[in]  argcCallback        count of input params to the service callback
//!
//! \param[in]  argvCallback        set of input params to the service callback
//!
//! \param[in] netAppRequest        netapp request structure
//!
//! \return 0 on success else negative
//!
//****************************************************************************
int32_t lightPostCallback(uint8_t requestIdx, uint8_t *argcCallback, uint8_t **argvCallback, SlNetAppRequest_t *netAppRequest)
{
    uint8_t *argvArray, ledIdx;
    uint16_t ledState, metadataLen, elementType;

    argvArray = *argvCallback;

    while (*argcCallback > 0)
    {
        elementType = setElementType(1, requestIdx, CONTENT_LEN_TYPE);
        if ( *((uint16_t *)argvArray) != elementType)       /* content length is irrelevant for POST */
        {
            if (*(argvArray + 1) & 0x80)    /* means it is the value, not the parameter */
            {
                /* get the light operation  */
                switch (*(argvArray + ARGV_VALUE_OFFSET))
                {
					/* TODO: set cases */

                }

                if (ledState == 0xFF)
                {
                    GPIO_toggle(ledIdx);
                }
                else
                {
                    GPIO_write(ledIdx, ledState);
                }
            }
            else    /* means it is the parameter, not the value */
            {
                /*  apply to the right light    */
                switch (*(argvArray + ARGV_VALUE_OFFSET))
                {
                    case LedIdx_RedLed:

                        ledIdx = Board_LED0;

                        break;
                }
            }
        }

        (*argcCallback)--;
        argvArray += ARGV_LEN_OFFSET;       /* skip the type */
        argvArray += *argvArray;    /* add the length */
        argvArray++;        /* skip the length */
    }

    metadataLen = preparePostMetadata(0);

    sl_NetAppSend (netAppRequest->Handle, metadataLen, gMetadataBuffer, SL_NETAPP_REQUEST_RESPONSE_FLAGS_METADATA);

    return 0;
}

//****************************************************************************
//                            MAIN FUNCTION
//****************************************************************************

//*****************************************************************************
//
//! \brief This task handles Httpserver transactions with the client
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * httpserverThread(void *pvParameters)
{
    mq_attr attr;
    int32_t msgqRetVal;
    int32_t retVal = -1;

    /* Switch off all LEDs on boards */
    GPIO_write(Board_LED0, Board_LED_OFF);
    GPIO_write(Board_LED1, Board_LED_OFF);
    GPIO_write(Board_LED2, Board_LED_OFF);

    DisplayBanner(APPLICATION_NAME, APPLICATION_VERSION);

    retVal = ConfigureSimpleLinkToDefaultState();
    if(retVal < 0)
    {
        UART_PRINT("[Http server task] Failed to configure the device in its default state.., halt\n\r");
        LOOP_FOREVER();
    }
    retVal = sl_Start(0, 0, 0);

    /* initializes mailbox for http messages */
    attr.mq_maxmsg = 1;         /* queue size */
    attr.mq_msgsize = sizeof( SlNetAppRequest_t* );      /* Size of message */
    httpserverMQueue = mq_open("httpserver msg q", O_CREAT, 0, &attr);
    if(httpserverMQueue == NULL)
    {
            UART_PRINT("[Http server task] could not create msg queue\n\r");
            while (1);
    }

    initHttpserverDB();
        
    while (1)
    {
        SlNetAppRequest_t *netAppRequest;

        msgqRetVal = mq_receive(httpserverMQueue, (char *)&netAppRequest, sizeof( SlNetAppRequest_t* ), NULL);
        if(msgqRetVal < 0)
        {
                UART_PRINT("[Http server task] could not receive element from msg queue\n\r");
                while (1);
        }

        DEBUG_PRINT("[Http server task] NetApp Request Received - handle from main context AppId = %d, Type = %d, Handle = %d\n\r", netAppRequest->AppId, netAppRequest->Type, netAppRequest->Handle);

        DEBUG_PRINT("[Http server task] Metadata len = %d\n\r", netAppRequest->requestData.MetadataLen);

        if ((netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET) || (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_DELETE ))
        {
            if (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_GET)
            {
                UART_PRINT("[Http server task] HTTP GET Request\n\r");
            }
            else
            {
                UART_PRINT("[Http server task] HTTP DELETE Request\n\r");
            }

            httpGetHandler(netAppRequest);
        }
        else if ((netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST) || (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_PUT))
        {

            if (netAppRequest->Type == SL_NETAPP_REQUEST_HTTP_POST)
            {
                UART_PRINT("[Http server task] HTTP POST Request\n\r");
            }
            else
            {
                UART_PRINT("[Http server task] HTTP PUT Request\n\r");
            }

            DEBUG_PRINT("[Http server task] Data received, len = %d, flags= %x\n\r", netAppRequest->requestData.PayloadLen, netAppRequest->requestData.Flags);

            httpPostHandler(netAppRequest);
        }

        if (netAppRequest->requestData.MetadataLen > 0)
        {
            free (netAppRequest->requestData.pMetadata);
        }
        if (netAppRequest->requestData.PayloadLen > 0)
        {
            free (netAppRequest->requestData.pPayload);
        }

        free (netAppRequest);
    }

}
