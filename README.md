# IC705BlueAmpCtrl
Bluetooth Amplifier Controller with PTT and band switching

The IC-705 radio by Icom provides a bluetooth connection to its CI-V data.  Although the radio also provides an external PTT connection, Icom has not yet published the internal electrical characteristics of this connection, so a bluetooth PTT is desirable for the experimenter.

This code was developed for Arduino's Nano 33 Iot.  This board has a Bluetooth ESP32 module which can do "Classic" bluetooth (as well as the new Bluetooth Low Energy.)  It also, conveniently, comes with a 10 bit DAC for producing the band switching voltage.

First, follow the procedure at "[This Link](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)" to load the esp-idf support to your Windows computer.  This allows the Arduino IDE to be able to load software to the ESP computer in step 4 below.

EDIT: Also - make SURE you are using a USB3.0 port on your Windows computer.  USB2.0 won't work!

So, the steps are (using the Arduino IDE):

1. Set the board to "Arduino Nano 33 Iot" (under SAMD 32 bit ....)
2.  Load the 33 Iot computer with the "SerialNINAPassthrough" example (under WifiNINA, Tools.)
3.  Change the board to "u-Blox NINA W-10" (under ESP32 Arduino.)
4.  Load the NINA W-10 computer with "SerialToSerialBT" example (under Bluetooth Serial.)
5. Change the board back to "Arduino Nano 33 Iot."
EDIT: 5.5 Double click the reset button on the IOT 33.  Set the "Port" to the NEW serial port that shows up!
6. Load this code, "IcomToYaesuBandData."
7. Connect analog port A0 (output of the DAC) to the band data of the amplifier.
8. Connect digital port D13 to the amplifier's PTT (through a buffer - remember -- there is an LED load already on this port.)
I suggest a 2n2222 or 2n4123 or similar buffer transistor.  Connect D13 through a 1000 ohm resistor to the base of the transistor,
connect the emitter to ground, and the collector to the amplifier PTT line.  This also handles level shifting for the 3v D13 port.
9. On the radio, go to Menu, Set, Bluetooth Set, Pairing/Connect, Device Search, Search Data Device, ESP32test, Connect-Yes.
10.  As you change frequencies on the radio, the amplifier's bands should automatically switch.
11.  As you press PTT on the radio, the amplifier's PTT should follow.

This code uses code from Mooneer Salem's "Elecraft ToYaesuBandData" Arduino project.

If you have suggestions or have noticed errors in the source code, you may communicate via email at engdahl@snowcrest.net.

IMPORTANT: I DO NOT ASSUME ANY RESPONSIBILITY FOR YOUR USE OF THIS SOFTWARE, AND I MAKE NO REPRESENTATIONS ABOUT ITS SAFETY, APPROPRIATENESS, OR USE FOR A PARTICULAR PURPOSE.  USE OF THIS SOFTWARE IS AS-IS.

Dennis Engdahl
NR6J
engdahl@snowcrest.net

