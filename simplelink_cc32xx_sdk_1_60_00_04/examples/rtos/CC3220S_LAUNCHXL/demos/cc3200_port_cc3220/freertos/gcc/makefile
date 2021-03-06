SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/freertos/builds/CC3220S_LAUNCHXL/release

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

#OBJECTS = uart_term.obj main_freertos.obj cmd_parser.obj netapp_cmd.obj network_terminal.obj radio_tool.obj socket_cmd.obj str.obj transceiver_cmd.obj wlan_cmd.obj CC3220S_LAUNCHXL.obj
OBJECTS = uart_term.obj main_freertos.obj CC3220S_LAUNCHXL.obj network_terminal.obj network_if.obj hl_apconfig.obj cc3200_system.obj nxcGenUDID_24.obj base64.obj \
		httpserverapp.obj url_parser.obj webserver.obj sha1.obj HttpCore.obj HttpSocket.obj HttpDebug.obj HttpString.obj WebSockHandler.obj osi_freertos.obj \
		p2p_main.obj nonos.obj userconfig.obj sflash.obj system.obj

NAME = network_terminal

CFLAGS = -I../.. \
    -DCC32XX \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/posix/gcc" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM3" \
    "-I$(KERNEL_BUILD)" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=soft \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,-T,../../freertos/gcc/CC3220S_LAUNCHXL_FREERTOS.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    -l:ti/display/lib/display.am4g \
    -l:ti/drivers/lib/drivers_cc32xx.am4g \
    -l:third_party/fatfs/lib/fatfs.am4g \
    "-L$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/net/wifi" \
    -l:gcc/rtos/simplelink.a \
    "-L$(KERNEL_BUILD)/gcc" \
    -l:freertos.lib \
    -l:ti/devices/cc32xx/driverlib/gcc/Release/driverlib.a \
    -march=armv7e-m \
    -mthumb \
    -nostartfiles \
    -static \
    -Wl,--gc-sections \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

$(KERNEL_BUILD)/gcc/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

uart_term.obj: ../../uart_term.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

CC3220S_LAUNCHXL.obj: ../../CC3220S_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

network_terminal.obj: ../../network_terminal.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

network_if.obj: ../../network_if.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

hl_apconfig.obj: ../../hl_apconfig.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

cc3200_system.obj: ../../cc3200_system.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

nxcGenUDID_24.obj: ../../nxcGenUDID_24.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

base64.obj: ../../base64.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

url_parser.obj: ../../url_parser.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

webserver.obj: ../../webserver.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

httpserverapp.obj: ../../httpserverapp.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@


p2p_main.obj: ../../p2p_main.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@	
	
HttpCore.obj: ../../HttpCore.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@
	
HttpSocket.obj : ../../HttpSocket.c 
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

HttpDebug.obj: ../../HttpDebug.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

HttpString.obj: ../../HttpString.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@
	
WebSockHandler.obj:../../WebSockHandler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

osi_freertos.obj:../../osi_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

#base64.obj:../../base64.c
#	@ echo Building $@
#	@ $(CC) $(CFLAGS) $< --output_file=$@
	
sha1.obj:../../sha1.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

nonos.obj:../../nonos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

userconfig.obj:../../userconfig.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@
 
sflash.obj:../../sflash.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@
	
system.obj:../../system.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@


$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/gcc/freertos.lib
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
