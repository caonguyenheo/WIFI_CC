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

#ifndef __LINK_LOCAL_TASK_H__
#define __LINK_LOCAL_TASK_H__

/* TI-DRIVERS Header files */
#include <ti/drivers/net/wifi/simplelink.h>

/* POSIX Header files */
#include <mqueue.h>
#include <semaphore.h>

//#define HTTP_DEBUG_PRINT

#ifdef HTTP_DEBUG_PRINT
#define DEBUG_PRINT  Report
#else
#define DEBUG_PRINT(x,...)
#endif

#define SL_STOP_TIMEOUT         (200)
#define SSID_LEN_MAX            (32)
#define BSSID_LEN_MAX           (6)

#define OOB_IS_NETAPP_MORE_DATA(flags)              ((flags & SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION) == SL_NETAPP_REQUEST_RESPONSE_FLAGS_CONTINUATION)
#define OOB_IS_NETAPP_ERROR(flags)                  ((flags & SL_NETAPP_REQUEST_RESPONSE_FLAGS_ERROR) == SL_NETAPP_REQUEST_RESPONSE_FLAGS_ERROR)

/* header fields definitions */
#define WEB_SERVER_VERSION                      "HTTP Version:"
#define WEB_SERVER_REQUEST_URI                  "Request URI:"
#define WEB_SERVER_QUERY_STRING                 "Query String:"
#define WEB_SERVER_HEADER_CONTENT_TYPE          "Content-Type: "
#define WEB_SERVER_HEADER_CONTENT_LEN           "Content-Length: "
#define WEB_SERVER_HEADER_LOCATION              "Location: "
#define WEB_SERVER_HEADER_SERVER                "Server: "
#define WEB_SERVER_HEADER_USER_AGENT            "User-Agent: "
#define WEB_SERVER_HEADER_COOKIE                "Cookie:"
#define WEB_SERVER_HEADER_SET_COOKIE            "Set-Cookie: "
#define WEB_SERVER_HEADER_UPGRADE               "Upgrade: "
#define WEB_SERVER_HEADER_REFERER               "Referer: "
#define WEB_SERVER_HEADER_ACCEPT                "Accept: "
#define WEB_SERVER_HEADER_CONTENT_ENCODING      "Content-Encoding: "
#define WEB_SERVER_HEADER_CONTENT_DISPOSITION   "Content-Disposition: "
#define WEB_SERVER_HEADER_CONNECTION            "Connection: "
#define WEB_SERVER_HEADER_ETAG                  "Etag: "
#define WEB_SERVER_HEADER_DATE                  "Date: "
#define WEB_SERVER_HEADER_HOST                  "Host: "
#define WEB_SERVER_HEADER_ACCEPT_ENCODING       "Accept-Encoding: "
#define WEB_SERVER_HEADER_ACCEPT_LANGUAGE       "Accept-Language: "
#define WEB_SERVER_HEADER_CONTENT_LANGUAGE      "Content-Language: "

/* Content types list */
#define TEXT_HTML                    "text/html"
#define TEXT_CSS                     "text/css"
#define TEXT_XML                     "text/xml"
#define APPLICATION_JSON             "application/json"
#define IMAGE_PNG                    "image/png"
#define IMAGE_GIF                    "image/gif"
#define TEXT_PLAIN                   "text/plain"
#define TEXT_CSV                     "text/csv"
#define APPLICATION_JAVASCRIPT       "application/javascript"
#define IMAGE_JPEG                   "image/jpeg"
#define APPLICATION_PDF              "application/pdf"
#define APPLICATION_ZIP              "application/zip"
#define SHOCKWAVE_FLASH              "application/x-shockwave-flash"
#define AUDIO_X_AAC                  "audio/x-aac"
#define IMAGE_X_ICON                 "image/x-icon"
#define TEXT_VCARD                   "text/vcard"
#define APPLICATION_OCTEC_STREAM     "application/octet-stream"
#define VIDEO_AVI                    "video/avi"
#define VIDEO_MPEG                   "video/mpeg"
#define VIDEO_MP4                    "video/mp4"
#define FORM_URLENCODED              "application/x-www-form-urlencoded"

