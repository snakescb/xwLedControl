# xwLedControl
xwLedControl is a flexible, highly configurable RC Lightning System. It was formerly know as SkyLED, sold as a commercial product by InnoSKY which was decommissioned a few years ago. Since then, we received a lot of request to start offering it again. Therefore we recreated the hardware from scratch, including some improvements, and made it available for everybody to produce / order himself. The PC configurator is re-developped in simplified form as a cross platform .Net-Core 3.1 WPF application and published here.

![xwLedControl](https://user-images.githubusercontent.com/10495848/147221539-66b14f9b-4330-4ec9-8169-2145c49a1405.PNG)

## Get your Led Controller
- You can download gerber files from Github and produce the PCB on your own.
- I alwas try to have some assembled boards on stock. Get in touch with me to check inventory. It will be sold for CHF 50 per piece (excl. shipping), with no warranty as this is a non-profit open-source / open-hardware projct.
- The new controller is designed in EasyEDA and shared (Link below in the versions). You can clone the EasyEDA project, make changes as you like and order the PCB on your own. All designs are using basic parts which are normally in stock for assembly at JLCPCB, so you can order your board directly from EasyEDA fully assembled if desired.

## xwLedControl Basic
The original xwLedControl, formerly SkyLED by InnoSKY, with included USB port and auto-shutdown - [EasyEDA Project](https://oshwlab.com/luethich80/xwlightcontrol)

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
**Version 1.0**
- Changes
  - Inital version currently tested
- Known Issues
  - No known issues



