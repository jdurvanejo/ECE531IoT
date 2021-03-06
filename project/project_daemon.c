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
#define LOG_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/logging"

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


size_t incoming(char *buffer, size_t itemSize, size_t nItems, void* ignore)
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
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, incoming);
	res = curl_easy_perform(curl);
	if(res != 1)
	{
	    //fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);
	//return 1;

    }
}

void post_http(char* url, char* myString)
{

    CURL* curl;
    CURLcode res;
    char message[100];
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

  //int current_temp;
  char * current_temp;
  int current_temp_int;
  int set_tmp;

  char* set_id;
  char* set_time;
  char* set_temp_str;
  char* heater_setting;
  char* current_time;
  char* temp_hr;
  char* temp_min;
  char * buffer = 0;
  long length;


  char* hr_morn;
  char* hr_afnn;
  char* hr_nght;
  char* min_morn;
  char* min_afnn;
  char* min_nght;

  int hr_morn_int;
  int hr_afnn_int;
  int hr_nght_int;

  int min_morn_int;
  int min_afnn_int;
  int min_nght_int;

  int temp_morn;
  int temp_afnn;
  int temp_nght;



  char* to_send = 0;

  sleep(3);
  while(1==1)
  {

    //need to read from the database the different set points
    //////////////////////////////////////////
    //morning get
    get_http(MORNING_URL);
    //do some stuff to get the time and set point
    fptr = fopen("/var/log/heater", "rb");
    fseek (fptr, 0, SEEK_END);
    length = ftell (fptr);
    fseek (fptr, 0, SEEK_SET);
    buffer = malloc (length);
    if (buffer)
    {
	fread (buffer, 1, length, fptr);
    }
    fclose (fptr);
    syslog(LOG_INFO, "%s", buffer);    


    set_id = strtok(buffer, " ");
    set_time = strtok(NULL," ");
    set_temp_str = strtok(NULL, " ");
    syslog(LOG_INFO, "here's what I got for the first: id:%s time:%s temp:%s",set_id,set_time,set_temp_str);
    set_tmp = atoi(set_temp_str);
    temp_morn = atoi(set_temp_str);

    hr_morn = strtok(set_time, ":");
    hr_morn_int = atoi(hr_morn);
    //syslog(LOG_INFO, "morn hr: %i", int_hr_morn);
    min_morn = strtok(NULL, " ");
    min_morn_int = atoi(min_morn);
    //syslog(LOG_INFO, "morn min: %i", int_min_morn);
    ///////////////////////////////////////////////////////


    //////////////////////////////////////////////////
    //afternoon get
    get_http(AFTERNOON_URL);
    //do some stuff to get the time and set point
    fptr = fopen("/var/log/heater", "rb");
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, fptr);
    }
    fclose(fptr);
    syslog(LOG_INFO, "%s", buffer);


    set_id = strtok(buffer, " ");
    set_time = strtok(NULL, " ");
    set_temp_str = strtok(NULL, " ");
    syslog(LOG_INFO, "here's what I got for the second: id:%s time:%s temp:%s", set_id, set_time, set_temp_str);
    set_tmp = atoi(set_temp_str);
    temp_afnn = atoi(set_temp_str);

    hr_afnn = strtok(set_time, ":");
    hr_afnn_int = atoi(hr_afnn);
    //syslog(LOG_INFO, "afnn hr: %s", hr_afnn);
    min_afnn = strtok(NULL, " ");
    min_afnn_int = atoi(min_afnn);
    //syslog(LOG_INFO, "morn min: %s", min_afnn);
    ///////////////////////////////////////////////////////


     //////////////////////////////////////////////////
    //night get
    get_http(NIGHT_URL);
    //do some stuff to get the time and set point
    fptr = fopen("/var/log/heater", "rb");
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    buffer = malloc(length);
    if (buffer)
    {
        fread(buffer, 1, length, fptr);
    }
    fclose(fptr);
    syslog(LOG_INFO, "%s", buffer);


    set_id = strtok(buffer, " ");
    set_time = strtok(NULL, " ");
    set_temp_str = strtok(NULL, " ");
    syslog(LOG_INFO, "here's what I got for the third: id:%s time:%s temp:%s", set_id, set_time, set_temp_str);
    set_tmp = atoi(set_temp_str);
    temp_nght = atoi(set_temp_str);

    hr_nght = strtok(set_time, ":");
    hr_nght_int = atoi(hr_nght);
    //syslog(LOG_INFO, "afnn hr: %s", hr_nght);
    min_nght = strtok(NULL, " ");
    min_nght_int = atoi(min_nght);
    //syslog(LOG_INFO, "morn min: %s", min_nght_int);
    ///////////////////////////////////////////////////////

    //now I know the setpoints and when they go into effect
    //check the current time to compare to

    time(&time_current);
    time_stuff = localtime(&time_current);
    int current_hour = time_stuff->tm_hour;
    int current_min = time_stuff->tm_min;

    if (current_hour < hr_morn_int || current_hour > hr_nght_int)
    {
        //night
        set_tmp = temp_nght;

    }
    else if (current_hour == hr_morn_int)
    {
        if (current_min >= min_morn_int)
        {
            //morning
            set_tmp = temp_morn;
        }
        else
        {
            // still night
            set_tmp = temp_nght;
        }
    }
    else if (current_hour < hr_afnn_int && current_hour > hr_morn_int)
    {
        //morning
        set_tmp = temp_morn;
    }
    else if (current_hour == hr_afnn_int)
    {
        if (current_min >= min_afnn_int)
        {
            //afternoon
            set_tmp = temp_nght;
        }
        else
        {
            // still morning
            set_tmp = temp_afnn;
        }
    }
    else if (current_hour <= hr_nght_int && current_hour >= hr_afnn_int)
    {
        //afternoon
        set_tmp = temp_afnn;
    }
    else if (current_hour == hr_nght_int)
    {
        if (current_min >= min_nght_int)
        {
            //night
            set_tmp = temp_nght;
        }
        else
        {
            // still afternoon
            set_tmp = temp_afnn;
        }
    }


    //read from the temperature file
    /*
    fptr = fopen("/var/tmp/temp", "r");
    fscanf(fptr, "%s", &current_temp);
    fclose(fptr);
    current_temp_int = atoi(current_temp);
    syslog(LOG_INFO, "current temp: %i\n", current_temp_int);
    */

    fptr = fopen("/var/tmp/temp", "rb");
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    current_temp = malloc(length);
    if (current_temp)
    {
        fread(current_temp, 1, length, fptr);
    }
    fclose(fptr);
    current_temp_int = atoi(current_temp);
    syslog(LOG_INFO, "Current temp: %i", current_temp);

    //compare the two temperatures
    if (set_tmp >= current_temp_int)
    {
        heater_setting = "ON";
    }
    else
    {
        heater_setting = "OFF";
    }
    //syslog(LOG_INFO, "%s", heater_setting);

    //try to write to the heater
    fptr = fopen("/var/tmp/status", "w");
    fprintf(fptr, "%s : %i:%i:%i", heater_setting, time_stuff->tm_hour, time_stuff->tm_min, time_stuff->tm_sec);
    fclose(fptr);


    ///////////////////////////////////////////////////////
    //send back the log
    fptr = fopen("/var/log/poster", "w");
    fprintf(fptr, "hour=%i", time_stuff->tm_hour);
    fprintf(fptr, "&min=%i", time_stuff->tm_min);
    fprintf(fptr, "&heater=%s", heater_setting);
    fprintf(fptr, "&setpt=%s",set_temp_str);
    fprintf(fptr, "&actual=%s", current_temp);
    fclose(fptr);

    fptr = fopen("/var/log/poster", "rb");
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    to_send = malloc(length);
    if (to_send)
    {
        fread(to_send, 1, length, fptr);
    }
    fclose(fptr);
    syslog(LOG_INFO, "%s", to_send);


    post_http(LOG_URL,to_send);

    ///////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
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
