/*

Thomas Daley
11/13/2016
WinCat.cc

*/
#include "wincat.h"
#include "pipes.h"
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#define DEBUG 0

WinCat::WinCat(char *filename) {
    this->filename = NULL;
    if (filename != NULL) {
        this->filename = (char *) malloc((strlen(filename) + 1) * sizeof(char));
        strcpy(this->filename, filename);
    }
    this->launched = 0;
}

WinCat::~WinCat() { 
    if (pipes.process_spawned) close_pipes(pipes);
    free(filename);
    if (input) delete input;
    _setmode(fileno(stdout), _O_TEXT);
}

int WinCat::Launch() {
    int reader = TEXT_READER;

    if (this->filename == NULL) {
        if (!_isatty(fileno(stdin)) ) {
            reader = BINARY_READER;
        }

        /* Create thread for non-blocking stdin reader */
        AsyncStreamReader *input = new AsyncStreamReader(stdin, reader);
        this->input = input;

        /* Enable VT100 and similar control character sequences 
           that control cursor movement, color/font mode, 
           and other operations */
        SetConsoleMode( GetStdHandle(STD_OUTPUT_HANDLE), 
                        ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING); 

        this->output = stdout;

    } else {
        /* Create a new process */
        this->pipes = get_pipes(this->filename);
        if (!pipes.process_spawned) {
            return 1;
        } 
    
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

int WinCat::Process(SOCKET ClientSocket) {
    if (!this->launched) {
        if(Launch()) return 1;
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

        int rs = select(1, &readfds, &writefds, &exceptfds, NULL);
        if (rs == SOCKET_ERROR) {
            if (DEBUG) fprintf(stderr, "Wincat: Select() failed with error: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(ClientSocket, &readfds)) {
            /* SOCKET IS READY FOR READING */
            iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if ( iResult > 0 ) {
                fwrite(recvbuf, 1, iResult, output);
                fflush(output);
            } else if ( iResult == 0 ) {
                if (DEBUG) fprintf(stderr, "Wincat: Connection closed on other end.\n");
                break;
            } else {
                if (DEBUG) fprintf(stderr, "Wincat: Recv() failed with error: %d\n", WSAGetLastError());
                break;
            }
            memset(recvbuf, '\0', DEFAULT_BUFLEN);

        } else if (FD_ISSET(ClientSocket, &writefds)) {
            /* SOCKET IS READY FOR WRITING */
            int bytes_read = this->input->Read(sendbuf);
            if (bytes_read > 0) {
                iResult = send(ClientSocket, sendbuf, bytes_read, 0);
                if ( iResult == 0 ) {
                    if (DEBUG) fprintf(stderr, "Wincat: Connection closed on other end.\n");
                    break;
                } else if (iResult < 0 ) {
                    if (DEBUG) fprintf(stderr, "Wincat: Send() failed with error: %d\n", WSAGetLastError());
                    break;
                }
            } else if (bytes_read < 0) {
                if (DEBUG) fprintf(stderr, "Wincat: Input reading complete.\n");
                break;
            }
            memset(sendbuf, '\0', DEFAULT_BUFLEN);

        } else if (FD_ISSET(ClientSocket, &exceptfds)) {
            /* SOCKET EXCEPTION */
            if (DEBUG) fprintf(stderr, "Wincat: Socket exception.\n");
            break;
        }
        /* Free up some CPU time for the OS */
        Sleep(1);
    }
    return 0;
}
