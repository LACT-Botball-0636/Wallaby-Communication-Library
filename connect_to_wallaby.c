void connectToWallaby(const char ssid[], const char psk[]) //returns -1 if connection fails, -2 if not in client mode
{
	//ssid is the ssid of wifi network - e.g. 1234-wallaby
	//psk is the password of the wifi network - e.g. 1234ab00

    char cmd[50];
    char set_ssid[75];
    char set_psk[75];
	
	//remove existing networks
    int i;
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
	system("sudo dhclient wlan0");
	
	printf("Connected to %s!", ssid);
}
