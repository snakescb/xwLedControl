# xwLedControl
xwLedControl is a flexible, highly configurable RC Lightning System. It was formerly know as SkyLED, sold as a commercial product by InnoSKY which was decommissioned a few years ago. Since then, a lot of request to start offering it again have been made. Therefore the  hardware was recreated from scratch, including some improvements, and was made available for everybody to produce / order himself. The PC configurator is re-developped as a .Net-Core 3.1 WPF application and is published here as well.

## Welcome Version 2
xwLedControl just became better, lighter, smaller and cheaper! By eliminiating some unused features (like the sensor input), streamlining and simplifying the schematics and the use of smaller component, the new controller has all the great features the Software offers, but at a 50 smaller size!

![v2](https://user-images.githubusercontent.com/10495848/156210805-ca65eced-25a5-41af-9e9d-e228244335f5.PNG)

### V2 Features
- Only 37 x 17mm 4 layer PCB
- Builtin mini USB for configuration of light sequences from PC Software
- Auto-shutdown, disconnects the battery when receiver or USB is not connected (can be disabled with solder jumper, the module can light your model without receiver connected)
- Per module up to 6 single color channels, 2 RGB channels or 3 LED + 1 RGB channel (for common anode RGB strips)
- Up to 24 channels with one master module conecting up to 3 slave modules through Master/Slave connection
- On/Off control and light sequences switching from RC receiver
- Battery monitoring
- New 1s - 3s LiPo input voltage, for also connecting high power LED's with smaller inline resistors
- Up to 5A per output channel (Mosfets are rated for 10A per channel!)

### V2 Versions and know issues

**Version 2.0**
- Changes
  - Inital version currently tested
- Known Issues
  - Power supply error is burning USB chip on battery power
  - Ground pin not connected on STM32
  - Boot1 pin is floating, which is preventing STM32 to enter bootloader mode

## Get your Led Controller
- You can download gerber files from Github and produce the PCB on your own.
- I alwas try to have some assembled boards on stock. Get in touch with me to check inventory. It will be sold for CHF 30 - 50 per piece (excl. shipping, depending on version and parts availability / prices at JTLPCB), with no warranty as this is a non-profit open-source / open-hardware projct.
- The new controller is designed in EasyEDA and shared (Link below in the versions). You can clone the EasyEDA project, make changes as you like and order the PCB on your own. All designs are using basic parts which are normally in stock for assembly at JLCPCB, so you can order your board directly from EasyEDA fully assembled if desired.

## xwLedControl Basic
The original xwLedControl, formerly SkyLED by InnoSKY, including USB port and auto-shutdown. The Basic version is simply connting the LED's with the attached batteries, so it requires inline resistors wit the LED's not to damage the,  - [EasyEDA Project](https://oshwlab.com/luethich80/xwlightcontrol)

![ledtop](https://user-images.githubusercontent.com/10495848/151327156-f55c5d70-1b84-4303-881a-be63a9818bc9.PNG)

### Features
- 47 x 25mm 4 layer PCB
- Builtin mini USB for configuration of light sequences from PC Software
- Auto-shutdown, disconnects the battery when receiver not connected (can be disabled with solder jumper)
- Per module up to 6 LED channels, 2 RGB channels or 3 LED + 1 RGB channel (for common anode RGB strips)
- Up to 24 channels with one master module conecting up to 3 slave modules through Master/Slave connection
- On/Off control and light sequences switching from RC receiver
- 5-14V input voltage, suitable for 2S - 3S LiPo batteries
- Up to 2A per output channel 

### Versions and know issues

**Version 1.1**
- Changes
  - Fixed power supply
  - Fixed ground connections
  - Added switched voltage pad, to power slave devices with auto shutdown from master device
  - Boot1 pin connected to ground
  - Additional capacity on CP2102N according datasheet
  - Added SWD, UART, Reset and Boot0 testpoints on back copper layer for easier development
- Known Issues
  - No known issues
  
**Version 1.0**
- Changes
  - Inital version currently tested
- Known Issues
  - Power supply error is burning USB chip on battery power
  - Ground pin not connected on STM32
  - Boot1 pin is floating, which is preventing STM32 to enter bootloader mode