/* MIME types list */
#define TEXT_HTML_MIME                  ".html"
#define TEXT_CSS_MIME                   ".css"
#define TEXT_XML_MIME                   ".xml"
#define APPLICATION_JSON_MIME           ".json"
#define IMAGE_PNG_MIME                  ".png"
#define IMAGE_GIF_MIME                  ".gif"
#define TEXT_PLAIN_MIME                 ".txt"
#define TEXT_CSV_MIME                   ".csv"
#define APPLICATION_JAVASCRIPT_MIME     ".js"
#define IMAGE_JPEG_MIME                 ".jpg"
#define APPLICATION_PDF_MIME            ".pdf"
#define APPLICATION_ZIP_MIME            ".zip"
#define SHOCKWAVE_FLASH_MIME            ".swf"
#define AUDIO_X_AAC_MIME                ".aac"
#define IMAGE_X_ICON_MIME               ".ico"
#define TEXT_VCARD_MIME                 ".vcf"
#define APPLICATION_OCTEC_STREAM_MIME   ".bin"
#define VIDEO_AVI_MIME                  ".avi"
#define VIDEO_MPEG_MIME                 ".mpeg"
#define VIDEO_MP4_MIME                  ".mp4"
#define URL_ENCODED_MIME                ".form"    /* dummy - no such extension */

typedef enum
{
/* Content types list */
    HttpContentTypeList_TextHtml,
    HttpContentTypeList_TextCSS,
    HttpContentTypeList_TextXML,
    HttpContentTypeList_ApplicationJson,
    HttpContentTypeList_ImagePNG,
    HttpContentTypeList_ImageGIF,
    HttpContentTypeList_TextPlain,
    HttpContentTypeList_TextCSV,
    HttpContentTypeList_ApplicationJavascript,
    HttpContentTypeList_ImageJPEG,
    HttpContentTypeList_ApplicationPDF,
    HttpContentTypeList_ApplicationZIP,
    HttpContentTypeList_ShokewaveFlash,
    HttpContentTypeList_AudioXAAC,
    HttpContentTypeList_ImageXIcon,
    HttpContentTypeList_TextVcard,
    HttpContentTypeList_ApplicationOctecStream,
    HttpContentTypeList_VideoAVI,
    HttpContentTypeList_VideoMPEG,
    HttpContentTypeList_VideoMP4,
    HttpContentTypeList_UrlEncoded,
}HttpContentTypeList;

#define CONTENT_LEN_TYPE    0xFF

/* offsets of TLV structure of parameters parsed in NetApp request */
#define ARGV_TYPE_OFFSET    0
#define ARGV_LEN_OFFSET        2
#define ARGV_VALUE_OFFSET    3

typedef struct    _http_headerFieldType_t_
{
    SlNetAppMetadataHTTPTypes_e headerType;
    char * headerText;
}http_headerFieldType_t;

typedef struct    _http_contentTypeMapping_t_
{
    HttpContentTypeList contentType;
    char *contentTypeText;
    char *mimeExt;
}http_contentTypeMapping_t;

typedef struct    _http_charValuesPair_t_
{
    char    *characteristic;
    char    *value[5];
}http_charValuesPair_t;

typedef struct    _http_RequestObj_t_
{
    uint8_t    requestIdx;
    uint8_t    httpMethod;
    char       *service;
    http_charValuesPair_t    charValues[10];
    int32_t (*serviceCallback)(uint8_t, uint8_t *, uint8_t **, SlNetAppRequest_t *);
}http_RequestObj_t;

/* Status keeping MACROS */

#define SET_STATUS_BIT(status_variable, bit) status_variable |= (1<<(bit))

#define CLR_STATUS_BIT(status_variable, bit) status_variable &= ~(1<<(bit))

#define GET_STATUS_BIT(status_variable, bit)    \
                                (0 != (status_variable & (1<<(bit))))

#define IS_NW_PROCSR_ON(status_variable)    \
                GET_STATUS_BIT(status_variable, AppStatusBits_NwpInit)

#define IS_CONNECTED(status_variable)       \
                GET_STATUS_BIT(status_variable, AppStatusBits_Connection)

#define IS_IP_LEASED(status_variable)       \
                GET_STATUS_BIT(status_variable, AppStatusBits_IpLeased)

#define IS_IP_ACQUIRED(status_variable)     \
                GET_STATUS_BIT(status_variable, AppStatusBits_IpAcquired)

#define IS_IPV6L_ACQUIRED(status_variable)  \
                GET_STATUS_BIT(status_variable, AppStatusBits_Ipv6lAcquired)

#define IS_IPV6G_ACQUIRED(status_variable)  \
                GET_STATUS_BIT(status_variable, AppStatusBits_Ipv6gAcquired)

#define IS_SMART_CFG_START(status_variable) \
                GET_STATUS_BIT(status_variable, AppStatusBits_SmartconfigStart)

