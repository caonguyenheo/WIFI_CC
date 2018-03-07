---
# i2secho

---

## Example Summary

Example that uses the I2S driver to echo back to the audio received from the
microphone over the speaker. This example shows how to initialize the I2S
driver in Issue/Reclaim mode with audio echo.

## Peripherals Exercised

* `Board_GPIO_LED0` - Indicates that the board was initialized within
`mainThread()`
* `Board_I2C0` - Used to configure the TI codec on the Audio booster pack
* `Board_I2S0` - Used to echo sounds received on the microphone

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find Board.html in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

* This example was designed to use the CC3200 Audio BoosterPack
(CC3200AUDBOOST).  The booster pack is required to successfully complete this
example.

* A pair of (headphones|speakers) are required to observe functionality.

## Example Usage

* Mount the CC3200 Audio BoosterPack.
    * The BoosterPack contains an onboard microphone. Optionally,
    an external microphone may be used.

* Connect headphones or speakers to the Audio BoosterPack.

* Run the example. `Board_GPIO_LED0` turns ON to indicate driver
initialization is complete.

* Sounds captured by the microphone are echoed.

> If you halt the target during execution of this example, and then
run again, the echoing will not resume. You will need to reload the program.

## Application Design Details

* Two threads, `rxEchoThread` and `txEchoThread`, are used to read data from the
microphone and write the data back to the speaker.

> The drivers by default are non-instrumented in order to limit code size.

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
