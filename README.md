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


**void clearAllData();**

Clears all of the data currently stored in the data[] array on the local wallaby.

Returns: N/A


**int sendSignal(int signal);**

*int signal: the signal or message that is to be sent to the other wallaby*

Sends an integer signal to the other wallaby. If the other wallaby is not waiting for a signal with the waitForSignal() function, then this function will act in the same way as adding a piece of data to the data queue. Signals are supposed to be used for instances where waiting for the other wallaby to do something is ideal, but can also be used to transmit data.

Returns: -1 if error occurs, 0 if everything functions properly


**int readData();**

Reads the oldest piece of data in the data queue and returns it. After this, the oldest piece of data is removed and the rest of data is shifted forward. Up to 100 pieces of data can be stored, but by changing the DATA_AMNT constant in communication.h the user can set it to whatever he or she would like.

Returns: the first index of the data array


**int sendData(int signal);**

*int signal: the signal or message that is to be sent to the other wallaby*

Functions identically to sendSignal except for the fact that it only adds the signal that is sent to the current end of the data queue array. This is used for simple transmission of data where waiting on the other end is unimportant, and for when many pieces of data are meant to be sent.

Returns: -1 if function failed, 0 if everything functions properly


________________

**Important security info**
For the wallabies given out to teams over the last two seasons, the passwords can be easily cracked with only the wallaby's ID by running a small Python 3 script, or a slightly modified Python 2 one. The algorithm is as follows:

password is composed of: the first five characters of a sha256 hash of a string of the wallaby 4-digit ID + "00"

Python 3 script:
```
import hashlib
hash_id = hashlib.sha256(str(wallaby_id).encode("utf-8")).hexdigest()[0:6]+'00'
```
This can be made more secure by accessing the terminal on the wireless KISS IDE, typing "cd /; find -name wifi_configurator.py" and editing the line in that file containing something similar to the last line of the above code to be a defined and personal password, such as
```
hash_id = "ThisIsMyPassword"
```
It is unsure, but measures to prevent this may be put in place next season.
