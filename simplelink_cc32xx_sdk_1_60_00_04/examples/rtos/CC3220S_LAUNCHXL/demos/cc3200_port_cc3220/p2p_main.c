/*
 * p2p_main.c
 *
 *  Created on: Aug 3, 2017
 *      Author: Admin
 */

#include "p2p_main.h"
#include "url_parser.h"
#include "uart_term.h"
#include "cc3200_system.h"
#include "network_terminal.h"
#include "system.h"
char p2p_key_char[P2P_KEY_CHAR_LEN] = {0};
char p2p_rn_char[P2P_RN_CHAR_LEN] = {0};
char p2p_key_hex[P2P_KEY_HEX_LEN] = {0};
char p2p_rn_hex[P2P_RN_HEX_LEN] = {0};

char ps_ip[P2P_PS_IP_LEN] = {0};
char ps_url[P2P_PS_URL_LEN] = {0};
char pr_ip[P2P_PS_IP_LEN] = {0};
char pl_ip[P2P_PS_IP_LEN] = {0};
char my_pr_ip[P2P_PS_IP_LEN] = {0};
char my_pl_ip[P2P_PS_IP_LEN] = {0};
uint32_t ps_port = 20000, pl_port = 20000, pr_port = 20000, my_pr_port = 20000, my_pl_port = 20000, ps_ssl_port = 443;
char str_ps_port[P2P_PS_PORT_LEN]={0};

#define MAX_CMD_BUFLEN 10

#define PS_TIMEOUT 7000
#define UPLOAD_TIMEOUT 20
#define AKKL_TIMEOUT 1
#define PR_TIMEOUT 7000
#define PL_TIMEOUT 7000

