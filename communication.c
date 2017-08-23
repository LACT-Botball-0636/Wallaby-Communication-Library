#include <kipr/botball.h>
#include <stdlib.h>
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

int side = HOST;

int last_index = 0;
int data[DATA_AMNT];

//server variables
struct sockaddr_in server;
struct sockaddr_in dest;
int status, client_fd;
socklen_t size = sizeof(struct sockaddr_in);

//client variables
struct sockaddr_in server_info;
struct hostent *host_ent;
int socket_fd;

thread listenerThread;

void dataListener()
{
    int latest_data = 0;
    
    if (side == HOST)
    {
        while (1)
        {
            if (recv(client_fd, (int *)latest_data, sizeof(int), 0) != 0)
            {
                if (last_index >= DATA_AMNT-1) last_index = 0;
                data[last_index] = latest_data;
                last_index++;
            }
        }
    }
    else
    {
        while (1)
        {
            if (recv(socket_fd, (int *)latest_data, sizeof(int), 0) != 0)
            {
                if (last_index >= DATA_AMNT-1) last_index = 0;
                data[last_index] = latest_data;
                last_index++;
            }
        }
    }
}

int initializeCommunications(int mode)
{
    //1 = client, 0 = host
    //1 = client, 0 = host
    //char text[140];
    side = mode;
    listenerThread = thread_create(dataListener);
    //if network setup files do not yet exist, install them

    //set up scripts
    system("sudo mkdir /home/root/wifi_setup");
    FILE *f = fopen("/home/root/wifi_setup/client_on.sh", "w");
    fprintf(f,"wpa_cli ter\nkillall hostapd\nwpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf\necho \"Client is now on.\"");
    fclose(f);
    system("chmod +x /home/root/wifi_setup/client_on.sh");

    f = fopen("/home/root/wifi_setup/host_on.sh", "w");
    fprintf(f,"wpa_cli ter\nkillall hostapd\nwpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant-wlan0.conf\n/usr/bin/python /usr/bin/wifi_configurator.py\necho \"Host is now on.\"");
    fclose(f);
    system("chmod +x /home/root/wifi_setup/host_on.sh");
    
    if (mode == CLIENT)
    {
        system("cd /;./home/root/wifi_setup/client_on.sh");
        return 0;
    }
    else if (mode == HOST)
    {
        system("cd /;./home/root/wifi_setup/host_on.sh");
        
        //set up socket server. sockets are much more efficient than ssh+touch lol
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        
        struct hostent *he = gethostbyname("192.168.125.1");
        
        memset(&server, 0, sizeof(server));
        memset(&dest, 0, sizeof(dest));
        
        server.sin_family      = AF_INET;
        server.sin_port        = htons(PORT);
        //server.sin_addr.s_addr = INADDR_ANY; //DOES NOT WORK! NEEDS TO LISTEN ON SPECIFIC IP ADDRESS (OF COURSE)
        bcopy((char *)he->h_addr, (char *)&server.sin_addr.s_addr, he->h_length); //assigns an ip address to listen on
        
        
        bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
        
        return 0;
           
    }
    else
    {
        //mode is not recognized
        return -1;
    }
}

int waitForConnection()
{
    if (side == HOST)
    {
        listen(socket_fd, BACKLOG);
        if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size)) == -1)
        {
            printf("Failure accepting connection.\n");
            return -1;
        }
        printf("Connection successfully made.\n");
        
        //start data listening thread
        thread_start(listenerThread);
        
        return 0;
    }
    else
    { return -1;}
    
}

int connectToWallaby(const char ssid[], const char psk[]) //returns -1 if connection fails, -2 if not in client mode
{
    if (side == HOST) return -2; //instead of doing this, maybe run client_on.sh
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
        strcpy(response,"");
        fp = popen("wpa_cli status", "r");
        while (fgets(temp,1000,fp))
        {
            sprintf(response,"%s%s",response,temp);
        }
        strcpy(temp,strstr(response,"wpa_state="));
        memcpy(status, &response[((int)(temp-response))+10],9); //places the status into var status
        status[9] = '\0';
        if (!strcmp(status, "COMPLETED"))
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
        console_clear();
    }
    
    //if connection succeeded, assign IP addresses
    //then connect via sockets
    if (end == 1)
    {        
        system("dhclient wlan0");
        
        host_ent  = gethostbyname("192.168.125.1");
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        memset(&server_info, 0, sizeof(server_info));
        
        server_info.sin_family = AF_INET;
        server_info.sin_port   = htons(PORT);
        //server_info.sin_addr   = *((struct in_addr *)host_ent->h_addr);
        bcopy((char *)host_ent->h_addr, (char *)&server_info.sin_addr.s_addr, host_ent->h_length);
        
        if (connect(socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) < 0)
        {
            printf("Connection failed.\n");
            return -1;
        }
        thread_start(listenerThread);
        return 0;
    }
    return -1;
}

int waitForSignal(int timeout)
{      
    //returns signal if it is received, otherwise returns error
    int message = 0;
    int base = 0;
    
    thread_destroy(listenerThread);
    
    if (side == HOST)
    {
        while (base < timeout)
        {
            if ((recv(client_fd, (int *)message, sizeof(int), MSG_DONTWAIT)) != 0)
            {
                listenerThread = thread_create(dataListener);
                thread_start(listenerThread);
                return message;
            }
            base += 100;
            msleep(100);
        }
    }
    else
    {
        while (base < timeout)
        {
            if ((recv(socket_fd, (int *)message, sizeof(int), MSG_DONTWAIT)) != 0)
            {
                listenerThread = thread_create(dataListener);
                thread_start(listenerThread);
                return message;
            }
            base += 100;
            msleep(100);
        }
    }
    listenerThread = thread_create(dataListener);
    thread_start(listenerThread);
    return TIMEOUT_ERR;
}

void clearAllData()
{
    int i;
    for (i = 0; i < DATA_AMNT; i++)
    {
        data[i] = 0;
    }
}

int sendSignal(int signal)
{
    //return 0 if sending works, otherwise return -1
    if (side == HOST)
    {
        if ( send(client_fd, (int *)signal, sizeof(int), 0) == 0)
        {
            return -1;
        }
    }
    else
    {
        if ( send(socket_fd, (int *)signal, sizeof(int), 0) == 0)
        {
            return -1;
        }
    }
    
    return 0;
}

int readData()
{
    int data_val = data[0];
    int i;
    int val;
    
    //shifts data one location forward
    for (i = 0; i < 99; i++)
    {
        val = data[i+1];
        data[i] = val;
        if (last_index != 0) last_index -= 1;
    }
    return data_val;
}

int sendData(int signal)
{
    //return 0 if sending works, otherwise return -1
    if (side == HOST)
    {
        if ( send(client_fd, (int *)signal, sizeof(int), 0) == 0)
        {
            return -1;
        }
    }
    else
    {
        if ( send(socket_fd, (int *)signal, sizeof(int), 0) == 0)
        {
            return -1;
        }
    }
    return 0;
}
