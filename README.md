# KiteXEspWsSlave
Arduino program that translate motor commands over Wifi (web socket) to i2c commands. 
Also reads the main battery voltage and sends this information back over the socket.

# Install
Just follow general guidelines on running Arduino on a ESP8266, once you have installed a blinky, run this program instead :)

# Learnings
I had a lot of success with the Adafruit Huzzad, once I figures out that powering it from my MB Airs USB port wasn't sufficient.
A seperate power supply (in my case a LIPO battery, directly connected) and then just connect Ground, TR AND TX to the FTDI.
Runnsing Smooth ever since.
I tried to run the websocket using the Async method, but didn't see any improved performance in terms of latency and blooking time - actually contrary so I went back to the loop().
