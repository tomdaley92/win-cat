/*

Thomas Daley
10/16/2016
client.cc

*/

#include "client.h"
#include "tomcat.h"
#include <io.h>
#include <fcntl.h>

#define DEBUG 0

int client(char *host, char *port, char *filename) {

	WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;

    /* Initialize Winsock */
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    /* Resolve the server address and port */
    iResult = getaddrinfo(host, port, &hints, &result);
    if ( iResult != 0 ) {
        fprintf(stderr, "Unable to resolve host and/or port.\n");
        if (DEBUG) fprintf(stderr, "Getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    /* Attempt to connect to an address until one succeeds */
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        /* Create a SOCKET for connecting to server */
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            fprintf(stderr, "Socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        /* Connect to server */
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        fprintf(stderr, "Unable to connect to host.\n");
        WSACleanup();
        return 1;
    }
    
   	/* Where the magic happens */
    TomCat *tomcat = new TomCat(filename);
    tomcat->Process(ConnectSocket);

    /* Shut down the connection since we're done */
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "Shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
   	
    /* Cleanup */
    closesocket(ConnectSocket);
    WSACleanup();

	return 0;
}
