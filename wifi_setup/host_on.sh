#!/bin/bash
#string version:
#"/usr/bin/python /usr/bin/wifi_configurator.py\necho \"Host is now on.\""
wpa_cli ter
killall hostapd
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
/usr/bin/python /usr/bin/wifi_configurator.py
echo "Host is now on."
