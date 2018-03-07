---
# uartpower

---

## Example Summary

UART application using power management. This example has one thread that
repeatedly writes a buffer out to the UART for a fixed number of times,
and then sleeps for 5 seconds.  While the thread is asleep, the processor
goes into a low power mode (eg, LPDS for CC32XX).  The power consumption
can be measured by connecting an ammeter to the appropriate pins on the
LaunchPad.

## Peripherals Exercised

* `Board_GPIO_LED0` - Used to indicate status.
* `Board_UART0` - Used to send data to the UART.

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.


## Example Usage

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
Homepage"), etc.) to the appropriate COM port.
    * The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

The connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

* Run the example.

* You should see a line containing the letters of the alphabet continually
written to the serial session for a few seconds, followed by a line that
says __Going to sleep for 5 seconds...__.  This repeats until you halt
the program.

> By connecting an ammeter to the appropriate jumper, you
can see the current usage drop while the thread is asleep.

## Application Design Details

> N/A

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
