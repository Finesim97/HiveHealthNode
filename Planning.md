# Planing
How should the user interact with the node?
## Usual operation
On powerup connect with the wifi using the credentials stored on the EEPROM chip. Read every available sensor and send the data to appropiate topics on the MQTT server. Following that the chip waits for all network operations  to finish or fail and then goes into deep sleep for the amount of time configured (Also stored on the EEPROM). The cycle repeats.
## Setup operation
If no credentials are stored, the unit goes into setup mode.
In setup mode the node hosts its own WIFI network. Client devices connected to this wifi will be redirected to a webpage containing a small UI. On this page the user can select a wifi network, choose a mqtt server, set the sleep time and select a topic on said mqtt server for the available sensors.
### Wifi setup page
On a page request a wifi scan is performed and the networks get listed on the returned page. The user can select one of these networks, its SSID then gets pasted into the textbox, which also can be used for manual entry (hidden networks). There also should be a button to activate WPS. When the hardware setup button is pressed again, it also activates WPS. 
There needs a button to test the given settings, before writing them. 
### MQTT page
On this page there needs to be options for:
 - hostname
 - port
 - username
 - password
 - clientid
[Maybe the weird skip option](https://github.com/256dpi/arduino-mqtt)
The sensors get listed and the user can set the topic and a QOS.
Maybe a button to test the connection. Here there also an be an option for the sleep time.
## Reconfiguration
When the setup button is pressed during boot up for at least 2 seconds, the unit enters setup mode as well.
## LED-Codes
The unit has one LED which shows the current status:
- ON: Unit is booting and can be brought into setup mode.
- OFF: Unit is either off or sleeping
- Heart beat: setup mode
- Slow blinking: Active
- Quick blinking: Panic! 
