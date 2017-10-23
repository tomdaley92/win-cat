/*

Thomas Daley
10/16/2016
main.cc

WinCat - A minimal windows implementation of the netcat tool.

*/
#include "server.h"
#include "client.h"
#include "scan.h"
#include <stdlib.h>
#include <stdio.h>
#include <String.h>
#include <windows.h>
#include <tchar.h>

#define DEBUG 0
#define MAX_LINE 1025

const char *title = "WinCat - v1.04\n";

const char *about = "A simple TCP/IP network debugging utility for Windows.\n"
                    "Inspired by the traditional nc we all know and love.\n";

const char *usage = "usage: wincat [-lkszh] [--e filename] [--c string] [host] [port]\n";

const char *details =
                    "   -l   : Listen for incoming connections. It is an error to\n"
                    "          use this option with a host specified.\n"
                    "\n"
                    "   -k   : Keep listening. Forces wincat to stay listening \n"
                    "          for another connection after its current\n"
                    "          connection is completed. It is an error to use\n"
                    "          this option without -l.\n"
                    "\n"
                    "   -s   : Specify host(s) on the network to send ICMP echo\n"
                    "          requests. It is an error to use this option with\n"
                    "          any other options specified.\n"
                    "          e.g.    wincat -s 192.168.1.0/24\n"
                    "\n"
                    "   -z   : Specify port(s) on the host to scan for listening\n"
                    "          daemons using the connect() call. It is an error\n"
                    "          to use this option with any other options\n"
                    "          specified.\n"
                    "          e.g.    wincat -z localhost 1-200\n"
                    "\n"
                    "   --c  : Specify commands to pass to \"cmd /c\" for\n"
                    "          execution after connecting. It is an error\n"
                    "          to use this option with --e, -s, or -z.\n"
                    "          e.g.    host A (10.0.0.2): wincat -l --c whoami 8118\n"
                    "                  host B (10.0.0.3): wincat 10.0.0.2 8118\n"
                    "\n"
                    "   --e  : Specify filename to execute after connecting\n"
                    "          (use with caution). It is an error to use this\n"
                    "          option with --c, -s, or -z.\n"
                    "          e.g.    host A (10.0.0.2): wincat -lk --e cmd 8118\n"
                    "                  host B (10.0.0.3): wincat 10.0.0.2 8118\n"
                    "\n"
                    "   -h   : Displays this help page, when this option\n"
                    "          is specified.\n"
                    "\n"
                    "  host  : Can be a numerical address or a symbolic\n"
                    "          hostname. If the -s option is specified, CIDR\n"
                    "          notation (IPv4 only) can be used to specify a\n"
                    "          range of hosts.\n"
                    "\n"
                    "  port  : Must be single integer. If the -z option\n"
                    "          is specified, a range of ports can be used instead.\n";
                    

int main(int argc, char **argv) {
    int exit_code = 0;

    switch (argc) {
        case 2:
            if (!strcmp(argv[1], "-h"))
                fprintf(stdout, "%s\n%s\n%s\n%s",title, about, usage, details);
            else 
                fprintf(stderr, "%s", usage);
            break;
        case 3:
            if (!strcmp(argv[1], "-s")) 
                exit_code = ping_scan(argv[2], 1000);
            else if (!strcmp(argv[1], "-l")) 
                exit_code = server(argv[2], NULL, 0);
            else if (!strcmp(argv[1], "-lk")) 
                exit_code = server(argv[2], NULL, 1);
            else 
                exit_code = client(argv[1], argv[2], NULL); 
            break;
        case 4:
            if (!strcmp(argv[1], "-z")) {
                int low;
                int high;
                char *start = strtok(argv[3], "-");
                low = atoi(start);
                char *end = strtok(NULL, "-");
                if (end == NULL)  high = low;
                else high = atoi(end);
                exit_code = connect_scan(argv[2], low, high, 750);
            } else {
                fprintf(stderr, "%s", usage);
                exit_code = 1;
            }
            break;
        case 5:
            char command[MAX_LINE];
            memset(command, '\0', MAX_LINE);
            if (!strcmp(argv[1], "--e")) 
                exit_code = client(argv[3], argv[4], argv[2]);
            else if (!strcmp(argv[1], "--c")) {
                memcpy(command, "cmd /c ", 7);
                memcpy(command+7, argv[2], strlen(argv[2]));
                exit_code = client(argv[3], argv[4], command);
            } else if (!strcmp(argv[1], "-l") && !strcmp(argv[2], "--e"))
                exit_code = server(argv[4], argv[3], 0);
             else if (!strcmp(argv[1], "-l") && !strcmp(argv[2], "--c")) {
                memcpy(command, "cmd /c ", 7);
                memcpy(command+7, argv[3], strlen(argv[3]));
                exit_code = server(argv[4], command, 0);
            } else if (!strcmp(argv[1], "-lk") && !strcmp(argv[2], "--e"))
                exit_code = server(argv[4], argv[3], 1);
            else if (!strcmp(argv[1], "-lk") && !strcmp(argv[2], "--c")) {
                memcpy(command, "cmd /c ", 7);
                memcpy(command+7, argv[3], strlen(argv[3]));
                exit_code = server(argv[4], command, 1);
            } else {
                fprintf(stderr, "%s", usage);
                exit_code = 1;
            }
            break;
        default:
            fprintf(stderr, "%s", usage);
            exit_code = 1;
    }

    if (DEBUG) fprintf(stderr, "ExitProcess()\n");
    return exit_code;
}
