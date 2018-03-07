SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/freertos/builds/CC3220SF_LAUNCHXL/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = uart_term.obj main_freertos.obj cmd_parser.obj netapp_cmd.obj network_terminal.obj radio_tool.obj socket_cmd.obj str.obj transceiver_cmd.obj wlan_cmd.obj CC3220SF_LAUNCHXL.obj

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
    ../../freertos/ccs/CC3220SF_LAUNCHXL_FREERTOS.cmd \
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

cmd_parser.obj: ../../cmd_parser.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

netapp_cmd.obj: ../../netapp_cmd.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

network_terminal.obj: ../../network_terminal.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

radio_tool.obj: ../../radio_tool.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

socket_cmd.obj: ../../socket_cmd.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

str.obj: ../../str.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

transceiver_cmd.obj: ../../transceiver_cmd.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

wlan_cmd.obj: ../../wlan_cmd.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

CC3220SF_LAUNCHXL.obj: ../../CC3220SF_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ccs/freertos.lib
	@ echo linking...
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
