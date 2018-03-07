#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ti/drivers/net/wifi/simplelink.h>
#include "cc3200_system.h"
#include "nxcGenUDID.h"

#define GM_INTERRUPT_GPIO            12  // GPIO 12, PIN 03

static char g_device_mac[MAX_MAC_ADDR_SIZE] = {0};
static char g_udid[MAX_UID_SIZE] = {0};
//static uint32_t g_gpio_port_addr = 0;
//static uint8_t g_gpio_pin_addr = 0;

int get_mac_address(char * str_mac_addr) {
	int ret_val;
	static uint8_t mac_addr_init = 0;
	uint8_t mac_addr[SL_MAC_ADDR_LEN];
	uint8_t macAddressLen = SL_MAC_ADDR_LEN;

	if (str_mac_addr == NULL) {
		return -1;
	}

	if (mac_addr_init == 0) {
		ret_val = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, NULL, &macAddressLen, (uint8_t *)mac_addr);
		if (ret_val != 0) {
			return ret_val;
		}

		ret_val = snprintf(g_device_mac, MAX_MAC_ADDR_SIZE, "%02X%02X%02X%02X%02X%02X",
		         mac_addr[0], mac_addr[1], mac_addr[2],
		         mac_addr[3], mac_addr[4], mac_addr[5]);
		if (ret_val <= 0) {
			return -1;
		}
		mac_addr_init = 1;
	}
	strcpy(str_mac_addr, g_device_mac);
	return 0;
}
// Set Mac Address
// str_mac_addr: input mac address
int set_mac_address(char *str_mac_addr) {
	int32_t i;
	int32_t ret_val = 0;
    uint8_t MAC_Address[6];
    uint8_t str_octet[3] = {0};
	if (str_mac_addr == NULL) {
		return -1;
	}

	if (strlen(str_mac_addr) < SL_MAC_ADDR_LEN * 2) {
		// input str mac addr invalid
		return -2;
	}

	// Convert str to hex
	// FFFFFFFFFFFF (12 bytes) -> 0xFFFFFFFFFFFF (6bytes)
	for (i = 0; i < SL_MAC_ADDR_LEN; i++) {
		memset(str_octet, 0x00, sizeof(str_octet));
		memcpy(str_octet, str_mac_addr + i*2, 2);
		ret_val = strtoul(str_octet, NULL, 16) & 0xFF;
		if (ret_val > 0xFF || ret_val < 0) {
			return -3;
		}
		MAC_Address[i] = ret_val;
	}

    ret_val = sl_NetCfgSet(SL_NETCFG_MAC_ADDRESS_SET, 1, SL_MAC_ADDR_LEN, MAC_Address);
    sl_Stop(0);
    sl_Start(NULL,NULL,NULL);

	return ret_val;	
}

int get_uid(char *uid) {
	static uint8_t uid_init = 0;

	if (uid == NULL) {
		return -1;
	}

	if (uid_init == 0) {
		char mac_addr[MAX_MAC_ADDR_SIZE] = {0};
		get_mac_address(mac_addr);
		memset(g_udid, 0, MAX_UID_SIZE);
		inxcGenUDID(VENDOR, TYPE_NUM, STR_MODEL, mac_addr, g_udid);
		uid_init = 1;
	}
	strncpy(uid, g_udid, strlen(g_udid));
	return 0;
}

int get_version(char *l_version) {
	l_version[0]=SYSTEM_VERSION[0];
	l_version[1]=SYSTEM_VERSION[1];
	l_version[2]='.';
	l_version[3]=SYSTEM_VERSION[2];
	l_version[4]=SYSTEM_VERSION[3];
	l_version[5]='.';
	l_version[6]=SYSTEM_VERSION[4];
	l_version[7]=SYSTEM_VERSION[5];
	l_version[8]=0;
	return 0;
}

