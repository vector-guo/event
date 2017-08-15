#ifndef __OS__LOG__H
#define __OS__LOG__H

#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#define DEFAULT_LOG_FILE "log.txt"
#define PRINT_LOG  1


int os_log_init(const char * filename);
int os_log_info(const char * fmt,...);
int os_log_error(const char * fmt, ...);

#endif