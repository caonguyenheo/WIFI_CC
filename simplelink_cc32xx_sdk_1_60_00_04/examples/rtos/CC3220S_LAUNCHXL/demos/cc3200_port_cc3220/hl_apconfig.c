#include <string.h>
#include <stdio.h>

#include "hl_apconfig.h"
#include <ti/drivers/net/wifi/simplelink.h>
#include "network_terminal.h"
//#include "simplelink.h"
//#include "uart_if.h"
// #include "system.h"
#include "cc3200_system.h"
#include "uart_term.h"


//!****************************************************************************
//! set ap name
//!
//! \param[in] dev_name         device name - product name
//! \param[in] mac_addr         mac address of device
//!
//! \return    0 if success, others for failure.
//!****************************************************************************
extern char DoorBell_name[];
//char DoorBell_name[];
int32_t
hl_ap_setname(char *dev_name, char *mac_addr)
{
    int32_t ret_val = -1;
    char ssid[MAX_SSID_LEN+1];

    if ((NULL == dev_name) || (NULL == mac_addr))
        return -1;

    // name-mac
    if (strlen(dev_name) + strlen(mac_addr) + 1 > MAX_SSID_LEN)
        return -1;

    int32_t mac_pos = strlen(mac_addr) - 6;
    if (mac_pos < 0)
        return -1;

    // join product name into ssid
    snprintf(ssid, sizeof(ssid), "%s%s%s", dev_name, STR_MODEL, &mac_addr[mac_pos]);
    sprintf( DoorBell_name, "%s%s%s", dev_name, STR_MODEL, &mac_addr[mac_pos]);
    UART_PRINT("SSID: %s\n", ssid);
    // call sl
    ret_val = sl_WlanSet(SL_WLAN_CFG_AP_ID, SL_WLAN_AP_OPT_SSID, strlen(ssid),
                         (unsigned char*)ssid);
    if (ret_val < 0) {
        return ret_val;
    }
    // restart network processor
    ret_val = sl_Stop(SL_STOP_TIMEOUT);
    return sl_Start(NULL, NULL, NULL);
}

//!****************************************************************************
//! set ap ip address
//!
//! \param[in] ip               ip to set
//!
//! \return    0 if success, others for failure.
//!****************************************************************************
int32_t
hl_ap_setip(int ip)
{
    int32_t ret_val = -1;
    SlNetCfgIpV4Args_t ipv4;
    ipv4.Ip = ip;
    ipv4.IpMask = DEF_SUBNET_MASK;
    ipv4.IpGateway = DEF_AP_IP_ADDR ;   //(_u32)SL_IPV4_VAL(192,168,193,1);
    ipv4.IpDnsServer = DEF_AP_IP_ADDR;   //(_u32)SL_IPV4_VAL(192,168,193,1);

    ret_val = sl_NetCfgSet(SL_NETCFG_IPV4_AP_ADDR_MODE,
                           4,
                           sizeof(SlNetCfgIpV4Args_t),
                           (uint8_t *)&ipv4);
    if (ret_val < 0) {
        return ret_val;
    }
    // restart network processor
    ret_val = sl_Stop(SL_STOP_TIMEOUT);

    return sl_Start(NULL, NULL, NULL);
}

//!****************************************************************************
//! set ap dhcp client ip range
//!
//! \param[in] ip_start       start of ip range
//! \param[in] ip_end         end of ip range
//!
//! \return    0 if succ    ess, others for failure.
//!
//! \note      client ip range must be in subnet of AP IP address
//!****************************************************************************
int32_t
hl_ap_dhcp_range(int ip_start, int ip_end)
{
    int32_t ret_val = -1;
    SlNetAppDhcpServerBasicOpt_t dhcp_params;
    dhcp_params.lease_time = DHCP_LEASE_TIME;
    dhcp_params.ipv4_addr_start = ip_start;        //SL_IPV4_VAL(192,168,193,2);
    dhcp_params.ipv4_addr_last = ip_end;   ///SL_IPV4_VAL(192,168,193,254);
    sl_NetAppStop(SL_NETAPP_DHCP_SERVER_ID);
    ret_val = sl_NetAppSet(SL_NETAPP_DHCP_SERVER_ID,
                           SL_NETAPP_DHCP_SRV_BASIC_OPT,
                           sizeof(SlNetAppDhcpServerBasicOpt_t),
                           (uint8_t *)&dhcp_params);
    if (ret_val < 0) {
        return ret_val;
    }
    return sl_NetAppStart(SL_NETAPP_DHCP_SERVER_ID);
}


//!****************************************************************************
//! Configure for device in AP role, include SSID name, IP, DHCP IP range
//!
//! \param     None
//!
//! \return    0 if success, others for failure.
//!****************************************************************************
int32_t
hl_ap_config()
{
    int32_t ret_val = -1;
    char mac_addr[MAX_MAC_ADDR_SIZE] = {0}, udid[MAX_UID_SIZE] = {0};
    ClockP_sleep(6);
        // get device mac address
    get_mac_address(mac_addr);
    get_uid(udid);
    UART_PRINT("***udid***: %s\r\n", udid);
    ret_val = hl_ap_setname(PRODUCT_NAME, mac_addr);
    if (ret_val < 0)
    {
        UART_PRINT("Set AP name failed \n");
        return ret_val;
    }
    // set AP IP
    ret_val = hl_ap_setip(DEF_AP_IP_ADDR);
    if (ret_val < 0) {
        UART_PRINT("Set AP IP address failed \n");
        return ret_val;
    }
    // set AP DHCP IP range
    ret_val = hl_ap_dhcp_range(DHCP_IP_START, DHCP_IP_END);
    if (ret_val < 0) {
        UART_PRINT("Set AP DHCP failed \n");
        return ret_val;
    }
    return 0;
}

