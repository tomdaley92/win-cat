/*

Thomas Daley
10/16/2016
main.cc

WinCat - A minimal windows implementation of the netcat tool.

*/

#include "options.h"
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

const char *version = "WinCat - v1.04\n";

const char *about = "A simple TCP/IP network debugging utility for Windows.\n"
                    "Inspired by the traditional nc we all know and love.\n";

const char *usage = "usage: wc [-lkszrwhvec] [host] [port]\n";

const char *details =
                    "   l              Listen for incoming connections. It is an error to\n"
                    "                  use this option with a host specified.\n"
                    "\n"
                    "   k              Keep listening. Forces wc to stay listening \n"
                    "                  for another connection after its current\n"
                    "                  connection is completed. It is an error to use\n"
                    "                  this option without -l.\n"
                    "\n"
                    "   s              Specify host(s) on the network to send ICMP echo\n"
                    "                  requests. The default timeout is 900 milliseconds.\n"
                    "                  e.g.    wc -s 10.0.0.0/24\n"
                    "\n"
                    "   z              Specify port(s) on the host to scan for listening\n"
                    "                  daemons using the connect() call. The default\n"
                    "                  timeout is 750 milliseconds."
                    "                  e.g.    wc -z localhost 1-200\n"
                    "\n"
                    "   r              Do a reverse dns lookup with ICMP echo requests.\n"
                    "                  e.g.    wc -sr 10.0.0.0/24\n"
                    "\n"
                    "   w  timeout     The timeout in milliseconds for pings and connect\n"
                    "                  scans.\n"
                    "                  e.g.    wc -zw 100 localhost 1-200\n"
                    "\n"
                    "   c  command     Specify a command to pass to \"cmd /c\" for\n"
                    "                  execution after connecting. It is an error\n"
                    "                  to use this option with -e, -s, or -z.\n"
                    "                  e.g.    host A (10.0.0.2): wc -l -c whoami 8118\n"
                    "                          host B (10.0.0.3): wc 10.0.0.2 8118\n"
                    "\n"
                    "   e  filename    Specify a filename to execute after connecting\n"
                    "                  (use with caution). It is an error to use this\n"
                    "                  option with -c, -s, or -z.\n"
                    "                  e.g.    host A (10.0.0.2): wc -lk -e cmd 8118\n"
                    "                          host B (10.0.0.3): wc 10.0.0.2 8118\n"
                    "\n"
                    "   h              Print this help page.\n"
                    "\n"
                    "   v              Print version information.\n"
                    "\n"
                    "   host           Can be a numerical address or a symbolic\n"
                    "                  hostname. If the -s option is specified, CIDR\n"
                    "                  notation (IPv4 only) can be used to specify a\n"
                    "                  range of hosts.\n"
                    "\n"
                    "   port           Must be single integer. If the -z option\n"
                    "                  is specified, a range of ports can be used instead.\n";               

int print_usage() {
    fprintf(stderr, "%s", usage);
    return 1;
}

int print_help() {
    fprintf(stdout, "%s\n%s\n%s\n%s", version, about, usage, details);
    return 0;
}

int print_version() {
    fprintf(stdout, "%s", version);
    return 0;
}


