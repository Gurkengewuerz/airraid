# Air Raid Sirene (motorized) Hardware

This is the repository to my hardware of the motorized Air Raid Sirene model from [Thingiverse](https://www.thingiverse.com/thing:2357451) by the user DarthBane.

[**SEE THIS THING IN ACTION!**](https://twitter.com/Gurkengewuerz/status/1040680115078922241)

![raid_druck_mit_platine](/img/raid_druck_mit_platine.jpg | width=850)



## Schematic

The schematic is pretty simple. The ESP8266 is powered by the same power supply (power supply should be ratet with **12V/8A**) as the ESC/BEC by using the LM7805 positive-voltage regulator. The ESC ist controlled by the WEMOS D1 mini via PWM with the Arduino servo Library.

![schematic](/schematic.png)



## PCB Desgine

There is no PCB desgine because i just used a small perfboard.

### !!! ATTENTION !!!

Please leave a good gap of minimum 100 mil or more between the 12V and GND traces! If the motor is at high speeds it can draw up to **7 Amps**! Be careful and **don't use** small jumper Wires!!! You are using a large power supply!



## Hide the PCB

To hide the PCB and also to hold the Air Raid at the ceiling, i designed a small holder. It replaces the original handle.
To install the holder i replaced the motor M3 6mm screws with longer M3 20mm screws and also uses the original small pins from the handle for stabilisations.

![extra holder](/img/holder.jpg)



## Software

After flashing the WEMOS D1 mini and after powering it up, the µC opens a AccessPoint.

**IP**: *192.168.4.1*
**SSID**: *AIR_RAID*
**PW**: *BBK-Raid-v01*

The webinterface is pretty simple and self explaining.

![webinterface](/img/raid_webinterface.jpg)



---

Twitter: [@Gurkengewuerz](https://twitter.com/Gurkengewuerz)
