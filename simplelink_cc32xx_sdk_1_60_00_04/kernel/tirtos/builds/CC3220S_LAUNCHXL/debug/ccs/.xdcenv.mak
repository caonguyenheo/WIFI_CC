#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/source;/media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages
override XDCROOT = /home/heo/ti/xdctools_3_50_03_33_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/source;/media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages;/home/heo/ti/xdctools_3_50_03_33_core/packages;..
HOSTOS = Linux
endif
