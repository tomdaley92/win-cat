/*

Thomas Daley
10/16/2016
server.h

*/

#ifndef SERVER_H
#define SERVER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

int server(char *port, char *filename, int keep_listening);

#endif