//void set_gm_pin(uint8_t gpio_value) {
//	if (g_gpio_port_addr == 0 && g_gpio_pin_addr == 0) {
//		GPIO_IF_GetPortNPin(GM_INTERRUPT_GPIO, (unsigned int *)&g_gpio_port_addr
//							, &g_gpio_pin_addr);
//	}
//	GPIO_IF_Set(GM_INTERRUPT_GPIO, g_gpio_port_addr, g_gpio_pin_addr, gpio_value);
//}

//// #include "user_app_config.h"
//#define GPIO_08					8  // GPIO 08 - PIN 63
//#define GPIO_28					28 // GPIO 28 - PIN 18
//#define GPIO_12					12 // GPIO 12 - PIN ??
//#define AK_POWER_GPIO			GPIO_08
//#define AK_SPI_READ_INT			GPIO_28
//#define AK_BUTTON_LED			GPIO_12
//static cc_hndl tGPIO_AK_POWER_hndl = NULL;
//static cc_hndl tGPIO_AK_READ_hndl = NULL;
//static cc_hndl tGPIO_AK_LED_hndl = NULL;
///*
//void ak_init_gpio(void) {
//	if (tGPIO_AK_POWER_hndl == NULL) {
//		tGPIO_AK_POWER_hndl = cc_gpio_open(AK_POWER_GPIO, GPIO_DIR_OUTPUT);
//	}
//	if (tGPIO_AK_READ_hndl == NULL) {
//		tGPIO_AK_READ_hndl = cc_gpio_open(AK_SPI_READ_INT, GPIO_DIR_OUTPUT);
//	}
//	UART_PRINT("Init GPIO Done!!!!!\r\n");
//	// DkS: test
//	// cc_gpio_write(tGPIO_AK_POWER_hndl, AK_POWER_GPIO, 0);
//	// cc_gpio_write(tGPIO_AK_READ_hndl, AK_SPI_READ_INT, 0);
//}*/
//
//void ak_power_set(int val) {
//	if (tGPIO_AK_POWER_hndl == NULL) {
//		tGPIO_AK_POWER_hndl = cc_gpio_open(AK_POWER_GPIO, GPIO_DIR_OUTPUT);
//	}
//	if (val < 0 || val > 1) {
//		return;
//	}
//	cc_gpio_write(tGPIO_AK_POWER_hndl, AK_POWER_GPIO, val);
//}
//void ak_led1_set(int val) {
//	static int led1_val=-1;
//	if (tGPIO_AK_LED_hndl == NULL) {
//		tGPIO_AK_LED_hndl = cc_gpio_open(12, GPIO_DIR_OUTPUT);
//	}
//	if ((val < 0 || val > 1) || (val == led1_val)) {
//		return;
//	}
//	cc_gpio_write(tGPIO_AK_LED_hndl, 12, val);
//}
//
//extern int to_send_setting;
//void ak_power_up(void) {
//	UART_PRINT("AK POWER UP!!!!!\r\n");
//	ak_power_set(1);
//	to_send_setting = 1;
//}
//void ak_power_down(void) {
//	UART_PRINT("AK POWER DOWN!!!!!\r\n");
//	ak_power_set(0);
//	to_send_setting = 0;
//}
//void ak_set_read_gpio(char val) {
//	if (tGPIO_AK_READ_hndl == NULL) {
//		tGPIO_AK_READ_hndl = cc_gpio_open(AK_SPI_READ_INT, GPIO_DIR_OUTPUT);
//	}
//	UART_PRINT("AK READ INTERRUPT!!!!!\r\n");
//	if (val < 0 || val > 1) {
//		UART_PRINT("ERROR: AK READ INTERRUPT invalid val, %d\r\n", val);
//		return;
//	}
//	cc_gpio_write(tGPIO_AK_READ_hndl, AK_SPI_READ_INT, val);
//}
//void ak_read_interrupt(void) {
//	if (tGPIO_AK_READ_hndl == NULL) {
//		tGPIO_AK_READ_hndl = cc_gpio_open(AK_SPI_READ_INT, GPIO_DIR_OUTPUT);
//	}
//	UART_PRINT("AK READ INTERRUPT!!!!!\r\n");
//	cc_gpio_toggle(tGPIO_AK_READ_hndl, AK_SPI_READ_INT);
//}
