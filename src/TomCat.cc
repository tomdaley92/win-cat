/*

Thomas Daley
11/13/2016
tomcat.cc

*/

#include "tomcat.h"
#include "pipes.h"
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#define DEBUG 0

TomCat::TomCat(char *filename) {
    this->filename = NULL;
    if (filename != NULL) {
        this->filename = (char *) malloc((strlen(filename) + 1) * sizeof(char));
        strcpy(this->filename, filename);
    }
    this->launched = 0;
}

TomCat::~TomCat() { 
    free(filename);
    delete input;
    close_pipes(pipes);
    _setmode(fileno(stdout), _O_TEXT);
}

int TomCat::Launch() {

    int reader = TEXT_READER;
    if (this->filename == NULL) {

        if (!_isatty(fileno(stdin)) ) {
            reader = BINARY_READER;
        }

        /* Create thread for non-blocking stdin reader */
        AsyncStreamReader *input = new AsyncStreamReader(stdin, reader);
        this->input = input;
        this->output = stdout;

    } else {

        this->pipes = get_pipes(this->filename);
        
        int in_fd = _open_osfhandle((intptr_t)this->pipes.Child_Std_OUT_Rd, _O_BINARY | _O_RDONLY);
        int out_fd = _open_osfhandle((intptr_t)this->pipes.Child_Std_IN_Wr, _O_BINARY | _O_APPEND);

        FILE *in = _fdopen(in_fd, "rb");
        FILE *out = _fdopen(out_fd, "wb");  

        /* Create thread for non-blocking stream reader */
        AsyncStreamReader *input = new AsyncStreamReader(in, reader);
        this->input = input;
        this->output = out;
    }

    _setmode(fileno(stdout), _O_BINARY);
    this->launched = 1;
    return 0;
}

int TomCat::Process(SOCKET ClientSocket) {

    if (!this->launched) {
        Launch();
    }
    
    int iResult; 
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;

    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    
    memset(recvbuf, '\0', DEFAULT_BUFLEN);
    memset(sendbuf, '\0', DEFAULT_BUFLEN);
    
    /* Main run-forever loop: */
    for(;;) {

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        FD_SET(ClientSocket, &readfds);
        FD_SET(ClientSocket, &writefds);
        FD_SET(ClientSocket, &exceptfds);

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
       
        int rs = select(3, &readfds, &writefds, &exceptfds, &timeout);
        if (rs == SOCKET_ERROR) {
            if (DEBUG) fprintf(stderr, "Select() failed with error: %d\n", WSAGetLastError());
            break;
        }       

        if (FD_ISSET(ClientSocket, &readfds)) {
            /* SOCKET IS READY FOR READING */
            iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if ( iResult > 0 ) {
                
                fwrite(recvbuf, 1, iResult, output);
                fflush(output);

            } else if ( iResult == 0 ) {
                if (DEBUG) fprintf(stderr, "Connection closed on other end.\n");
                break;
            } else {
                if (DEBUG) fprintf(stderr, "Recv() failed with error: %d\n", WSAGetLastError());
                break;
            }

            memset(recvbuf, '\0', DEFAULT_BUFLEN);

        } else if (FD_ISSET(ClientSocket, &writefds)) {
            /* SOCKET IS READY FOR WRITING */
            int bytes_read;
            bytes_read = this->input->Read(sendbuf);

            if (bytes_read > 0) {

                iResult = send(ClientSocket, sendbuf, bytes_read, 0);

                if ( iResult == 0 ) {
                    if (DEBUG) fprintf(stderr, "Connection closed on other end.\n");
                    break;
                } else if (iResult < 0 ) {
                    if (DEBUG) fprintf(stderr, "Send() failed with error: %d\n", WSAGetLastError());
                    break;
                }
            } else if (bytes_read < 0) {
                if (DEBUG) fprintf(stderr, "Input reading complete.\n");
                break;
            }

            memset(sendbuf, '\0', DEFAULT_BUFLEN);

        } else if (FD_ISSET(ClientSocket, &exceptfds)) {
            /* SOCKET EXCEPTION */
            if (DEBUG) fprintf(stderr, "Error: Socket exception.\n");
            break;
        }
    }

    if (DEBUG) fprintf(stderr, "Returned from tomcat Process().\n");
    return 0;
}
