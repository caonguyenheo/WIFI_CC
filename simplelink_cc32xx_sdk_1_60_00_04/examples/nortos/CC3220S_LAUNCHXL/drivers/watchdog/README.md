## Example Summary

This application demonstrates how to use the Watchdog driver to cause a
reset.

## Peripherals Exercised

* `Board_GPIO_LED0`      - Indicator LED
* `Board_GPIO_BUTTON0`   - Used to control the application
* `Board_WATCHDOG0` - Timer to reset the device

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.


## Example Usage

* The application turns ON `Board_GPIO_LED0` to indicate driver initialization
is complete.

* `Board_GPIO_LED0` is toggled repeatedly until Board_GPIO_BUTTON0 is pressed.
A device reset occurs on the second watchdog timeout after `Board_GPIO_BUTTON0`
is pushed.

> What happens after the reset depends on whether there is a debugger attached
or not.

* In the debugger case, the reset will either cause the application to restart
or cause it to halt, depending on your device. Again depending on your device,
you can set a breakpoint at the beginning of `main` to see that the device was
reset after pressing the button.

* If the debugger is not in use and `Board_GPIO_BUTTON0` is pressed, there may
be a brief flicker in the blinking of `Board_GPIO_LED0` as the reset occurs.

## Application Design Details

* The application supplies a callback function to the Watchdog module which
is executed on every watchdog timer time-out (in watchdog interrupt context).
The callback repeatedly clears the watchdog timer, thus preventing a device
reset, and toggles `Board_GPIO_LED0`.

* When `Board_GPIO_BUTTON0` is pressed, the `mainThread` clears a `flag` used
by the callback. The callback sees the flag and stops calling
`Watchdog_clear()`.

* The `mainThread` performs the following actions:
  Determine if Board_GPIO_BUTTON0 had been pressed.  If so, clear the reset
flag;
  allow watchdog timer to reset device on next interrupt.

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.
