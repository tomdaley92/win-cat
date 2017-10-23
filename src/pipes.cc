/*

Thomas Daley
10/26/2016
pipe.cc

*/

#include <windows.h>
#include <tchar.h>
#include "atlstr.h"
#include "pipes.h"

#define DEBUG 0

PipeHandles get_pipes(char *filename) {

    struct PipeHandles pipes;

    pipes.Child_Std_IN_Rd = NULL;
    pipes.Child_Std_IN_Wr = NULL;
    pipes.Child_Std_OUT_Rd = NULL;
    pipes.Child_Std_OUT_Wr = NULL;

    SECURITY_ATTRIBUTES saAttr; 

    /* Set the bInheritHandle flag so pipe handles are inherited. */
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    /* Create a pipe for the child process's STDOUT */
    if (!CreatePipe(&pipes.Child_Std_OUT_Rd, &pipes.Child_Std_OUT_Wr, &saAttr, 0)) {
        fprintf(stderr, "StdoutRd CreatePipe error.\n");
        return pipes; 
    }

    /* Ensure the read handle to the pipe for STDOUT is not inherited */
    if (!SetHandleInformation(pipes.Child_Std_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "Stdout SetHandleInformation error.\n");
        return pipes; 
    }

    /* Create a pipe for the child process's STDIN */ 
    if (!CreatePipe(&pipes.Child_Std_IN_Rd, &pipes.Child_Std_IN_Wr, &saAttr, 0))  {
        fprintf(stderr, "Stdin CreatePipe error.\n");
        return pipes; 
    }

    /* Ensure the write handle to the pipe for STDIN is not inherited */ 
    if (!SetHandleInformation(pipes.Child_Std_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "Stdin SetHandleInformation error.\n");
        return pipes; 
    }

    TCHAR szCmdline[MAX_PATH];
    _tcscpy(szCmdline, A2T(filename));
 
    STARTUPINFO siStartInfo; 

    /* Set up members of the PROCESS_INFORMATION structure */ 
    ZeroMemory( &pipes.piProcInfo, sizeof(PROCESS_INFORMATION) );

    /* Set up members of the STARTUPINFO structure. 
       This structure specifies the STDIN and STDOUT handles for redirection */
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
    siStartInfo.hStdError = NULL;
    siStartInfo.hStdError = pipes.Child_Std_OUT_Wr;
    siStartInfo.hStdOutput = pipes.Child_Std_OUT_Wr;
    siStartInfo.hStdInput = pipes.Child_Std_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    HANDLE CallerToken= NULL;
    if ( !OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS , &CallerToken) ) {
        fprintf(stderr, "OpenProcessToken failed - 0x%08x\n", GetLastError());
    }

    /* Create the child process */ 
    pipes.process_spawned = CreateProcessAsUser(
        CallerToken,
        NULL, 
        szCmdline,     // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &pipes.piProcInfo);  // receives PROCESS_INFORMATION 

    /* If an error occurs, exit the application */ 
    if ( !pipes.process_spawned ) {
        fprintf(stderr, "Unable to spawn child process.\n");   
    }
    
    /* We no longer need this handle */
    CloseHandle(CallerToken);

    return pipes;
}

int close_pipes(struct PipeHandles pipes) {
    //if (!CancelIo(pipes.Child_Std_OUT_Rd)){
        //if (DEBUG) fprintf(stderr, "Unable to cancel IO operations on child's STDOUT read handle.\n");
    //}

    //if (!CancelIo(pipes.Child_Std_IN_Wr)) {
        //if (DEBUG) fprintf(stderr, "Unable to cancel IO operations on child's STDIN write handle.\n");
    //}

    /* Wait until child thread exits */
    //if (pipes.piProcInfo.hThread != NULL) {
        //if (DEBUG) fprintf(stderr, "Waiting on child thread\n");
        
       // WaitForSingleObject( pipes.piProcInfo.hThread, INFINITE);
        //if (!TerminateThread(pipes.piProcInfo.hThread, 0)){
            //if (DEBUG)fprintf(stderr, "Error: failed to terminate child thread.\n");
        
        //}
        //CloseHandle(pipes.piProcInfo.hThread);
    //}

    /* Wait until child process exits */
    //if (pipes.piProcInfo.hProcess != NULL) {
        //if (DEBUG) fprintf(stderr, "Waiting on child process\n");
        
        //WaitForSingleObject( pipes.piProcInfo.hProcess, INFINITE );

        //if (!TerminateProcess(pipes.piProcInfo.hProcess, 0)) {
        //    if (DEBUG) fprintf(stderr, "Error: failed to terminate child process. %lu\n", GetLastError());
        //}

        //if (WaitForInputIdle(pipes.piProcInfo.hProcess, INFINITE)){
        //    fprintf(stderr, "Unable to synchronize with child process.\n");
        //}
        //CloseHandle(pipes.piProcInfo.hProcess);
    //}

    if (DEBUG) fprintf(stderr, "Pipes: Waiting for child process to exit.\n");

    // Wait until child process exits.
    WaitForSingleObject( pipes.piProcInfo.hProcess, INFINITE );

    if (DEBUG) fprintf(stderr, "Pipes: Closing handles.\n");
    // Close process and thread handles. 
    CloseHandle( pipes.piProcInfo.hProcess );
    CloseHandle( pipes.piProcInfo.hThread );
    
    //if (pipes.Child_Std_IN_Rd != NULL) {
        //CloseHandle(pipes.Child_Std_IN_Rd);
    //}
    if (pipes.Child_Std_IN_Wr != NULL) {
        CloseHandle(pipes.Child_Std_IN_Wr);
    }
    //if (pipes.Child_Std_OUT_Rd != NULL) {
        //CloseHandle(pipes.Child_Std_OUT_Rd);
   // }
    if (pipes.Child_Std_OUT_Wr != NULL) {
        CloseHandle(pipes.Child_Std_OUT_Wr);
    }
    return 0;
}