// Socket waiting time in ms
#define WAITING_TIME    15
int command_handler(char* input, int length, command_type commandtype, char* response, int* response_len) {
    int ret_val = 0;
    url_parser parsed_url;
    char* l_command_ptr = input;
    char l_response[512]; //Response max 512 bytes
    char *ptr_response = l_response;
    int l_response_len = 0;
    char str_command[64];
    int str_command_sz = sizeof(str_command);
    int command_num=-1;
    if (commandtype != MQTT_COMMAND && commandtype != P2P_COMMAND && commandtype != HTTP_COMMAND) {
           goto exit_error;
       }
    ret_val = parse_url(l_command_ptr, &parsed_url);
    if (ret_val != 0) {
        // UART_PRINT("Parse failed!\r\n");
        goto exit_error;
    }
    if (HTTP_COMMAND == commandtype) {
         ret_val = getValueFromKey(&parsed_url, STR_REQ, str_command, str_command_sz);
          if (ret_val != 0) {
              goto exit_error;
          }
     }
    else {
        goto exit_error;
   }
    command_num=command_string_2_token(str_command);
    UART_PRINT("COMMAND HANDLER %d\r\n",command_num);
   switch (command_num) {
   case CMD_GET_VERSION:
   {
         char version[24] = {0};
//         ret_val = get_version(version);
//         UART_PRINT("version : %s\r\n", version);
//         if (ret_val != 0) {
//             goto exit_error;
//         }
//         l_response_len += sprintf(ptr_response, "%s: %s", str_command, version);
         l_response_len += sprintf(ptr_response, "%s: %s", str_command, "02.04.14");
         memcpy(response, l_response, l_response_len);
   }
   break;
   case CMD_ID_GET_MAC:
   {
        char strMacAddr[13] = {0};
        ret_val = get_mac_address(strMacAddr);
        if (ret_val != 0) {
            goto exit_error;
        }
       l_response_len += sprintf(ptr_response, "%s: %s", str_command, strMacAddr);
       memcpy(response, l_response, l_response_len);
   }
   break;
   case CMD_ID_GET_UDID:
   {
       char strUdid[32] = {0};
       ret_val = get_uid(strUdid);
       if (ret_val != 0) {
           goto exit_error;
       }
//       UART_PRINT("get_udid: %s\r\n", strUdid);
       l_response_len += sprintf(ptr_response, "%s: %s", str_command, strUdid);
       memcpy(response, l_response, l_response_len);
   }
   break;
   case CMD_ID_CHECK_FIRMWARE_UPGRADE:
   {
       l_response_len += sprintf(ptr_response, "%s: %s", str_command, "new_version");
       memcpy(response, l_response, l_response_len);
   }
   break;
   case CMD_ID_RESTART_SYSTEM:
   {
         // optional restart system or switch device to station mode to connect to an AP
   	l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
   	 *response_len = -1;
   	 memcpy(response, l_response, l_response_len);
//   	 cmd_processing_status = 0;
   	 return -1;
   }
   break;
   case CMD_ID_GET_RT_LIST:
   {
   	l_response_len += sprintf(ptr_response, "%s: %s", str_command, "router_list");
   	*response_len = l_response_len;
   }
   break;
   case CMD_ID_SET_SERVER_AUTH:
   {
       char strValue[64] = {0}, strTimezone[32] = {0};

       ret_val = getValueFromKey(&parsed_url, STR_CMD_VALUE, strValue, sizeof(strValue));
       if (ret_val != 0){
           goto exit_error;
       }

       ret_val = getValueFromKey(&parsed_url, STR_CMD_TIMEZONE, strTimezone, sizeof(strTimezone));
       if (ret_val != 0){
          goto exit_error;
       }
       set_server_authen(strValue, strTimezone);
       UART_PRINT("API_key: %s TimeZone: %s\r\n", strValue, strTimezone);
       l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
   }
   break;
   case CMD_ID_SETUP_WIRELESS_SAVE:
   {
      char str_auth[MAX_STR_PARAM_LEN], str_ssid[MAX_STR_PARAM_LEN], str_key[MAX_STR_PARAM_LEN], str_index[MAX_STR_PARAM_LEN];

      ret_val = getValueFromKey(&parsed_url, STR_CMD_AUTH, str_auth, sizeof(str_auth));
      if (ret_val != 0){
       	goto exit_error;
      }

      ret_val = getValueFromKey(&parsed_url, STR_CMD_SSID, str_ssid, sizeof(str_ssid));
      if (ret_val != 0){
         goto exit_error;
      }

      ret_val = getValueFromKey(&parsed_url, STR_CMD_KEY, str_key, sizeof(str_key));
      if (ret_val != 0){
        goto exit_error;
      }

      ret_val = getValueFromKey(&parsed_url, STR_CMD_INDEX, str_index, sizeof(str_index));
      if (ret_val != 0){
        goto exit_error;
      }
      UART_PRINT("\r\nssid=%s\r\n pass=%s\r\n auth=%s\r\n index=%s\r\n", str_ssid, str_key, str_auth, str_index);
      l_response_len += sprintf(ptr_response, "%s: %d", str_command, set_wireless_config(str_ssid, str_key, str_auth));
//      l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
   }
   break;
   case CMD_ID_URL_SET:
   {
   	char str_api_url[MAX_STR_LEN ] = {0}, str_mqtt_url[MAX_STR_LEN ] = {0}, str_ntp_url[MAX_STR_LEN ] = {0}, str_rms_url[MAX_STR_LEN ] = {0},
   	     str_stun_url[MAX_STR_LEN ] = {0};

   	ret_val = getValueFromKey(&parsed_url, STR_CMD_API, str_api_url, sizeof(str_api_url));
   	if (ret_val != 0){
   		memset(str_api_url, 0x00, strlen(str_api_url));
   		strcpy(str_api_url, API_URL_DEFAULT);
   	}

   	ret_val = getValueFromKey(&parsed_url, STR_CMD_MQTT, str_mqtt_url, sizeof(str_mqtt_url));
   	if (ret_val != 0){
   		memset(str_mqtt_url, 0x00, strlen(str_mqtt_url));
   		strcpy(str_mqtt_url, MQTT_URL_DEFAULT);
   	}

   	ret_val = getValueFromKey(&parsed_url, STR_CMD_NTP, str_ntp_url, sizeof(str_ntp_url));
   	if (ret_val != 0){
   		memset(str_ntp_url, 0x00, strlen(str_ntp_url));
   		strcpy( str_ntp_url, NTP_URL_DEFAULT);
   	}

   	ret_val = getValueFromKey(&parsed_url, STR_CMD_RMS, str_rms_url, sizeof(str_rms_url));
   	if (ret_val != 0){
   		memset(str_rms_url, 0x00, strlen(str_rms_url));
   		strcpy(str_rms_url, RMS_URL_DEFAULT);
   	}

   	ret_val = getValueFromKey(&parsed_url, STR_CMD_STUN, str_stun_url, sizeof(str_stun_url));
   	if (ret_val != 0){
   		memset(str_stun_url, 0x00, strlen(str_stun_url));
   		strcpy(str_stun_url, STUN_URL_DEFAULT);
   	}
   	UART_PRINT("api_url=%s\r\n mqtt_url=%s\r\n ntp_url=%s\r\n rms_url=%s\r\n stun_url=%s\r\n", str_api_url, str_mqtt_url, str_ntp_url, str_rms_url, str_stun_url);
   	l_response_len += sprintf(ptr_response, "%s: %d", str_command, set_url(str_api_url, str_mqtt_url, str_ntp_url, str_rms_url, str_stun_url));
 //	l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
   }
   break;
   case CMD_ID_SET_CITY_TIMEZONE:
   {
   	char str_set_timezone[32] = {0};
   	ret_val = getValueFromKey(&parsed_url, STR_CMD_VALUE, str_set_timezone, sizeof(str_set_timezone));
   	if (ret_val != 0){
   		goto exit_error;
   	}
   	UART_PRINT("\r\nset_timezone_value = %s\r\n", str_set_timezone);
   	l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
   }
   break;
   case CMD_ID_SET_FLICKER:
   {
 	  	char str_set_flicker[SIZE_FLICKER] = {0};
 		ret_val = getValueFromKey(&parsed_url, STR_CMD_VALUE, str_set_flicker, sizeof(str_set_flicker));
 		if (ret_val != 0)
 		{
 			goto exit_error;
 		}
 		UART_PRINT("\r\nset_flicker_value = %s\r\n", str_set_flicker);
 		cam_settings[0]=atoi(str_set_flicker);
 		cam_settings_update();
 		l_response_len += sprintf(ptr_response, "%s: %d", str_command, 0);
// 		set_down_anyka(2);
   }
   break;
   default:
  		if (HTTP_COMMAND == commandtype)
  			return 404;
  		else
      	 l_response_len += sprintf(ptr_response, "%s: -1", str_command);
         break;
   }
   if (commandtype == MQTT_COMMAND) {
//	    signal_event(eEVENT_STREAMING);
//       char pub_topic[128] = {0};
//       ret_val = getValueFromKey(&parsed_url, "app_topic_sub", pub_topic, sizeof(pub_topic));
//       if (ret_val != 0) {
//           // Print out error
//           goto exit_error;
//       }
//       ret_val = mqtt_pub(pub_topic, l_response);
//       if (ret_val < 0) {
//           UART_PRINT("fail to pub\r\n");
//           goto exit_error;
//       }
       UART_PRINT("\r\nPUB: %s\r\n", l_response);
//	    if (server_req_upgrade){
//	        while(server_req_upgrade<1100){
//	        	UART_PRINT(".");
//	        	server_req_upgrade++;
//	        }
//	        system_reboot();
//	    }
   }
   else {
       // Because response generated for MQTT so in case http command: remove "3" prefix
       if (response != NULL && response_len != NULL) {
       	  memcpy(response, l_response, l_response_len);
       	  UART_PRINT("Response: '%s'\r\n", response);
       }
       else {
           UART_PRINT("WARN: response/response_len are NULL\r\n");
       }
   }
    return 200;
exit_error:
    l_response_len = -1;
    return;
}
