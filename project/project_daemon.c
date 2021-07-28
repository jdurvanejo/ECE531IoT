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
#include <curl/curl.h>
#include <fcntl.h>
#undef _POSIX_SOURCE

#define ERR_WTF	   5
#define ERR_CHDIR  4
#define ERR_SETSID 3
#define ERR_FORK   2
#define OK	   1
#define DAEMON_NAME "projectd"
#define ERROR_FORMAT "%s"

#define URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/"
#define MORNING_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/1"
#define AFTERNOON_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/2"
#define NIGHT_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/3"

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


size_t got_data(char *buffer, size_t itemSize, size_t nItems, void* ignore)
{
    FILE *heater_ptr;
    //char *info;
    size_t bytes = itemSize * nItems;
    heater_ptr = fopen("/var/log/heater","w");
    fprintf(heater_ptr,"%s",buffer);
    fclose(heater_ptr);
    //syslog(LOG_INFO, buffer);
    return bytes;
}


//const char *get_http(char* url)
void get_http(char* url)
{
    curl_global_init(CURL_GLOBAL_ALL);

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl)
    {
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);
	res = curl_easy_perform(curl);
	if(res != 1)
	{
	    //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	//char *response = res;
	//return 1;

    }
}

void post_http(char* url, char* myString)
{
    /*
    time_t time_current;
    struct tm* time_stuff;
    int current_temp;
    char msg_time[];
    time(&time_current);
    time_stuff = localtime(&time_current);
    msg_time = ("%i:%i",time_stuff->tm_hour,time_stuff->tm_min);*/

    CURL* curl;
    CURLcode res;
    char str[4] = "str=";
    char message[100];
    strcat(message, str);
    strcat(message, myString);

    curl = curl_easy_init();
    if (curl) {

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

static void _do_work(void) {
  time_t time_current;
  struct tm *time_stuff;
  FILE *fptr;
  //int test_var;
  int current_temp;
  int set_tmp;
  char* settings;
  char* set_id;
  char* set_time;
  char* set_temp_str;
  char* heater_setting;

  while(1==1)
  {
    //time(&time_current);
    //time_stuff = localtime(&time_current);
    //syslog(LOG_INFO, "current time: %i:%i:%i\n",time_stuff->tm_hour,time_stuff->tm_min,time_stuff->tm_sec);
    // for (int i = 0; true; i++) {
    //   syslog(LOG_INFO, "iteration: %d", i);


    //need to read from the database the different set points
    //syslog(LOG_INFO, "from get: %s",get_http());

    get_http(MORNING_URL);
    //do some stuff to get the time and set point
    fptr = fopen("var/log/heater", "r");
    fscanf(fptr,"%s",&settings);
    set_id = strtok(settings, " ");
    syslog(LOG_INFO, "here's what I got for id: %i", (int)set_id);
    set_time = strtok(NULL, " ");
    syslog(LOG_INFO, "here's what I got for time: %s", set_time);
    set_temp_str = strtok(NULL, " ");
    syslog(LOG_INFO, "here's what I got for id: %i", int(set_temp_str);
    /*set_tmp = (int)set_temp_str;
    if (set_tmp >= current_temp)
    {
        heater_setting = "ON";
    }
    else
    {
        heater_setting = "OFF";
    }*/
    //get_http(AFTERNOON_URL);
    //get_http(NIGHT_URL);
    //read from the temperature file
    fptr = fopen("/var/tmp/temp","r");
    fscanf(fptr,"%i",&current_temp);
    fclose(fptr);
    syslog(LOG_INFO, "current temp: %d\n",current_temp);

    
    //try to write to the heater
    time(&time_current);
    time_stuff = localtime(&time_current);
    fptr = fopen("/var/tmp/status","w");
    fprintf(fptr,"%s : %i:%i:%i",heater_setting,time_stuff->tm_hour,time_stuff->tm_min,time_stuff->tm_sec);
    fclose(fptr);


    //log the current state "time,heater state, set point, current temp"



    sleep(1);
  }
}

int main(void) {

   openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
  syslog(LOG_INFO, "starting projectd");


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

  syslog(LOG_INFO,"Did I make it");
  _do_work();

  return ERR_WTF;

}
