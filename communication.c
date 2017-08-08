#include <kipr/botball.h>
#include <cstlib>
#include <cstdio>
#include "communication.h"

int side;

int initializeCommunications(int mode)
{
    //1 = client, 0 = host
	char text[140];
	side = mode;
	
	//if network setup files do not yet exist, install them
	if (system("[ -d /home/root/wifi_setup ]")) 
	{
		system("sudo mkdir /home/root/wifi_setup");
		FILE *f = fopen("/home/root/wifi_setup/client_on.sh", "w");
		text = "wpa_cli ter\nkillall hostapd\nwpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf\necho \"Client is now on.\"";
		fprintf(f,text);
		fclose(f);
		
		f = fopen("/home/root/wifi_setup/client_on.sh", "w");
		text = "/usr/bin/python /usr/bin/wifi_configurator.py\necho \"Host is now on.\"";
		fprintf(f,text);
		fclose(f);
	}
	
	//if communication directories do not yet exist, create them
	if (system("[ -d /home/root/communication ]")
	{
		system("sudo mkdir /home/root/communication");
		system("sudo mkdir /home/root/communication/signal");
		system("sudo mkdir /home/root/communication/data");
	}
	
	if (mode == 1)
	{
	    system("cd /;./home/root/wifi_setup/client_on.sh");
		return 0;
	}
	else if (mode == 0)
	{
	    system("cd /;./home/root/wifi_setup/host_on.sh");
		return 0;
	}
	else
	{
		return -1;
	}
}

//ALSO NEEDS TO SET UP SOCKETS
int connectToWallaby(const char ssid[], const char psk[]) //returns -1 if connection fails, -2 if not in client mode
{
	if (side == 0) return -2;
	//remove existing networks
	int i;
	
	char cmd[50];
	char set_ssid[75];
	char set_psk[75];
	
	FILE *fp;
	char response[1000];
	char temp[1000];
	char status[10];
	
	int timer = 0;
	int end = 0;

	for (i = 0; i < 10; i++)
	{
		sprintf(cmd, "wpa_cli remove_network %d", i);
		system(cmd);
	}
	console_clear();
	
	//connect to given wallaby
	system("wpa_cli add_network");
	sprintf(set_ssid, "wpa_cli set_network 0 ssid '\"%s\"'", ssid);
	sprintf(set_psk, "wpa_cli set_network 0 psk '\"%s\"'", psk);
	
	console_clear();
	
	system(set_ssid);
	system(set_psk);
	system("wpa_cli enable_network 0");
	system("wpa_cli save");
	
	//wait for connection (not sure if this works yet)
	while (!end)
	{
		response = "";
		fp = popen("wpa_cli status", "r");
		while (fgets(temp,1000,fp))
		{
			sprintf(response,"%s%s",response,temp);
		}
		temp = strstr(response,"wpa_state=");
		memcpy(status, &response[(int)(temp-response)],9);
		status[9] = '\0';
		if (status == "COMPLETED")
		{
			end = 1;
		}
		if (timer >= 10000)
		{
			end = 2;
		}
		timer += 100;
		msleep(100);
	}
	
	//if connection succeeded, assign IP addresses
	if (end == 1)
	{
		system("dhclient -v -r wlan0");
		return 0;
	}
	return -1;
}

int waitForSignal(int time)
{
	int passed = 0;
	int end = 0;
	while (!end && passed < time)
	{
		
		passed += 100;
		msleep(100);
	}
}