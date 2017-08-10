#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#define TIMEOUT_ERR -5000

int initializeCommunications(int mode); //initialize communication lib, choosing client or host
int waitForConnection(); //applies only to the host
int connectToWallaby(const char ssid[], const char psk[]); //connect to nearby wallaby controller. must be in client mode
int waitForSignal(int timeout); //waits for signal to be sent, timeout is in ms  
void clearAll(); //clears data and signals currently in folder
int sendSignal(int signal); //sends integer as a signal
int readData(); //reads oldest piece of data in queue
int sendData(int data); //adds data to other controller's queue

#endif