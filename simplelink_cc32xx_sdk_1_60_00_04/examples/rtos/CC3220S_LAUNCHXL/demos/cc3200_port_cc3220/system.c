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
#include "p2p_config.h"

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/net/wifi/wlan.h>
#include "nonos.h"
/* Example/Board Header files */
#include "network_terminal.h"
//#include "cmd_parser.h"
#include "wlan_cmd.h"
#include "osi.h"
//#include "platform.h"
#include <time.h>

extern char ps_ip[];
extern uint32_t ps_port;
extern char p2p_key_char[];
extern char p2p_rn_char[];
extern char p2p_key_hex[];
extern char p2p_rn_hex[];
extern char str_ps_port[];
char timeoutbuffer[];
extern char mqtt_rece[];

myCinUserInformation_t g_mycinUser;
char g_version_8char[9]={0};
char g_strMacAddr[13]={0};
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
timer_t gTimer;
int timezone_s=0;
char ak_version[7]={0};
char ak_version_format[9]={0};
//int32_t ConfigureSimpleLinkToDefaultState();
void escapeJsonString(char* in_str, char* out_str){
	int l_len=strlen(in_str);
	int i,j=0;
	for(i=0;i<l_len;i++){
		switch (in_str[i]){
			case '\\':
			case '"':
			case '/':
			case '\b':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
				out_str[j++]='\\';
				break;
			default:
				break;
		}
		out_str[j++]=in_str[i];
	}
	out_str[j]=0x00;
}
int sec_string_to_num(){
	int l_len=strlen(ap_sec);
	int i;
	for(i=0;i<l_len;i++)
		ap_sec[i]=tolower(ap_sec[i]);
	UART_PRINT("Wifi Sec %s\r\n",ap_sec);
	if (strcmp (ap_sec, "open" )==0)
		return 0;
	if (strcmp (ap_sec, "wep" )==0)
		return 1;
	/*if (strcmp (ap_sec, "WPA" )==0)
		return 2;*/
	return 2;
}
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
static int ParseWifiConfigString(unsigned char * strDataIn, nxcNetwork_st *wifiConfig)
{
    int retVal = 0;
    unsigned char lenSSID, lenKey, lenIP, lenSubnet, lenGW, lenwp, lenUsrN, lenPassW;
    int data_len = strlen((char const *)strDataIn);
    int index, check_count = 0;
    char strInt[10];

    unsigned char * strData = strDataIn;
    if (wifiConfig == NULL)
        return -1;
    memset((char*)wifiConfig, 0, sizeof(nxcNetwork_st));

    index = 0;
    //! get wifi mode
    switch (strData[index])
    {
    case '0':
        wifiConfig->NetworkType = 0;
        break;
    case '1':
        wifiConfig->NetworkType = 1;
        break;
    default:
        wifiConfig->NetworkType = 2;
        break;
    }
    index++;
    if (index > data_len)
    {
        return -1;
    }
    //! get channel to set in adhoc mode
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    wifiConfig->Channel = atoi(strInt);
    if ((wifiConfig->Channel <= 0) || (14 < wifiConfig->Channel))
    {
        wifiConfig->Channel = WIFI_DEFAULT_ADHOC_CHANNEL;//default channel
    }
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get authentication mode
    switch (strData[index++])
    {
    case '0':
    //                        wifiConfig->AuthenType = NETWORK_AUTH_NONE;
    //                       break;
    case '1':
        wifiConfig->AuthenType = 1;
        break;
    case '2':
        wifiConfig->AuthenType = 2;
        break;

    default:
        wifiConfig->AuthenType = 3;
        break;
    }
    if (index > data_len)
    {
        return -1;
    }
    //get keyindex if in open or shared mode
    if ((wifiConfig->AuthenType == 1))
    {
        switch (strData[index]) {
        case '0':
            wifiConfig->KeyIndex = 0;
            break;
        case '1':
            wifiConfig->KeyIndex = 1;
            break;
        case '2':
            wifiConfig->KeyIndex = 2;
            break;
        case '3':
            wifiConfig->KeyIndex = 3;
            break;
        default:
            break;
        }
    }
    index++;
    if (index > data_len)
    {
        return -1;
    }
    //get ip mode
    switch (strData[index++])
    {
    case '0':
        wifiConfig->IPMode = 0;
        break;
    case '1':
        wifiConfig->IPMode = 1;
        break;
    default:
        break;
    }
    if (index > data_len)
    {
        return -1;
    }
    //get length ssid
    strncpy(strInt, (char const *)strData + index, 3);
    strInt[3] = '\0';
    lenSSID = atoi(strInt);
    index += 3;
    if (index > data_len)
    {
        return -1;
    }
    //get length key
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenKey = atoi(strInt);
    if (lenKey == 0)
        wifiConfig->AuthenType = 0;
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get length ip
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenIP = atoi(strInt);
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get length subnet mask
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenSubnet = atoi(strInt);
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get length default gateway
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenGW = atoi(strInt);
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get length working port
    strncpy(strInt, (char const *)strData + index, 1);
    strInt[1] = '\0';
    lenwp = atoi(strInt);
    index += 1;
    if (index > data_len)
    {
        return -1;
    }
    //get length username
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenUsrN = atoi(strInt);
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //get length password
    strncpy(strInt, (char const *)strData + index, 2);
    strInt[2] = '\0';
    lenPassW = atoi(strInt);
    index += 2;
    if (index > data_len)
    {
        return -1;
    }
    //! Check data available
    check_count = index + lenSSID + lenKey + lenIP + lenSubnet + lenGW + lenwp + lenUsrN + lenPassW;
    //UART_PRINT("Dump len: lenSSID=%d + lenKey=%d + lenIP=%d + lenSubnet=%d + lenGW=%d + lenwp=%d+ lenUsrN=%d + lenPassW=%d\n",
    //    lenSSID,lenKey,lenIP,lenSubnet,lenGW,lenwp,lenUsrN,lenPassW);
    if (check_count != data_len)
    {
        UART_PRINT("Error->data_len=%d,correct_len=%d\n", data_len, check_count);
        return -1;
    }
    //get string ssid
    strncpy((char *)wifiConfig->ESSID, (char const *) strData + index, lenSSID);
    wifiConfig->ESSID[lenSSID] = '\0';
    index += lenSSID;
    //get string key
    strncpy((char *)wifiConfig->Key, (char const *) strData + index, lenKey);
    wifiConfig->Key[lenKey] = '\0';
    index += lenKey;
    //! Ignore if DHCP
    if (wifiConfig->IPMode == 1) {
        strncpy((char *)wifiConfig->DefaultIP, (char const *)strData + index, lenIP);
        wifiConfig->DefaultIP[lenIP] = '\0';
        index += lenIP;
        strncpy((char *)wifiConfig->SubnetMask, (char const *)strData + index, lenSubnet);
        wifiConfig->SubnetMask[lenSubnet] = '\0';
        index += lenSubnet;
        strncpy((char *)wifiConfig->Gateway, (char const *)strData + index, lenGW);
        wifiConfig->Gateway[lenGW] = '\0';
        index += lenGW;
        if (lenwp != 0) {
            //strncpy(wifiConfig->WorkPort, strData+index,lenwp);
            //wifiConfig->WorkPort[lenwp] = '\0';
        }
        else {
            //strcpy(wifiConfig->WorkPort,"80");
        }
        index += lenwp;
    }
    else {
        //strcpy(wifiConfig->WorkPort,"80");
        index += lenIP + lenSubnet + lenGW + lenwp;
    }

    if (wifiConfig->NetworkType == 1) {
        //strncpy(wifiConfig->Username, strData+index,lenUsrN);
        //wifiConfig->Username[lenUsrN] = '\0';
        index += lenUsrN;
        //strncpy(wifiConfig->Password, strData+index,lenPassW);
        //wifiConfig->Password[lenPassW] = '\0';
        index += lenPassW;
    }
    else {
        index += lenUsrN + lenPassW;
    }
    //! Dump

    return retVal;
}
void set_wireless_save(char *wireless)
{
    #if 1
    if (wireless)
    {
        nxcNetwork_st wifiConfig;
        UART_PRINT("String=%s\r\n", wireless);
        ParseString(wireless);
        UART_PRINT("String_affter parse=%s\r\n", wireless);
        ParseWifiConfigString((unsigned char *)wireless, &wifiConfig);
        UART_PRINT("SSID=%s,Key=%s,IP=%s,Mask=%s,Mask=%s,GW=%s\r\n", wifiConfig.ESSID,wifiConfig.Key,wifiConfig.DefaultIP,wifiConfig.SubnetMask,wifiConfig.Gateway);
        set_wireless_config(wifiConfig.ESSID, wifiConfig.Key, NULL);
    }
    #else
    // NOTE: ERROR: FIXME: DKS: hardcode here
    // set_wireless_config("Hubble-Mir", "12345678@", NULL);
    set_wireless_config("Hubble-QA11", "qa13579135", NULL);
    #endif
}
int32_t set_wireless_config(char *ssid, char *pass, char *auth_type) {
    // auth_type not used
    if (ssid == NULL || pass == NULL) {
        return -1;
    }
    // URL decoder
    ParseString(ssid);
    ParseString(pass);
    UART_PRINT("SSID:%s, PASS:%s, SEC:%s\r\n", ssid, pass, auth_type);

    memset(ap_ssid, 0, MAX_AP_SSID_LEN);
    memset(ap_pass, 0, MAX_AP_PASS_LEN);
    memset(ap_sec, 0, MAX_AP_SEC_LEN);
    memcpy(ap_ssid, ssid, strlen((char const *)ssid));
    memcpy(ap_pass, pass, strlen((char const *)pass));
    memcpy(ap_sec, auth_type, strlen((char const *)auth_type));

    return 0;
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
		UART_PRINT("\r\nSet cam default: flicker %d UD %d LR %d brate %d kbps framerate %d resolution %d\r\n", cam_settings[0],cam_settings[1],cam_settings[2],cam_settings[3]*256+cam_settings[4],cam_settings[5],cam_settings[6]*256+cam_settings[7]);
		userconfig_set(cam_settings, CAM_LEN, CAM_ID);
		userconfig_flash_write();
	}
	else
	{
		UART_PRINT("\r\nSetting ret=%d: flicker %d UD %d LR %d brate %d kbps framerate %d resolution %d\r\n", ret_val, cam_settings[0],cam_settings[1],cam_settings[2],cam_settings[3]*256+cam_settings[4],cam_settings[5],cam_settings[6]*256+cam_settings[7]);
	}
	return 0;
}


