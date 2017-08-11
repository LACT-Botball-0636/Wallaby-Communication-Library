#include <kipr/botball.h>
#include <stlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "communication.h"

int side = 0;

//server variables
struct sockaddr_in server;
struct sockaddr_in dest;
int status, client_fd;
socklen_t size = sizeof(struct sockaddr_in);

//client variables
struct sockaddr_in server_info;
struct hostent *host_ent;
int socket_fd;

#define PORT 1266 //636 is already taken
#define BACKLOG 1 //number of clients, essentially

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
		system("chmod +x /home/root/wifi_setup/client_on.sh");
        
        f = fopen("/home/root/wifi_setup/client_on.sh", "w");
        text = "/usr/bin/python /usr/bin/wifi_configurator.py\necho \"Host is now on.\"";
        fprintf(f,text);
        fclose(f);
		system("chmod +x /home/root/wifi_setup/host_on.sh");
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
        
        //set up socket server. sockets are much more efficient than ssh+touch lol
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        
        memset(&server, 0, sizeof(server));
        memset(&dest, 0, sizeof(dest));
        
        server.sin_family      = AF_INET;
        server.sin_port        = htons(PORT);
        server.sin_addr.s_addr = INADDR_ANY;
        
        bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
    	
		
        return 0;
           
    }
    else
    {
        return -1;
    }
}

int waitForConnection()
{
    if (side == 0)
    {
        listen(socket_fd, BACKLOG);
        if ((client_fd = accept(socket_fd, (struct sockaddr_in *)&dest, &size)) == -1)
        {
            printf("Failure accepting connection.\n");
            return -1;
        }
        printf("Connection successfully made.\n");
        
        return 0;
    }
    else
    { return -1;}
    
}

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
        memcpy(status, &response[((int)(temp-response))+10],9); //places the status into var status
        status[9] = '\0';
        if (status == "COMPLETED")
        {
            end = 1;
        }
        if (timer >= 10000)
        {
            //timed out
            end = 2;
        }
        timer += 100;
        msleep(100);
    }
    
    //if connection succeeded, assign IP addresses
    //then connect via sockets
    if (end == 1)
    {        
        system("dhclient -v -r wlan0");
        
        host_ent  = gethostbyname("192.168.125.1");
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                
        memset(&server_info, 0, sizeof(server_info));
        
        server_info.sin_family = AF_INET;
        server_info.sin_port   = htons(PORT);
        server_info.sin_addr   = *((struct in_addr *)host_ent->h_addr);
        
        if (connect(socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) < 0)
        {
            printf("Connection failed.\n");
            return -1;
        }
        return 0;
    }
    return -1;
}

int waitForSignal(int timeout)
{      
    //returns signal if it is received, otherwise returns error
    int message;
    int base = 0;
    while (base < timeout)
    { 
        if ((recv(socket_fd, message, sizeof(int), MSG_DONTWAIT)) != 0)
        {
            return message;
        }
        base += 100;
        msleep(100);
    }
    return TIMEOUT_ERR;
}
//f
int sendSignal(int signal)
{
    //return 0 if sending works, otherwise return -1
    if ( send(socket_fd, signal, sizeof(int), 0) == 0)
    {
        return -1;
    }
    return 0;
}
