# BlueCAT-ICOM-1

This code will allow an ESP32 (or other microcontroller) to read frequency and mode from an Icom radio via the 3.5mm REMOTE jack on the rear.

I'm not a C++ developer and it shows, this is my first ever C++ project coming from a C# and PHP background. I'm simply making this code open source for the benifit of the HAM Radio community as there are many use cases for such code.

Code contributions more than welcome, I'm here to learn!

Amateur Radio Callsign G5STU - creator of https://station-master.online/

Developed on an ESP32-S3 dev kit connected directly to an ICOM-7300, the tip of the 3.5mm jack goes to pin 37, the ring goes to any ground pin, I have no idea if it's the same for other Icom radio or if there is a voltage or current concern, I'll be consulting a professional electrics person before these goes into production. Reproduce at your own risk. 

Perform a full reset on the 7300 to get the matching (known good) settings.

73
G5 STU
