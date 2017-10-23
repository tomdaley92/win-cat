/*

Thomas Daley
10/16/2016
AsyncStreamReader.cc

*/

#ifndef ASYNC_STREAM_READER_H
#define ASYNC_STREAM_READER_H

#define DEFAULT_BUFLEN 4096

#define TEXT_READER 1
#define BINARY_READER 0

#include <windows.h>
#include <queue>

typedef struct StreamReaderData {
    FILE *stream;
    int flag;
    HANDLE key;
    std::queue<char*> q;
    std::queue<int> s;
} StreamReaderData, *PStreamReaderData;

class AsyncStreamReader {
    
    private:
        struct StreamReaderData data;
        HANDLE T_HANDLE = NULL;
        DWORD T_ID;

    public:
        AsyncStreamReader();

        AsyncStreamReader(FILE *pipe, 
                          int type);
        
        ~AsyncStreamReader();
        int Read(char *dest);

};  

#endif
