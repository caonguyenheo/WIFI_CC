/*
 * p2p_main.h
 *
 *  Created on: Aug 3, 2017
 *      Author: Admin
 */

#include"datatypes.h"
typedef enum command_type
    {
    P2P_COMMAND = 0,
    MQTT_COMMAND,
    HTTP_COMMAND,
    }command_type;
enum EVENT {
    	eEVENT_INIT_DONE = 0,
    	eEVENT_START_STREAMING,
    	eEVENT_STREAMING,
    	eEVENT_STOP_STREAMING,
    	eEVENT_STOP_WAIT,
    	eEVENT_MAX
};
#define P2P_KEY_CHAR_LEN        17
#define P2P_RN_CHAR_LEN         13
#define P2P_KEY_HEX_LEN         33
#define P2P_RN_HEX_LEN          25
#define P2P_PS_IP_LEN           20
#define P2P_PS_URL_LEN          40
#define P2P_PS_PORT_LEN         8

#define STR_IP_BUFFER_SZ        20
#define STR_PORT_BUFFER_SZ      20
#define MAX_STR_LEN             32
#define MAX_STR_PARAM_LEN       64


#define STR_RELAY               "relay"
#define STR_COMMAND             "command"
#define STR_REQ                 "req"
#define STR_MODE                "mode"
#define STR_REMOTE              "remote"
#define STR_COMBINE             "combine"
#define STR_LOCAL               "local"
#define STR_PORT_1              "port1"
#define STR_IP                  "ip"
#define STR_STREAM_NAME         "streamname"
#define STR_TIME                "time"


#define STR_CMD_VALUE           "value"
#define STR_CMD_UPDOWN     	   	"updown"
#define STR_CMD_LEFTRIGHT      	"leftright"
#define STR_CMD_URL     		"url_set"
#define STR_CMD_API     		"api_url"
#define STR_CMD_MQTT    		"mqtt_url"
#define STR_CMD_NTP     		"ntp_url"
#define STR_CMD_RMS     		"rms_url"
#define STR_CMD_STUN    		"stun_url"
#define STR_CMD_AUTH          	"auth"
#define STR_CMD_SSID            "ssid"
#define STR_CMD_KEY             "key"
#define STR_CMD_INDEX           "index"
#define STR_CMD_TIMEZONE        "timezone"

#define SIZE_FLIP        4
#define SIZE_BITRATE     8
#define SIZE_FRAMERATE   4
#define SIZE_FLICKER     4
#define SIZE_RESOLUTION  4
#define SIZE_CAM_ALL     12
int command_handler(char* input, int length, command_type commandtype, char* response, int* response_len);
INT32 get_ps_info(char *out_buf, UINT32 out_buf_sz,  UINT32 *used_bytes);
INT32 fast_ps_mode();
