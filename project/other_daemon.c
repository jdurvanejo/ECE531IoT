//used arm-linux-gcc to compile

#define _POSIX_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
//#include <stdbool.h>
#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#undef _POSIX_SOURCE

#define ERR_WTF	   5
#define ERR_CHDIR  4
#define ERR_SETSID 3
#define ERR_FORK   2
#define OK	   1
#define DAEMON_NAME "otherd"
#define ERROR_FORMAT "%s"

static void _signal_handler(const int signal) {
  switch(signal) {
    case SIGHUP:
      break;
    case SIGTERM:
      syslog(LOG_INFO, "received SIGTERM, exiting.");
      closelog();
      exit(OK);
      break;
    default:
      syslog(LOG_INFO, "received unhandled signal");
  }
}

static void _do_work(void) {
  time_t time_current;
  struct tm *time_stuff;
  FILE *fptr;
  //int test_var;
  int current_temp;

  while(1==1)
  {
    //read from the temperature file
    fptr = fopen("/var/tmp/temp","r");
    fscanf(fptr,"%d",&current_temp);
    fclose(fptr);
    syslog(LOG_INFO, "current temp: %d\n",current_temp);

    //try to write to the heater
    time(&time_current);
    time_stuff = localtime(&time_current);
    fptr = fopen("/var/tmp/status","w");
    fprintf(fptr,"OFF : %i:%i:%i",time_stuff->tm_hour,time_stuff->tm_min,time_stuff->tm_sec);
    fclose(fptr);
    sleep(1);
  }
}

int main(void) {

   openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
  syslog(LOG_INFO, "starting otherd");


  pid_t pid = fork();
  if (pid < 0) {
    syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
    return ERR_FORK;
  }

  if (pid > 0) {
    return OK;
  }

  if (setsid() < -1) {
    syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
    return ERR_SETSID;
  }


  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  if (chdir("/") < 0) {
    syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
    return ERR_CHDIR;
  }

  signal(SIGTERM, _signal_handler);
  signal(SIGHUP,_signal_handler);

  syslog(LOG_INFO,"Am i making it this far?");
  _do_work();

  return ERR_WTF;

}