void set_server_authen(char *api_key, char *timezone)
{
    memcpy(apiKey, api_key, strlen(api_key));
    memcpy(timeZone, timezone, strlen(timezone));
}
int set_url(char *api, char *mqtt, char *ntp, char *rms, char *stun)
{
	int ret_val = 0;

	memset(MYCIN_API_HOST_NAME, 0, strlen(MYCIN_API_HOST_NAME));
	memset(MYCIN_HOST_NAME, 0, strlen(MYCIN_HOST_NAME));
	memset(g_acSNTPserver_single, 0, strlen(g_acSNTPserver_single));
	memset(relay_url_ch, 0, strlen(relay_url_ch));
	memset(SERVER_URL, 0, strlen(SERVER_URL));

	ParseString(api);
	ParseString(mqtt);
	ParseString(ntp);
	ParseString(rms);
	ParseString(stun);

/***********************************start set api down flash**********************************/

	memcpy(MYCIN_API_HOST_NAME, api, strlen(api));
	ret_val = userconfig_set(MYCIN_API_HOST_NAME, strlen(MYCIN_API_HOST_NAME), API_URL);
	if(ret_val < 0)
	{
		return -1;
	}
	else
	{
		UART_PRINT("API %s\r\n",MYCIN_API_HOST_NAME);
	}
/*********************************************************************************************/


/************************************start set mqtt down flash********************************/

	memcpy(MYCIN_HOST_NAME, mqtt, strlen(mqtt));
	ret_val = userconfig_set(MYCIN_HOST_NAME, strlen(MYCIN_HOST_NAME), MQTT_URL);
	if(ret_val < 0)
	{
		return -2;
	}
	else
	{
		UART_PRINT("MQTT %s\r\n",MYCIN_HOST_NAME);
	}
/*********************************************************************************************/


/***********************************start set ntp down flash**********************************/

	memcpy(g_acSNTPserver_single, ntp, strlen(ntp));
	ret_val = userconfig_set(g_acSNTPserver_single, strlen(g_acSNTPserver_single), NTP_URL);
	if(ret_val < 0)
	{
		return -3;
	}
	else
	{
		UART_PRINT("g_acSNTPserver_single %s\r\n",g_acSNTPserver_single);
	}
/*********************************************************************************************/


/*************************************start set rms down flash*******************************/

	memcpy(relay_url_ch, rms, strlen(rms));
	ret_val = userconfig_set(relay_url_ch, strlen(relay_url_ch), RMS_URL);
	if(ret_val < 0)
	{
		return -4;
	}
	else
	{
		UART_PRINT("relay_url_ch %s\r\n",relay_url_ch);
	}
/*********************************************************************************************/


/*************************************start set stun down flash******************************/

	memcpy(SERVER_URL, stun, strlen(stun));
	ret_val = userconfig_set(SERVER_URL, strlen(SERVER_URL), STUN_URL);
	if(ret_val < 0)
	{
		return -5;
	}
	else
	{
		UART_PRINT("SERVER_URL %s\r\n",SERVER_URL);
	}
/*********************************************************************************************/
	userconfig_flash_write();
	return 0;
}

