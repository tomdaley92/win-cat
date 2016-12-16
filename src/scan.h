/*

Thomas Daley
11/13/2016
scan.h

*/

#ifndef SCAN 
#define SCAN 

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#define MIN_PORT 1
#define MAX_PORT 65535

int connect_scan(char *host, int low, int high);
int ping_scan(char *cidr, int timeout);

#endif
