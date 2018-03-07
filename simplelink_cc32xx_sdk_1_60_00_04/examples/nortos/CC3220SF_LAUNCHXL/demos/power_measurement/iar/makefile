SIMPLELINK_CC32XX_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = uart_term.obj main_nortos.obj platform.obj power_measure_nortos.obj CC3220SF_LAUNCHXL.obj

NAME = power_measurement

CFLAGS = -I.. \
    -DCC32XX \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source" \
    -DNORTOS_SUPPORT \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --cpu=Cortex-M4 \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_cc32xx.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/fatfs.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/drivers/net/wifi/iar/nortos/simplelink.a" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/kernel/nortos/lib/nortos_cc32xx.arm4" \
    "$(SIMPLELINK_CC32XX_SDK_INSTALL_DIR)/source/ti/devices/cc32xx/driverlib/iar/Release/driverlib.a" \
    --config ../iar/CC3220SF_LAUNCHXL_NoRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4 \
    --redirect _Printf=_PrintfSmall \
    --redirect _Scanf=_ScanfSmall \
    --semihosting=iar_breakpoint

all: $(NAME).out

uart_term.obj: ../uart_term.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_nortos.obj: ../main_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

platform.obj: ../platform.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

power_measure_nortos.obj: ../power_measure_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

CC3220SF_LAUNCHXL.obj: ../CC3220SF_LAUNCHXL.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking...
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
