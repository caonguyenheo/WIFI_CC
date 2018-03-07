#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>


#include "system.h"
#include "cc3200_system.h"
#include "server_api.h"
#include "p2p_main.h"
#include "userconfig.h"
#include "uart_term.h"

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/wlan.h>
#include "nonos.h"
/* Example/Board Header files */
#include "network_terminal.h"
//#include "cmd_parser.h"
#include "wlan_cmd.h"
#include "osi.h"

#ifdef NTP_CHINA
char g_acSNTPserver[NUM_NTP_SERVER][NTP_RUL_MAXLEN] = {"cn.pool.ntp.org", "cn.ntp.org.cn", "ntp1.aliyun.com", "ntp2.aliyun.com"};
#else
char g_acSNTPserver[NUM_NTP_SERVER][NTP_RUL_MAXLEN] = {"pool.ntp.org", "time.nist.gov", "time4.google.com", "nist1-nj2.ustiming.org", "ntp-nist.ldsbc.edu"};
#endif

#define g_acSNTPserver_single (&g_acSNTPserver[0][0])
char relay_url_ch[rms_size] = {0};
char SERVER_URL[stun_size] = {0};
char MYCIN_API_HOST_NAME[api_size] = {0};
char MYCIN_HOST_NAME[mqtt_size] = {0};
static char *ParseString (char *instring)
{
    char *ptr1 = instring, *ptr2 = instring;

    if (instring == NULL)
        return instring;

    while (isspace (*ptr1))
        ptr1++;

    while (*ptr1)
    {
        if (*ptr1 == '+')
        {
            ptr1++; *ptr2++ = ' ';
        }
        else if (*ptr1 == '%' && isxdigit (*(ptr1 + 1)) && isxdigit (*(ptr1 + 2)))
        {
            ptr1++;
            *ptr2    = ((*ptr1 >= '0' && *ptr1 <= '9') ? (*ptr1 - '0') : ((char)toupper(*ptr1) - 'A' + 10)) << 4;
            ptr1++;
            *ptr2++ |= ((*ptr1 >= '0' && *ptr1 <= '9') ? (*ptr1 - '0') : ((char)toupper(*ptr1) - 'A' + 10));
            ptr1++;
        }
        else
            *ptr2++ = *ptr1++;
    }

    /*while (ptr2 > instring && isspace(*(ptr2 - 1)))
        ptr2--;*/

    *ptr2 = '\0';

    return instring;
}
char cam_settings[CAM_LEN]={0};
int get_cam_default()
{
	int ret_val = 0;
	char temp_str[CAM_LEN]=CAM_SETTING_DEFAULT;
	ret_val = userconfig_get(cam_settings, CAM_LEN, CAM_ID);
	if(ret_val <= 0)
	{
		memset(cam_settings,0,CAM_LEN);
		memcpy(cam_settings,temp_str,CAM_LEN);
		//UART_PRINT("\r\nSet cam default: flicker %d UD %d LR %d brate %d kbps framerate %d resolution %d\r\n", cam_settings[0],cam_settings[1],cam_settings[2],cam_settings[3]*256+cam_settings[4],cam_settings[5],cam_settings[6]*256+cam_settings[7]);
		userconfig_set(cam_settings, CAM_LEN, CAM_ID);
		userconfig_flash_write();
	}
	//UART_PRINT("\r\nSetting ret=%d: flicker %d UD %d LR %d brate %d kbps framerate %d resolution %d\r\n", ret_val, cam_settings[0],cam_settings[1],cam_settings[2],cam_settings[3]*256+cam_settings[4],cam_settings[5],cam_settings[6]*256+cam_settings[7]);
	return 0;
}


