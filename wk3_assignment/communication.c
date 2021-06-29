#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define OK        0
#define INIT_ERR  1
#define REQ_ERR   2

#define URL       "http://10.0.2.3:8000"

//create array of verb strings to reference
char in_strings[20][100] = {"-o","--post","-g","--get","-p","--put","-d","--delete","-h","--help"};
int main(int argc, char **argv)
{
    int command = 50;
    CURL    *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    size_t i = 0;
    int arg = 0;

    //look for the url command if not entered then ignore the command
    if (argv[1] == "-u" | argv[1] == "--url")
    {
	arg = 1;
    }
    if (argv[2] == "-u" | argv[2] == "--url")
    {
	arg = 2;
    }
    if (arg != 1 && arg != 2)
    {
	printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
	return 1;
    }
    if (argv[1] == argv[2])
        {
            printf("improper input command format see help (-h) for assistance");
            return 1;
        }


    //check for command argument
    for( i = 0; i < sizeof(in_strings) / sizeof(in_strings[0]); i++)
    {
	if (strcmp(in_strings[i],argv[1]) == 0)
	{
            command = i;
	}
    }

    //assume argv3 is the url and argv4 is the bit to do stuff with
    //URL = argv[3]

    if (command == 0 | command == 1)
    {
	//post
	curl = curl_easy_init();
	if(curl)
	{
            curl_easy_setopt(curl, CURLOPT_URL, URL);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "did it work");
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "post failed: %s\n", curl_easy_strerror(res));
            }
            else {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);
        }
        else
        {
            printf("initialization error");
        }

    }
    else if (command == 2 | command == 3)
    {
	//get
	curl = curl_easy_init();
	if(curl) 
	{
            curl_easy_setopt(curl, CURLOPT_URL, URL);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                //printf(stderr, "get failed: %s\n", curl_easy_strerror(res));
            }
	    else {
		printf("%s\n",res);
	    }
            curl_easy_cleanup(curl);
        }
        else
        {
	    printf("initialization error");
        }
    }
    else if (command == 4 | command == 5)
    {
	//put
	curl = curl_easy_init();
	if(curl) {
	    //curl_easy_setopt(curl,CURLOPT_READFUNCTION, readcallback)
	    //need to add stuff here but don't fully understand
	    res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                //printf(stderr, "put failed: %s\n", curl_easy_strerror(res));
            }
            else {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);

	}
	else
        {
            printf("initialization error");
        }
    }
    else if (command == 6 | command == 7)
    {
	//delete
	curl = curl_easy_init();
        if(curl) {
            //curl_easy_setopt(curl,CURLOPT_READFUNCTION, readcallback);
            //need to add stuff here but don't fully understand
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                //printf(stderr, "put delete: %s\n", curl_easy_strerror(res));
            }
            else {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);

        }
        else
        {
            printf("initialization error");
        }

    }
    else if (command == 8 | command == 9)
    {
	//help
	printf("Hi there, here are a list of the commands that can be used:\n");
	printf("-o/--post -g/--get -p/--put -d/--delete\n");
	printf("an an example if one were to need to get from a specific url\n");
	printf("the line would be entered as './communication -g yoururl.here'\n");
	printf("the same result would come from '.communication --get yoururl.here'\n");
    }
    else if (command == 50)
    {
	printf("I don't know that command\n");
    }
    curl_global_cleanup();
    return OK;
}


