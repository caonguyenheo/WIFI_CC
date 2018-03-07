/*
 * This file was generated by linkcmd_gnu.xdt from the
 * ti.platforms.simplelink package.
 */

__STACK_SIZE = STACKSIZE;

__TI_STACK_SIZE = __STACK_SIZE;

INPUT(
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/builds/CC3220SF_LAUNCHXL/debug/gcc/package/cfg/debug_pm4g.om4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/ti/uia/sysbios/lib/release/ti.uia.sysbios.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/ti/uia/loggers/lib/release/ti.uia.loggers.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/builds/CC3220SF_LAUNCHXL/debug/gcc/package/cfg/debug_pm4g.src/sysbios/sysbios.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/ti/uia/services/lib/release/ti.uia.services.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/ti/uia/runtime/lib/release/ti.uia.runtime.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/ti/uia/events/lib/release/ti.uia.events.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/gnu/targets/arm/rtsv7M/lib/boot.am4g
    /media/sf_new_cc3220_60/simplelink_cc32xx_sdk_1_60_00_04/kernel/tirtos/packages/gnu/targets/arm/rtsv7M/lib/syscalls.am4g
)

/*
 * symbolic aliases for static instance objects
 */
xdc_runtime_Startup__EXECFXN__C = 1;
xdc_runtime_Startup__RESETFXN__C = 1;


SECTIONS {

        .bootVecs (DSECT) : {*(.bootVecs)} 
    xdc.meta (COPY) : {KEEP(*(xdc.meta))}  AT> REGION_TEXT

    /*
     * Linker command file contributions from all loaded packages:
     */
    
/* Content from ti.sysbios.family.arm (ti/sysbios/family/arm/linkcmd.xdt): */

/* Content from ti.sysbios.family.arm.m3 (ti/sysbios/family/arm/m3/linkcmd.xdt): */
    _intvecs_base_address = 16779264;
    _vtable_base_address = 536870912;
ti_sysbios_family_arm_m3_Hwi_nvic = 0xe000e000;

/* Content from ti.sysbios.rts (ti/sysbios/rts/linkcmd.xdt): */

/* Content from ti.sysbios.family.arm.cc32xx (ti/sysbios/family/arm/cc32xx/linkcmd.xdt): */


    __TI_STACK_BASE = __stack;
}

ENTRY(_c_int00)

/* function aliases */
xdc_runtime_System_asprintf_va__E = xdc_runtime_System_asprintf_va__F;
xdc_runtime_System_snprintf_va__E = xdc_runtime_System_snprintf_va__F;
xdc_runtime_System_printf_va__E = xdc_runtime_System_printf_va__F;
xdc_runtime_System_aprintf_va__E = xdc_runtime_System_aprintf_va__F;
xdc_runtime_System_sprintf_va__E = xdc_runtime_System_sprintf_va__F;