int get_url(char *rece_api, char *rece_mqtt, char *rece_ntp, char *rece_rms, char *rece_stun)
{
	 int  ret_val = 0, ret_val_default = 0;
	 memset(MYCIN_API_HOST_NAME, 0, strlen(MYCIN_API_HOST_NAME));
	 memset(MYCIN_HOST_NAME, 0, strlen(MYCIN_HOST_NAME));
	 memset(g_acSNTPserver_single, 0, strlen(g_acSNTPserver_single));
	 memset(relay_url_ch, 0, strlen(relay_url_ch));
	 memset(SERVER_URL, 0, strlen(SERVER_URL));

/***************************************get_api***********************************************/
	 ret_val = userconfig_get(MYCIN_API_HOST_NAME, api_size, API_URL);
	 if(ret_val <= 0)
	 {
		  ret_val_default = 0;
		  set_url_default(ret_val_default);
		  userconfig_get(MYCIN_API_HOST_NAME, api_size, API_URL);
	 }
	 UART_PRINT("GET_API_VALUE %d\r\n", ret_val);
	 memcpy(rece_api, MYCIN_API_HOST_NAME, strlen(MYCIN_API_HOST_NAME));

/*********************************************************************************************/


/**************************************get_mqtt***********************************************/
	 ret_val = userconfig_get(MYCIN_HOST_NAME, mqtt_size, MQTT_URL);
	 if (ret_val <= 0)
	 {
		 ret_val_default = 1;
		 set_url_default(ret_val_default);
		 userconfig_get(MYCIN_HOST_NAME, mqtt_size, MQTT_URL);
	 }
	 UART_PRINT("GET_MQTT_VALUE: %d\r\n", ret_val);
	 memcpy(rece_mqtt, MYCIN_HOST_NAME, strlen(MYCIN_HOST_NAME));

/*******************************************************************************************/


/**************************************get_ntp*********************************************/

	 ret_val = userconfig_get(g_acSNTPserver_single, ntp_size, NTP_URL);
	 if (ret_val <= 0)
	 {
		ret_val_default = 2;
		set_url_default(ret_val_default);
		userconfig_get(g_acSNTPserver_single, ntp_size, NTP_URL);
	 }
	 UART_PRINT("GET_NTP_VALUE: %d\r\n", ret_val);
	 memcpy(rece_ntp, g_acSNTPserver_single, strlen(g_acSNTPserver_single));

/******************************************************************************************/


/*************************************get_rms**********************************************/

	 ret_val = userconfig_get(relay_url_ch, rms_size, RMS_URL);
	 if (ret_val <= 0)
	 {
		ret_val_default = 3;
	 	set_url_default(ret_val_default);
	 	userconfig_get(relay_url_ch, rms_size, RMS_URL);
	 }
	 UART_PRINT("GET_RMS_VALUE: %d\r\n", ret_val);
	 memcpy(rece_rms, relay_url_ch, strlen(relay_url_ch));

/******************************************************************************************/


/*************************************get_stun*********************************************/

	  ret_val = userconfig_get(SERVER_URL, stun_size, STUN_URL);
	  if (ret_val <= 0)
	  {
		 ret_val_default = 4;
	  	 set_url_default(ret_val_default);
	  	 userconfig_get(SERVER_URL, stun_size, STUN_URL);
	  }
	  UART_PRINT("GET_STUN_VALUE: %d\r\n", ret_val);
	  memcpy(rece_stun,  SERVER_URL, strlen( SERVER_URL));
	  userconfig_flash_write();
/******************************************************************************************/
	return ;
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
			UART_PRINT("SET_SNTP_VALUE: %s\r\n", NTP_URL_DEFAULT);
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
		else
		{
			bool_to_write = 1;
		}
	}
	else
	{
		UART_PRINT("GET_API_VALUE %d %s\r\n", ret_val, MYCIN_API_HOST_NAME);
	}


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
		else
		{
			bool_to_write = 1;
		}
	}
	else
	{
		UART_PRINT("GET_MQTT_VALUE: %d %s\r\n", ret_val, MYCIN_HOST_NAME);
	}

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
		else
		{
			bool_to_write = 1;
		}
	}
	else
	{
		UART_PRINT("GET_NTP_VALUE: %d %s\r\n", ret_val, g_acSNTPserver);
	}

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
		{
			bool_to_write = 1;
		}
	}
	else
	{
		UART_PRINT("GET_RMS_VALUE: %d %s\r\n", ret_val, relay_url_ch);
	}

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
		else
		{
			bool_to_write = 1;
		}
	}
	else
	{
		UART_PRINT("GET_STUN_VALUE: %d %s\r\n", ret_val, SERVER_URL);
	}

	if (bool_to_write)
	{
		UART_PRINT("flash_write\r\n");
		userconfig_flash_write();
	}
	else
	{
		UART_PRINT("not flash_write\r\n");
	}
