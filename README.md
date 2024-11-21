# ESP8266 RepairCafe-AP
Temporary Access Point for Repair Cafes to emulate customer APs 

# What does it do? 
Every now and then you will get a customer in your Repair Cafe with an apparatus that relies on a WiFi connection. The apparatus depends on the WiFi settings at home and setting it up temporarily to use another WiFi network can be a hassle or just too much effort. That's where the ESP8266 Repair Cafe AP comes in. It allows to set up a temporary WiFi Network with the credentials the customer has at home. The apparatus under repair can access it as well as the Repair Cafe members – thus accessing the apparatus under repair. Once booted with the new credentials it deletes them, so it will never keep these stored.

# In easy steps:
1. Connect the ESP8266 to a 5V USB charger and start
2. The ESP8266 RepairCafe-AP boots up with default credentials:
       SSID: RepairCafe_AP
       PWD:  12345678
3. Log on to this AP with a laptop or other device
4. Navigate with a browser to http://192.168.4.1 and you'll get the Credentials Setup web-page
5. Set up the new credentials matching the WiFi name and password from the customer's WiFi network and save the page
6. The ESP8266 will store the new credentials in EEPROM and reboot
7. When rebooted, it will read the new credentials from EEPROM and start the temporary AP with these credentials.
8. After the temporary AP is started it will delete the new credentials from EEPROM, so with a subsequent reboot it will start an AP with the default credentials again
