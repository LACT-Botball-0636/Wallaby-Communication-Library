# Wallaby-Communication-Library
Library that includes functions and files to allow communication between two KIPR wallabies.

Written by Anson Bridges and Jonathan Liu of team 0636.

_________________________________________

# Library Documentation

**int initializeCommunications(int mode);**
*int mode: integer deciding whether the wallaby running this function will be the host or the client. One wallaby must be each. setting this to 1 puts it in client mode, setting it to 0 puts it in host mode. Anything else does effectively nothing and will cause the rest to not work.*

Initializes communications library by placing shell script files (.sh), running those files to put the wallaby's network setup into either a client or host configuration, and binding a socket if the user chooses host mode.

Returns: 0 if everything works, otherwise -1


**int waitForConnection();**

Only used on the host wallaby. This should be run before the other wallaby tries to connect to it with the connectToWallaby() function. Waits for a connection from a client wallaby. Right now there is no timeout, though in the future there will be one.

Returns: 0 if everything works, otherwise -1


**int connectToWallaby(const char ssid[], const char psk[]);**
*const char ssid[]: string containing the SSID of the wallaby that the client wants to connect to
const char psk[]: string containing the password of the wallaby that the client wants to connect to*

Only used for the client wallaby, while the host runs waitForConnection(). Connects to given wallaby over wifi, then to the other program using sockets.

Returns: -2 if wallaby is in host mode, -1 if an error occurs connecting to the other wallaby, and 0 if everything works

**int waitForSignal(int timeout);**
*int timeout: timeout for how long the function should wait before giving up, in milliseconds (ms)*

Waits for a signal from the other wallaby, and returns the signal it receives. 

Returns: the signal integer from the other wallaby if it works, or if the timeout triggers then it returns constant TIMEOUT_ERR.