/******************************************************************************************/
	return;
}
void cam_settings_update()
{
	userconfig_set(cam_settings, CAM_LEN, CAM_ID);
	userconfig_flash_write();
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
    	sl_Stop(200);
    	UART_PRINT("MODE_PARING, error=%d\r\n", modeID);
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
extern char timeZone[];
void uart_print_config_log(int l_param, int l_ret){
	UART_PRINT("Config %d %d\r\n",l_param,l_ret);
}
int system_registration()
{
	int RetVal = 0, i = 0;
	UART_PRINT("Network init start\r\n");
	sl_WlanProfileDel(0xFF);
//    RetVal = ConfigureSimpleLinkToDefaultState();
//    if(RetVal < 0)
//    {
//        /* Handle Error */
//        UART_PRINT("Network Terminal - Couldn't configure Network Processor\n");
//        return(NULL);
//    }
    InitializeAppVariables();
    Network_IF_WifiSetMode(ROLE_STA);
    UART_PRINT("Network init done\r\n");
    Network_IF_provisionAP();
    UART_PRINT("Scan wifi done\r\n");
    //Re-Start SimpleLink in AP Mode
    RetVal = sl_Stop(0xFF);
    RetVal = Network_IF_WifiSetMode(ROLE_AP);
    if (RetVal < 0)
    {
        return REGISTRATION_ERROR_SETMODEAP;
    }

    RetVal = sl_NetAppStop(SL_NETAPP_HTTP_SERVER_ID);
    if (RetVal < 0)
    {
    	 return REGISTRATION_ERROR_STOPHTTP;
    }
    //Run APPS HTTP Server

    HttpServerInitAndRun(NULL);
#if 0
// Initialize AP security params
#define SSID                "Hubble-Mir"
#define SSID_KEY            "12345678@"
SlSecParams_t SecurityParams = {0};
SecurityParams.Key = (signed char*)SSID_KEY;
SecurityParams.KeyLen = strlen(SSID_KEY);
SecurityParams.Type = SL_SEC_TYPE_WPA;

//
// Connect to the Access Point
//
lRetVal = Network_IF_ConnectAP(SSID,SecurityParams);
if(lRetVal < 0)
{
   Report("Connection to AP failed\n\r");
   LOOP_FOREVER();
}

#else
	i = 0;
	RetVal = Network_IF_Connect2AP_static_ip(ap_ssid, ap_pass, 0);
	//ak_power_up();
	if (RetVal<0)
	{
		//osi_Sleep(2000);
		UART_PRINT("Connect to AP fail\n");
//		g_input_state=LED_STATE_NONE;
		return REGISTRATION_ERROR_CONNECTAP;
	}
#endif
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
           UART_PRINT("device_token %s\r\n",g_mycinUser.authen_token);
           // RESET VARIABLE
           memset(ps_ip, 0x00, P2P_PS_IP_LEN);
           ps_port = 0;
           memset(p2p_key_char, 0x00, P2P_KEY_CHAR_LEN);
           memset(p2p_rn_char, 0x00, P2P_RN_CHAR_LEN);

           while (p2p_init(1)<0)
           {
        	   ClockP_sleep(5000);
           }
           UART_PRINT("Register to P2P server --- DONE\r\n");
//           int ret_val = 0;
//           // store data to flash
           RetVal = userconfig_set(g_mycinUser.authen_token, strlen(g_mycinUser.authen_token), MASTERKEY_ID);
           if (RetVal < 0)
           {
               UART_PRINT("Cannot write authen_token, error=%d\r\n", RetVal );
           }
    	  RetVal = userconfig_set(ap_ssid, strlen(ap_ssid), WIFI_SSID_ID);
           if ( RetVal< 0)
           {
               UART_PRINT("Cannot write ap_ssid, error=%d\r\n", RetVal );
               uart_print_config_log(WIFI_SSID_ID,  RetVal);
           }
           RetVal = userconfig_set(ap_pass, strlen(ap_pass), WIFI_KEY_ID);
           if ( RetVal < 0)
           {
               UART_PRINT("Cannot write ap_pass, error=%d\r\n",RetVal );
               uart_print_config_log(WIFI_KEY_ID,  RetVal);
           }
           RetVal = userconfig_set(timeZone, strlen(timeZone), TIMEZONE_ID);
           if (RetVal < 0)
           {
               UART_PRINT("Cannot write timeZone, error=%d\r\n", RetVal);
           }
//           // p2p
           RetVal  = userconfig_set(ps_ip, P2P_PS_IP_LEN, P2P_SERVER_IP_ID);
           if (RetVal  < 0)
           {
               UART_PRINT("Cannot write ps_ip, error=%d\r\n",RetVal );
           }
           RetVal  = userconfig_set(str_ps_port, strlen(str_ps_port), P2P_SERVER_PORT_ID);
           if (RetVal < 0)
           {
               UART_PRINT("Cannot write ps_port, error=%d\r\n", RetVal );
           }
           RetVal  = userconfig_set(p2p_key_char, P2P_KEY_CHAR_LEN, P2P_KEY_ID);
           if (RetVal  < 0)
           {
               UART_PRINT("Cannot write p2p_key_char, error=%d\r\n",RetVal);
           }
           RetVal  = userconfig_set(p2p_rn_char, P2P_RN_CHAR_LEN, P2P_RAN_NUM_ID);
           if (RetVal  < 0)
           {
               UART_PRINT("Cannot write p2p_rn_char, error=%d\r\n",RetVal);
           }
//
//           ret_val = userconfig_set(mycinRegister.mqtt_clientid, MQTT_CLIENTID_LEN, MQTT_CLIENTID);
//           if (ret_val < 0) {
//               uart_print_config_log(MQTT_CLIENTID, ret_val);
//           }
//           ret_val = userconfig_set(mycinRegister.mqtt_user, MQTT_USER_LEN, MQTT_USER);
//           if (ret_val < 0) {
//               uart_print_config_log(MQTT_USER, ret_val);
//           }
//           ret_val = userconfig_set(mycinRegister.mqtt_pass, MQTT_PASS_LEN, MQTT_PASS);
//           if (ret_val < 0) {
//               uart_print_config_log(MQTT_PASS, ret_val);
//           }
//           ret_val = userconfig_set(mycinRegister.mqtt_topic, MQTT_TOPIC_LEN, MQTT_TOPIC);
//           if (ret_val < 0) {
//               uart_print_config_log(MQTT_TOPIC, ret_val);
//           }
//           ret_val = userconfig_set(mycinRegister.mqtt_servertopic, MQTT_SERVERTOPIC_LEN, MQTT_SERVERTOPIC_ID);
//           if (ret_val < 0) {
//               uart_print_config_log(MQTT_SERVERTOPIC_ID, ret_val);
//           }
//
//           ret_val = userconfig_set(ap_sec, strlen(ap_sec), WIFI_SEC_ID);
//           if (ret_val < 0) {
//               uart_print_config_log(WIFI_SEC_ID, ret_val);
//           }
//
//           // set register flag
//           ret_val = userconfig_set(NULL, DEV_REGISTERED, MODE_ID);
//           if (ret_val < 0) {
//               UART_PRINT("Cannot write boot flag, error=%d\r\n", ret_val);
//           }
//
//           // Update camera setting as well as other changes
//           cam_settings_update();
    }
//       else
//       {
//           UART_PRINT("fail to register\n");
//           RetVal = REGISTRATION_ERROR_REGPOSTFAIL;
//       }

	return 0;
}