void set_server_authen(char *api_key, char *timezone)
{
    memcpy(apiKey, api_key, strlen(api_key));
    memcpy(timeZone, timezone, strlen(timezone));
}
int set_url_default(int val_in)
{
	int Retval = 0;
/***********************************start set api default**********************************/
	if(val_in == 0)
	{
		memset(MYCIN_API_HOST_NAME, 0, strlen(MYCIN_API_HOST_NAME));
		strcpy(MYCIN_API_HOST_NAME, API_URL_DEFAULT);
		Retval = userconfig_set(MYCIN_API_HOST_NAME, strlen(MYCIN_API_HOST_NAME), API_URL);
		if(Retval < 0)
		{
			UART_PRINT("API_VALUE_ERROR: %s\r\n");
		}
		else
		{
			UART_PRINT("SET_API_VALUE: %s\r\n", MYCIN_API_HOST_NAME);
		}
	}
/*****************************************************************************************/


/***********************************start set mqtt default********************************/
	if(val_in == 1)
	{
		memset(MYCIN_HOST_NAME, 0, strlen(MYCIN_HOST_NAME));
		strcpy(MYCIN_HOST_NAME, MQTT_URL_DEFAULT);
		Retval=userconfig_set(MYCIN_HOST_NAME, strlen(MYCIN_HOST_NAME), MQTT_URL);
		if(Retval < 0)
		{
			UART_PRINT("MQTT_VALUE_ERROR: %s\r\n");
		}
		else
		{
			UART_PRINT("SET_MQTT_VALUE: %s\r\n", MQTT_URL_DEFAULT);
		}
	}
/*****************************************************************************************/


/***********************************start set ntp default********************************/
	if(val_in == 2)
	{
		memset(g_acSNTPserver_single, 0, strlen(g_acSNTPserver_single));
		strcpy(g_acSNTPserver_single, NTP_URL_DEFAULT);
		Retval=userconfig_set(g_acSNTPserver_single, strlen(g_acSNTPserver_single), NTP_URL);
		if(Retval < 0)
		{
			UART_PRINT("SNTP_VALUE_ERROR\r\n");
		}
		else
		{
			UART_PRINT("set_SNTP_VALUE: %s\r\n", NTP_URL_DEFAULT);
		}
	}
/*****************************************************************************************/


/***********************************start set rms default********************************/
	if(val_in == 3)
	{
		memset(relay_url_ch, 0, strlen(relay_url_ch));
		strcpy(relay_url_ch, RMS_URL_DEFAULT);
		Retval=userconfig_set( relay_url_ch, strlen( relay_url_ch), RMS_URL);
		if(Retval < 0)
		{
			UART_PRINT("RMS_VALUE_ERROR\r\n");
		}
		else
		{
			UART_PRINT("SET_RMS_VALUE: %s\r\n", RMS_URL_DEFAULT);
		}
	}
/*****************************************************************************************/


/***********************************start set stun default********************************/
	if(val_in == 4)
	{
	memset(SERVER_URL, 0, strlen(SERVER_URL));
	strcpy(SERVER_URL, STUN_URL_DEFAULT);
	Retval = userconfig_set(SERVER_URL, strlen(SERVER_URL), STUN_URL);
	UART_PRINT("SET_STUN_VALUE: %s\r\n", STUN_URL_DEFAULT);
	if(Retval < 0)
	{
		UART_PRINT("SET_STUN_VALUE_ERROR\r\n");
	}
	else
	{
		UART_PRINT("SET_RMS_VALUE: %s\r\n", STUN_URL_DEFAULT);
	}
	}
//	userconfig_flash_write();
/*****************************************************************************************/

	return 0;
}
int get_url_default()
{
	int  ret_val = 0, value_in = 0;
	int bool_to_write = 0;

	memset(MYCIN_API_HOST_NAME, 0, strlen(MYCIN_API_HOST_NAME));
	memset(MYCIN_HOST_NAME, 0, strlen(MYCIN_HOST_NAME));
	memset(g_acSNTPserver_single, 0, strlen(g_acSNTPserver_single));
	memset(relay_url_ch, 0, strlen(relay_url_ch));
	memset(SERVER_URL, 0, strlen(SERVER_URL));

/***************************************get_api_default ***************************************/
	ret_val = userconfig_get(MYCIN_API_HOST_NAME, api_size, API_URL);
	if(ret_val <= 0)
	{
		value_in = 0;
		set_url_default(value_in);
		ret_val = userconfig_get(MYCIN_API_HOST_NAME, api_size, API_URL);
		if(ret_val < 0)
		{
			UART_PRINT("GET_API_VALUE_FAIL\r\n");
		}
		bool_to_write = 1;
		UART_PRINT("GET_API_VALUE %d %s\r\n", ret_val, MYCIN_API_HOST_NAME);
	}
	//UART_PRINT("GET_API_VALUE %d %s\r\n", ret_val, MYCIN_API_HOST_NAME);

/*********************************************************************************************/


/**********************************************get_mqtt_default*******************************/
	ret_val = userconfig_get(MYCIN_HOST_NAME, mqtt_size, MQTT_URL);
	if (ret_val <= 0)
	{
		value_in = 1;
		set_url_default(value_in);
		ret_val = userconfig_get(MYCIN_HOST_NAME, mqtt_size, MQTT_URL);
		if(ret_val < 0)
		{
			UART_PRINT("GET_MQTT_VALUE_FAIL\r\n");
		}
		bool_to_write = 1;
		UART_PRINT("GET_MQTT_VALUE %d %s\r\n", ret_val, MYCIN_HOST_NAME);
	}
	//UART_PRINT("GET_MQTT_VALUE: %d %s\r\n", ret_val, MYCIN_HOST_NAME);

/******************************************************************************************/


/********************************************get_ntp_default********************************/

	ret_val = userconfig_get(g_acSNTPserver_single, ntp_size, NTP_URL);
	if (ret_val <= 0)
	{
		value_in = 2;
		set_url_default(value_in);
		ret_val = userconfig_get(g_acSNTPserver_single, ntp_size, NTP_URL);
		if(ret_val < 0)
		{
			UART_PRINT("GET_NTP_VALUE_FAIL\r\n");
		}
		bool_to_write = 1;
		UART_PRINT("GET_NTP_VALUE %d %s\r\n", ret_val, g_acSNTPserver_single);
	}
	//UART_PRINT("GET_NTP_VALUE: %d %s\r\n", ret_val, g_acSNTPserver);

/******************************************************************************************/


/*****************************************get_rms_default**********************************/

	ret_val = userconfig_get(relay_url_ch, rms_size, RMS_URL);
	if (ret_val <= 0)
	{
		value_in = 3;
		set_url_default(value_in);
		ret_val = userconfig_get(relay_url_ch, rms_size, RMS_URL);
		if(ret_val < 0)
		{
			UART_PRINT("GET_RMS_VALUE_FAIL\r\n");
		}
		bool_to_write = 1;
		UART_PRINT("GET_RMS_VALUE %d %s\r\n", ret_val, relay_url_ch);
	}
	//UART_PRINT("GET_RMS_VALUE: %d %s\r\n", ret_val, relay_url_ch);

/******************************************************************************************/


/*****************************************get_stun_default*********************************/

	ret_val = userconfig_get(SERVER_URL, stun_size, STUN_URL);
	if (ret_val <= 0)
	{
		value_in = 4;
		set_url_default(value_in);
		ret_val = userconfig_get(SERVER_URL, stun_size, STUN_URL);
		if(ret_val < 0)
		{
			UART_PRINT("GET_STUN_VALUE_FAIL\r\n");
		}
		bool_to_write = 1;
		UART_PRINT("GET_STUN_VALUE: %d %s\r\n", ret_val, SERVER_URL);
	}
	//UART_PRINT("GET_STUN_VALUE: %d %s\r\n", ret_val, SERVER_URL);

	if (bool_to_write)
	{
		userconfig_flash_write();
	}
/******************************************************************************************/
	return;
}
int cam_mode = -1;
int system_IsRegistered()
{
    int modeID;
// sl_Start(0, 0, 0);
    //  userconfig_init();
//  userconfig_flash_read();
    if ((modeID = userconfig_get(NULL, 0, MODE_ID)) < 0)
    {
//    sl_Stop(200);
    	 UART_PRINT("MODE_GET_CONFIG_FAIL, error=%d\r\n", modeID);
        return -1;
    }
    if (modeID != DEV_REGISTERED)
    {
//    sl_Stop(200);
//    	UART_PRINT("MODE_PARING, error=%d\r\n", modeID);
    	cam_mode = DEV_NOT_REGISTERED;
        return -1;
    }
    else
    {
//	    UART_PRINT("MODE_ONLINE\r\n");
	    cam_mode = DEV_REGISTERED;
//    sl_Stop(200);
        return 0;
    }
}
int system_registration()
{
	int RetVal;
	UART_PRINT("Network init start\n");
    sl_WlanProfileDel(0xFF);
    RetVal = ConfigureSimpleLinkToDefaultState();
    if(RetVal < 0)
    {
        /* Handle Error */
        UART_PRINT("Network Terminal - Couldn't configure Network Processor\n");
        return(NULL);
    }
    InitializeAppVariables();
//    Network_IF_InitDriver(ROLE_STA);
    Network_IF_WifiSetMode(ROLE_STA);
    UART_PRINT("\nNetwork init done\r\n");
    Network_IF_provisionAP();
    UART_PRINT("Scan wifi done\r\n");
    //Re-Start SimpleLink in AP Mode
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

    HttpServerInitAndRun(NULL);
	return 0;
}
int system_init(void)
{
    userconfig_init();
    userconfig_flash_read();
    get_url_default();
	return 0;
}
