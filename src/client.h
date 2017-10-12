/*

Thomas Daley
10/16/2016
client.h

*/
#ifndef CLIENT_H
#define CLIENT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

int client(char *host, char *port, char *filename);

#endif
