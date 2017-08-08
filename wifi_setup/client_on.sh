#!/bin/bash

#string version:
#"wpa_cli ter\nkillall hostapd\nwpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf\necho \"Client is now on.\""
wpa_cli ter
killall hostapd
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf
echo "Client is now on."
