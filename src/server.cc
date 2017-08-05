/*

Thomas Daley
10/16/2016
server.cc

*/

#include "server.h"
#include "tomcat.h"
#include <io.h>
#include <fcntl.h>

#define DEBUG 0

int server(char *port, char *filename, int keep_listening) {

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    /* Initialize Winsock */
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    /* Resolve the server address and port */
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if ( iResult != 0 ) {
        if (DEBUG) fprintf(stderr, "Getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    /* Create a SOCKET for connecting to server */
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        fprintf(stderr, "Socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    /* Setup the TCP listening socket */
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "Port %s is already in use.\n", port);
        if (DEBUG) fprintf(stderr, "Bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    
    TomCat *tomcat = new TomCat(filename);

    do {

        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            fprintf(stderr, "Accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        
        tomcat->Process(ClientSocket);

    } while (keep_listening);


    /* No longer need server socket */
    closesocket(ListenSocket);

    /* Shut down the connection since we're done */
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        fprintf(stderr, "Shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        return 1;
    }
        
    /* Cleanup */
    delete tomcat;
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
