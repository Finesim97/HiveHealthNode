# WebPage Requirements
The setting values should be read from a URL serving a json file.

## Login page (login.html)
Form with a single password field, sends a POST to /login.do

## Main page (index.html)
Header, Footer and stuff like that (Will be reused in the other pages)
Menu with the following entries:
* Current Sensor Values (links to /sensors.html)
* WiFi Settings (links to /wifi.html )
* MQTT Settings (links to /mqtt.html)
* Reboot (links to /reboot.do)
* Update firmware (links to /update.html)
* Restore Defaults (links to /reset.html)
* Logout (links to /logout)

## sensors.html
Just a fragment, will be placed inside the stuff used in the index.html!
For each sensor the name, the description and a current value.

## wifi.html
Just a fragment, will be placed inside the stuff used in the index.html!
* A overview of currently visible networks. (JavaScript: When the user clicks on one, the name gets placed inside the textbox) + Rescan button
* SSID field
* Radiobox menu with Personal/Enterprise selection, the others get disabled
    * Personal: Passphrase field
    * Enterprise
         * EAP Identity field
         * EAP Username field
         * EAP Password field
* Radiobox menu with DNS Server Selection
    * Automatic
    * Manual
      * Primary DNS Server IP field
      * Secondary DNS Server IP field
* Radiobox menu with IP Setup Selection
    * Automatic
    * Manual
      * Local IP field
      * Gateway IP field
      * Subnet IP field 
* Test Connection button
* Save Settings button
* Start WPS Button button

## mqtt.html
Just a fragment, will be placed inside the stuff used in the index.html!
* MQTT hostname
* MQTT port
* MQTT username
* MQTT password
* MQTT clientname

Extra section with
* test topic
* test publish button

Every sensor with
* Name and description
* Topic name field
* Interval time (seconds)
* Checkbox retain (true/false)


## update.html
Also only a fragment, only an input for a file and a update button.

## reset.html
Only a warning and a checkbox with "I am sure" and a button.
