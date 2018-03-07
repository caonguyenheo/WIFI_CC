## Example Summary

This example demonstrate the local Over the Air (OTA) on CC3220 devices.

## Peripherals Exercised

* `Board_LED0 (red LED, D7 on the board)` - Indicates that the board was initialized within `main()`
and while blinking, indicates that the board is trying to connect to AP.
Upon successful connection and IP address acquisition, the LED should turn solidly on.

* `Board_LED2 (green LED, D5 on the board)` - Indicates that the board was initialized within `main()`
and while blinking, indicates that the board is pinging the connected AP.
Upon successful ping, the LED should turn solidly on.


## Example Usage

* Downloads and Installations.
	> Make sure you have Uniflash installed and Code Composer Studio or IAR IDE.

* Checklist of required elements.
	- archive tar file for device update. Filename should begin with 14 digits date and suffix should be uncompressed tar.
	- Uniflash projects according to device flavor.	
	- Certificate artifacts, ca.der certificate and ca-priv-key.der private key.

* Build the setup.
	> Make sure you have the a local AP and a PC/mobile/tablet with the archive tar file for device update.

* Open the *local\_ota.h* file and configure your network credentials.
	> By default, CC3220 tries to connect to a predefined open AP (cc3220demo).
Set values for the: __SSID\_NAME__, __SECURITY\_TYPE__ and __SECURITY\_KEY__ defines if you like to have non default parameters.

* Build the project and flash it by using the SimpleLink Uniflash utility.
	> The example could be executed from the debugger as well but since it involves platform reset at the end (so the new OTA image can be tested and committed),the debugger would get disconnected.

* Open a serial session (e.g. `HyperTerminal`,`puTTY`, etc.) to the appropriate COM port.
	> The COM port can be determined via Device Manager in Windows or via `ls /dev/tty*` in Linux.

The connection should have the following settings
```
    Baud-rate:    115200
    Data bits:    8
    Stop bits:    1
    Parity:       None
    Flow Control: None
```

* Run the example by pressing the reset button. The example should start running automatically when programming via Uniflash is done.

	*  `Board_LED0 (red LED, D7 on the board)` should blink once every second to indicate connection to AP is in progress. When connection is completed and IP address is acquired, the LED should turn solid on. Upon successful connection, terminal message should show: 
		- `[WLAN EVENT] STA connected to the AP:` with SSID and BSSID information
		- `[NETAPP EVENT] IP Acquired:` with IP address and Gateway information 
	*  `Board_LED2 (green LED, D5 on the board)` should blink once every second to indicate pinging the AP is in progress. When pinging is completed successfully, the LED should turn solid on. Upon successful ping, terminal message should show: 
		- `[Local ota task] pinging to gateway succeeded`
		- `[Local ota task] waiting for new ota upload...`

* At this point, local OTA is enabled and may be triggered.

* Users can navigate to the demo tab in two ways:
	- Explicitly type ota.html as the requested resource on the browser URL. The requested resource must be prefixed by CC3220 IP address.
	- Via the drop down menu selector on the upper left side of the screen.
	> Since the web connection is secured via HTTPS and since self-signed certificate is used, browsers may indicate that the connection is not private (suggesting that attackers may try to steal information). In this case, choose to proceed and load the page. The https prefix on the browser should appear as strikethrough. Clicking the start button automatically loads the main OTA page.

* The procedure to update a new version is as follows:
	> During OTA procedure, `Board_LED2 (green LED, D5 on the board)` should blink rapidly and turn solidly on if the OTA procedure is successful.
	- Click the *Update* button to initialize the SW update.
	- Click the *Choose file* button and pick the archive tar file.
	> archive tar file according to the connected CC3220 flavor should be picked.
	- Click the *Upload file* button to start the process.
	- See the upload progress bar appear with some textual messages to describe the procedure.
		>		- Uploading new SW version
		>		- Extracting archive file
		>		- Writing to serial flash
		>
		>		- Rebooting…
		>		- Testing new SW version
	- Upon successful update, `CC3220 is successfully updated` message should appear. 
	- Click the *Finalize* button to end the procedure. The new version should appear under `Current SW Version`. Terminal message should show: 
		- `[Link local task] sl_extLib_OtaRun: ---- Download file completed`
		- `[Common] CC3220 MCU reset request`
 

## Application Design Details

This example demonstrates local Over-the-Air update experience with CC3220 LaunchPad development kit.
Over-the-Air update demonstrates an update of a full image over a secured channel. OTA service enables in-system update of the MCU application, CC3220 firmware releases (a.k.a. Service Pack) made available by Texas Instruments and other vendor files. An update procedure executed in a full system integrity fashion, i.e. failure in upgrading any of the image components would lead to rolling back to the previous valid version.

> Please install the latest CC3220 Service Pack.

## Known issues and limitations

- Internet Explorer 11 browser does not work in its default state. In order to make it work, possible workaround is to use development mode (F12), and switch the Edge drop down menu on the top right to 10 instead of default.
- Up to 20 non-default files can be updated during Over the Air update. Default files include files that are non-secured, part of bundle, failsafe and with the size of the original file.
- Maximal file length inside a tar is 99 bytes. It is the full path as appears in the tar file itself (including all directories from root).
- Rarely, it is observed that the progress bar does not start or start but freeze in the middle; however the Over the Air update procedure is successful. In these cases, `Board_LED2 (green LED, D5 on the board)` indication and terminal printouts would show the true status of the procedure, and also the web client should eventually indicate a successful process.
- The SSL keys (ca_ in_ cert_store and ca-priv-key.der) are not update-able over the air.



## References

* For full detailed user guide open the following website: not available yet
 
