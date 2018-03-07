SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/freertos/builds/CC3220S_LAUNCHXL/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

#OBJECTS = uart_term.obj main_freertos.obj cmd_parser.obj netapp_cmd.obj network_terminal.obj radio_tool.obj socket_cmd.obj str.obj transceiver_cmd.obj wlan_cmd.obj CC3220S_LAUNCHXL.obj
OBJECTS = uart_term.obj main_freertos.obj CC3220S_LAUNCHXL.obj network_terminal.obj network_if.obj hl_apconfig.obj cc3200_system.obj nxcGenUDID_24.obj base64.obj \
		httpserverapp.obj url_parser.obj webserver.obj sha1.obj HttpCore.obj HttpSocket.obj HttpDebug.obj HttpString.obj WebSockHandler.obj osi_freertos.obj \
		p2p_main.obj nonos.obj userconfig.obj sflash.obj system.obj
		
NAME = network_terminal

CFLAGS = -I../.. \
    -DCC32XX \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/CCS/ARM_CM3" \
    "-I$(KERNEL_BUILD)" \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --gen_func_subsections=on \
    --float_support=vfplib \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

LFLAGS = "-i$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    -lti/display/lib/display.aem4 \
    -lti/drivers/lib/drivers_cc32xx.aem4 \
    -lthird_party/fatfs/lib/fatfs.aem4 \
    --diag_suppress=10063 \
    "-i$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/net/wifi" \
    -lccs/rtos/simplelink.a \
    "-l$(KERNEL_BUILD)/ccs/freertos.lib" \
    -lti/devices/cc32xx/driverlib/ccs/Release/driverlib.a \
    ../../freertos/ccs/CC3220S_LAUNCHXL_FREERTOS.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(KERNEL_BUILD)/ccs/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)
	
uart_term.obj: ../../uart_term.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

#wlan_cmd.obj: ../../wlan_cmd.c
#	@ echo Building $@
#	@ $(CC) $(CFLAGS) $< --output_file=$@

CC3220S_LAUNCHXL.obj: ../../CC3220S_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

network_terminal.obj: ../../network_terminal.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

network_if.obj: ../../network_if.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
	

hl_apconfig.obj: ../../hl_apconfig.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
	
cc3200_system.obj: ../../cc3200_system.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

nxcGenUDID_24.obj: ../../nxcGenUDID_24.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
	
base64.obj.obj: ../../base64.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	

url_parser.obj: ../../url_parser.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	
	
webserver.obj: ../../webserver.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	
	
httpserverapp.obj: ../../httpserverapp.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
	
p2p_main.obj: ../../p2p_main.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	
	
 HttpCore.obj: ../../HttpCore.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	
	
 HttpSocket.obj : ../../HttpSocket.c 
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	

 HttpDebug.obj: ../../HttpDebug.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

 HttpString.obj: ../../HttpString.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

WebSockHandler.obj:../../WebSockHandler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

osi_freertos.obj:../../osi_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

base64.obj:../../base64.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
	
sha1.obj:../../sha1.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

nonos.obj:../../nonos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

userconfig.obj:../../userconfig.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@
 
sflash.obj:../../sflash.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@	
	
system.obj:../../system.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@



$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ccs/freertos.lib
	@ echo linking...
	
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ echo $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
