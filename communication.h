#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

int initializeCommunications(int mode);
int connectToWallaby(const char ssid[], const char psk[]); //connect to nearby wallaby controller. must be in client mode
int waitForSignal(int time); //waits for signal to be sent. has shutoff in ms, returns -1 if no data sent else returns signal data
void clearAll(); //clears data and signals currently in folder
int sendSignal(int signal); //sends integer as a signal
int readData(); //reads oldest piece of data in queue
int sendData(int data); //adds data to other controller's queue

#endif