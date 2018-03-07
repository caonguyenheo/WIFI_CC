---
# cryptohmac

---

## Example Summary

This example demonstrates the use of the Cryptographic HMAC Hash functions.

## Peripherals Exercised

* `Board_UART0` - UART 0

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.


## Example Usage

* Open a serial session (e.g. `HyperTerminal`,`puTTY`, etc.) to the appropriate
COM port.
    * The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

The connection should have the following settings:
```
    Baud-rate:    115200
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
```

* Run the example.

The example starts by showing on the terminal the following Hash types options:
```
    1) for HMAC MD5
    2) for HMAC SHA1
    3) for HMAC SHA224
    4) for HMAC SHA256
    5) for MD5
    6) for SHA1
    7) for SHA224
    8) for SHA256
```

* Select a Hash type by pressing the appropriate number.

* The example proceeds according to the selected Hash type:
    - HMAC hash will asks for pre defined key or user specified key; then asks
for message
    - Hash type will ask for message

* The example shows the generated Hash value and then indicates verification

## Application Design Details

* This example shows how to initialize the Crypto CC32XX driver, sign and
verify a signature

* The example is a reference to usage of SHAMD5 DriverLib functions on CC32xx.

* Developer/User can refer to this simple example and re-use the functions in
their applications.

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