int main(int argc, char **argv) {
    int total_unique_opts = pre_parse_opts(argc, argv);
    
    char opt;
    while (opt = get_opt(argc, argv)) {
        switch(opt) {
            case 'l':
                {   
                    if (opt_count('e') && opt_count('c')) return print_usage();

                    argind += opt_arg('e')? 1 : 0;
                    argind += opt_arg('c')? 1 : 0;

                    if (argind >= argc) return print_usage();

                    if (DEBUG) {
                        if (opt_count('k')) fprintf(stderr, "-k\n");
                        if (opt_count('e')) fprintf(stderr, "-e %s\n", opt_arg('e'));
                        if (opt_count('c')) fprintf(stderr, "-c %s\n", opt_arg('c'));
                        fprintf(stderr, "port %s\n", argv[argind]);
                    }

                    if (opt_count('e')) {
                        return server(argv[argind], opt_arg('e'), opt_count('k') ? 1 : 0);
                    } 

                    if (opt_count('c')) {
                        char command[MAX_LINE] = {0};
                        memcpy(command, "cmd /c ", 7);
                        memcpy(command+7, opt_arg('c'), strlen(opt_arg('c')));
                        return server(argv[argind], command, opt_count('k') ? 1 : 0);
                    }

                    return server(argv[argind], NULL, opt_count('k') ? 1 : 0);
                }
            case 's':
                {
                    argind += opt_arg('w')? 1 : 0;

                    if (opt_arg('r')) {
                        if (strcmp(opt_arg('r') ? opt_arg('r') : "", opt_arg('w') ? opt_arg('w') : "")) argind--;
                    }
                    if (opt_arg('w')) {
                        if (strcmp(opt_arg('w') ? opt_arg('w') : "", opt_arg('s') ? opt_arg('s') : "")) argind--;
                    }

                    if (DEBUG) {
                        if (opt_count('s')) fprintf(stderr, "-s\n");
                        if (opt_count('r')) fprintf(stderr, "-r\n");
                        if (opt_arg('w')) fprintf(stderr, "-w arg: %s\n", opt_arg('w'));
                        if (argind < argc) fprintf(stderr, "cidr: %s\n", argv[argind]);
                    }

                    if (argind >= argc) return print_usage();

                    return ping_scan(argv[argind], opt_arg('w') ? atoi(opt_arg('w')) : 900, opt_count('r') ? 1: 0);
                }
            case 'z': 
                {   
                    if (opt_count('w') && !opt_arg('w')) return print_usage();
                    
                    argind += opt_arg('w')? 1 : 0;

                    if (opt_arg('z') && opt_arg('w')) {
                        if (strcmp(opt_arg('w'), opt_arg('z'))) argind--;
                    }

                    if (DEBUG) {
                        if (opt_count('z')) fprintf(stderr, "-z\n");
                        if (opt_arg('w')) fprintf(stderr, "-w arg: %s\n", opt_arg('w'));
                        if (argind < argc-1) fprintf(stderr, "host: %s\nport: %s\n", argv[argind], argv[argind+1]);
                    }

                    if (argind >= argc-1) return print_usage();

                    int low;
                    int high;
                    char *start = strtok(argv[argind+1], "-");
                    low = atoi(start);
                    char *end = strtok(NULL, "-");
                    if (end == NULL)  high = low;
                    else high = atoi(end);
                    
                    return connect_scan(argv[argind], low, high, opt_arg('w') ? atoi(opt_arg('w')) : 750);
                }
            case 'h': 
                return print_help();
            case 'v': 
                return print_version();
            case 'c':
                {
                    if (opt_count('e') || opt_count('z') || opt_count('s')) return print_usage();
                    
                    if (!opt_count('l')) {
                        
                        if (opt_arg('c')) argind++;

                        if (argind >= argc-1) return print_usage();

                        if (DEBUG) {
                            fprintf(stderr, "-c arg: %s\n", opt_arg('c'));
                            fprintf(stderr, "host: %s\nport: %s\n", argv[argind], argv[argind+1]);
                        }

                        char command[MAX_LINE] = {0};
                        memcpy(command, "cmd /c ", 7);
                        memcpy(command+7, opt_arg('c'), strlen(opt_arg('c')));
                        return client(argv[argind], argv[argind+1], command);
                    }

                }
                break;
            case 'e':
                {
                    if (opt_count('c') || opt_count('z') || opt_count('s')) return print_usage();
                    
                    if (!opt_count('l')) {
                        
                        if (opt_arg('e')) argind++;

                        if (argind >= argc-1) return print_usage();

                        if (DEBUG) {
                            fprintf(stderr, "-e arg: %s\n", opt_arg('e'));
                            fprintf(stderr, "host: %s\nport: %s\n", argv[argind], argv[argind+1]);
                        }

                        return client(argv[argind], argv[argind+1], opt_arg('e'));
                    }
                }
                break;
            default:
                if (DEBUG) fprintf(stderr, "? -%c\n", opt);
                break;
        }
    }

    /* No options specified */
    if (argind >= argc-1) return print_usage();
    if (DEBUG) fprintf(stderr, "host: %s\nport: %s\n", argv[argind], argv[argind+1]);
    return client(argv[argind], argv[argind+1], NULL);

    if (DEBUG) fprintf(stderr, "ExitProcess()\n");
    
    return 0;
}