#define IS_P2P_DEV_FOUND(status_variable)   \
                GET_STATUS_BIT(status_variable, AppStatusBits_P2pDevFound)

#define IS_P2P_REQ_RCVD(status_variable)    \
                GET_STATUS_BIT(status_variable, AppStatusBits_P2pReqReceived)

#define IS_CONNECT_FAILED(status_variable)  \
                GET_STATUS_BIT(status_variable, AppStatusBits_Connection)

#define IS_PING_DONE(status_variable)       \
                GET_STATUS_BIT(status_variable, AppStatusBits_PingDone)

#define IS_AUTHENTICATION_FAILED(status_variable)   \
            GET_STATUS_BIT(status_variable, AppStatusBits_AuthenticationFailed)

/* Application specific status/error codes */
typedef enum
{
    /* Choosing -0x7D0 to avoid overlap w/ host-driver's error codes */
    AppStatusCodes_LanConnectionFailed = -0x7D0,
    AppStatusCodes_InternetConnectionFailed = AppStatusCodes_LanConnectionFailed - 1,
    AppStatusCodes_DeviceNotInStationMode = AppStatusCodes_InternetConnectionFailed - 1,
    AppStatusCodes_StatusCodeMax = -0xBB8
}AppStatusCodes;

typedef enum
{
    AppStatusBits_NwpInit = 0,    /* If this bit is set: Network Processor is powered up */
    AppStatusBits_Connection = 1,  /* If this bit is set: the device is connected to the AP or client is connected to device (AP) */
    AppStatusBits_IpLeased = 2,   /* If this bit is set: the device has leased IP to any connected client */
    AppStatusBits_IpAcquired = 3, /* If this bit is set: the device has acquired an IP */
    AppStatusBits_SmartconfigStart = 4,   /* If this bit is set: the SmartConfiguration process is started from SmartConfig app */
    AppStatusBits_P2pDevFound = 5,       /* If this bit is set: the device (P2P mode) found any p2p-device in scan */
    AppStatusBits_P2pReqReceived = 6,    /* If this bit is set: the device (P2P mode) found any p2p-negotiation request */
    AppStatusBits_ConnectionFailed = 7,   /* If this bit is set: the device(P2P mode) connection to client(or reverse way) is failed */
    AppStatusBits_PingDone = 8,   /* If this bit is set: the device has completed the ping operation */
    AppStatusBits_Ipv6lAcquired = 9,  /* If this bit is set: the device has acquired an IPv6 address */
    AppStatusBits_Ipv6gAcquired = 10,  /* If this bit is set: the device has acquired an IPv6 address */
    AppStatusBits_AuthenticationFailed = 11,
    AppStatusBits_ResetRequired = 12,
}AppStatusBits;

typedef enum
{
    ControlMessageType_Switch2 = 2,
    ControlMessageType_Switch3 = 3,
    ControlMessageType_ResetRequired = 4,
    ControlMessageType_ControlMessagesMax = 0xFF
}ControlMessageType;

/* Control block definition */
typedef struct App_ControlBlock_t
{
    uint32_t  status;                                   /* SimpleLink Status */
    uint32_t  gatewayIP;                            /* Network Gateway IP address */
    uint8_t   connectionSSID[SSID_LEN_MAX+1];           /* Connection SSID */
    uint8_t   ssidLen;                                  /* Connection SSID */
    uint8_t   connectionBSSID[BSSID_LEN_MAX];           /* Connection BSSID */
}App_CB;

//typedef enum
//{
//    LedValues_Off,
//    LedValues_On,
//}LedValues;

typedef enum
{
    TempIdx_C,
    TempIdx_F,
}TempIdx;

typedef enum
{
    LedIdx_RedLed,
    LedIdx_OrangeLed,
    LedIdx_GreenLed,
    LedIdx_MaxLed,
}LedIdx;

typedef enum
{
    LedValues_Off,
    LedValues_On,
    LedValues_Toggle,
    LedValues_MaxLed,
}LedValues;

/****************************************************************************
                      GLOBAL VARIABLES
****************************************************************************/
extern App_CB App_ControlBlock;


//****************************************************************************
//                      FUNCTION PROTOTYPES
//****************************************************************************

//*****************************************************************************
//
//! \brief This task handles LinkLocal transactions with the client
//!
//! \param[in]  None
//!
//! \return None
//!
//****************************************************************************
void * httpserverThread(void *pvParameters);

#endif
