#include"os_log.h"


static char log_file[256];
static FILE *  log_fd;


int os_log_info(const char * fmt,...)
{

    char buf[512];
    va_list vArgList;
    va_start (vArgList, fmt);
    
    int err = vsnprintf(buf, 512, fmt, vArgList);
    
    if(err < 0)
    {
        return -1;
    }
    
    strcat(buf,"\n");
    
    fwrite(buf,strlen(buf),1,stderr);
    

    return 0;
}

int os_log_error(const char * fmt, ...)
{
    char buf[512];
    strcpy(buf,"error:");
    
    va_list vArgList;
    va_start (vArgList, fmt);
    
    int err = vsnprintf(buf+strlen(buf), 512-strlen(buf), fmt, vArgList);
    
    if(err < 0)
    {
        return -1;
    }
    
    sprintf(buf+strlen(buf),"   <SYS> [%d]",errno);
    
    
    fwrite(buf,strlen(buf),1,stderr);
    perror(" ");
    
   
    return 0;
}

int os_log_init(const char * filename)
{


    if(filename != NULL && strlen(filename) < 256)
    {
        strcpy(log_file,filename);
    }
    else
    {
        strcpy(log_file,DEFAULT_LOG_FILE);
    }


    log_fd = fopen(log_file,"a+");
    if(log_fd < 0)
    {
        return -1;
    }
    
#if(PRINT_LOG)


#else
    int err = dup2(fileno(log_fd), fileno(stderr));
#endif

    //os_log_info("log init!");

    //dup2(log_fd, fileno(stderr));
    //os_log_error("%s %d","test dup2 error",58);

    return 0;
    
    
}