int system_init(void)
{
	int32_t ret_val = 0;
	UART_PRINT("CC3200 version=%s\r\n", SYSTEM_VERSION);
    g_version_8char[0]=SYSTEM_VERSION[0];
    g_version_8char[1]=SYSTEM_VERSION[1];
    g_version_8char[2]='.';
    g_version_8char[3]=SYSTEM_VERSION[2];
    g_version_8char[4]=SYSTEM_VERSION[3];
    g_version_8char[5]='.';
    g_version_8char[6]=SYSTEM_VERSION[4];
    g_version_8char[7]=SYSTEM_VERSION[5];

    ak_version[0]=AK_VERSION[0];
    ak_version[1]=AK_VERSION[1];
    ak_version[2]='.';
    ak_version[3]=AK_VERSION[2];
    ak_version[4]=AK_VERSION[3];
    ak_version[5]='.';
    ak_version[6]=AK_VERSION[4];
    ak_version[7]=AK_VERSION[5];
    UART_PRINT("AK ver %s\r\n", ak_version);
	sl_Start(0, 0, 0);
	ClockP_sleep(2);
	userconfig_init();
	userconfig_flash_read();
	ret_val = userconfig_get(ap_ssid, MAX_AP_SSID_LEN, WIFI_SSID_ID);
	if (ret_val < 0)
	{
		UART_PRINT("Can not read WIFI SSID, error=%d\r\n", ret_val);
	}
	ret_val = userconfig_get(ap_pass, MAX_AP_PASS_LEN, WIFI_KEY_ID);
	if (ret_val < 0)
	{
		UART_PRINT("Can not read WIFI KEY, error=%d\r\n", ret_val);
	}
	ret_val = userconfig_get(ap_sec, MAX_AP_SEC_LEN, WIFI_SEC_ID);
	if (ret_val < 0)
	{
		UART_PRINT("Can not read WIFI SEC, error=%d\r\n", ret_val);
	}
    ret_val = userconfig_set(ap_sec, strlen(ap_sec), WIFI_SEC_ID);
    if (ret_val < 0)
    {
        uart_print_config_log(WIFI_SEC_ID, ret_val);
    }
	ret_val = userconfig_get(timeZone, 32, TIMEZONE_ID);
	timezone_s=atoi(timeZone);
	timezone_s=timezone_s*60*60;
	UART_PRINT("Ret %d, TIME ZONE is %s %d\r\n", ret_val, timeZone, timezone_s);
	get_url_default();
	get_cam_default();
	UART_PRINT("\r\nHOST_NAME = %s\r\n", MYCIN_API_HOST_NAME);
	get_url_default();
	get_cam_default();

    memset((char *)&g_mycinUser, 0, sizeof(g_mycinUser));
    ret_val = userconfig_get(g_mycinUser.authen_token, MAX_AUTHEN_TOKEN, MASTERKEY_ID);
    if (ret_val < 0)
    {
        UART_PRINT("Can not read authen_token\r\n");
        // FIXME: need return error
    }
    char device_udid[MAX_UID_SIZE] = {0};
    memset(device_udid,0,MAX_UID_SIZE);
    get_uid(device_udid);
    memcpy(g_mycinUser.device_udid, device_udid, strlen(device_udid));
    memcpy(g_mycinUser.user_token, g_mycinUser.authen_token, MAX_USER_TOKEN);
    get_mac_address(g_strMacAddr);


    UART_PRINT("mac addr: %s, udid: %s\r\n", g_strMacAddr, g_mycinUser.device_udid);
	ClockP_sleep(2);
//    osi_SyncObjSignal(&g_userconfig_init);
	return 0;
}
void mcuReboot(void)
{
    /* stop network processor activities before reseting the MCU */
    sl_Stop(SL_STOP_TIMEOUT );

#ifdef __MSP432P401R__
    MAP_ResetCtl_initiateSoftReset();
#elif  CC32XX
    UART_PRINT("CC3220 MCU reset request\r\n");
    /* Reset the MCU in order to test the bundle */
    PRCMHibernateCycleTrigger();
#endif
}
//int32_t ProcessRestartMcu()
//{
//	UART_PRINT("\n\n");
//	UART_PRINT("ProcessRestartMcu: reset the platform...\r\n");
//    Platform_Reset();
//    UART_PRINT("\n");
//    UART_PRINT("ProcessRestartMcu: platform does not support self reset\r\n");
//    UART_PRINT("ProcessRestartMcu: reset the NWP to rollback to the old image\r\n");
//    UART_PRINT("\n");
//    ProcessRestartRequest();       /* sl_Stop and sl_Start */
//    return 0;
//}
//int32_t ProcessRestartRequest(void)
//{
////    gIsWlanConnected = 0;
//
//    StartAsyncEvtTimer(2);
//
//    return 0;
//}
void StartAsyncEvtTimer(uint32_t duration)
{
    struct itimerspec value;

    /* Start timer */
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 0;
    value.it_value.tv_sec = duration;
    value.it_value.tv_nsec = 0;

    timer_settime(gTimer, 0, &value, NULL);
}
