SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/freertos/builds/CC3220SF_LAUNCHXL/release

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = uart_term.obj main_freertos.obj bma222drv.obj link_local_task.obj ota_archive.obj ota_json.obj ota_task.obj out_of_box.obj platform.obj provisioning_task.obj tmp006drv.obj CC3220SF_LAUNCHXL.obj

NAME = out_of_box

CFLAGS = -I../.. \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/IAR/ARM_CM3" \
    "-I$(KERNEL_BUILD)" \
    --cpu=Cortex-M4 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/net/json/iar/Release/json.a" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc32xx.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/fatfs.arm4" \
    --threaded_lib \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/net/wifi/iar/rtos/simplelink.a" \
    "$(KERNEL_BUILD)/iar/freertos.lib" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/devices/cc32xx/driverlib/iar/Release/driverlib.a" \
    --config ../../freertos/iar/CC3220SF_LAUNCHXL_FREERTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4 \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

all: $(NAME).out

$(KERNEL_BUILD)/iar/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

uart_term.obj: ../../uart_term.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

bma222drv.obj: ../../bma222drv.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

link_local_task.obj: ../../link_local_task.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ota_archive.obj: ../../ota_archive.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ota_json.obj: ../../ota_json.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ota_task.obj: ../../ota_task.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

out_of_box.obj: ../../out_of_box.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

platform.obj: ../../platform.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

provisioning_task.obj: ../../provisioning_task.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

tmp006drv.obj: ../../tmp006drv.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

CC3220SF_LAUNCHXL.obj: ../../CC3220SF_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/iar/freertos.lib
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
