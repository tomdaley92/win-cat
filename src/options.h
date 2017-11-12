/*

Thomas Daley
11/5/2017
options.h

*/

#ifndef OPTIONS_H
#define OPTIONS_H

/* 1st method: Parse once then call 
   opt_count or opt_arg freely */
int pre_parse_opts(int argc, char **argv);
int opt_count(char opt);
char *opt_arg(char opt);
extern int argind;

/* 2nd method: Incremental arg parsing 
   similar to get_opt */
char get_opt(int argc, char **argv);
extern char *optarg;
extern int optind;

#endif /* OPTIONS_H */
