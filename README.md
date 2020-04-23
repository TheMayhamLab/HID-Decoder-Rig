# HID-Reader

This project was created so I could learn more about Wiegnad encoding and how RFID cards work at the 125KHz frequency. 

The goal of this project was to read an 125KHz RFID card and extract the card number, facilty code, and the tag ID (similar to what you would see when scanning an RFID card with a Proxmark3). It would have been possible to look at manuals and the Proxmark3 source code and figure out how it decodes cards. However, I learn better from doing a project myself so I decided to build a tool (with the help of some existing code). This project was based off of code written by Daniel Smith (www.pagemac.com) in 2012. 

Equipment used
---------------
- HID reader
- 24v power supply
- Arduino Uno
- Magnetic lock
- Voltage regulator
- Wire
