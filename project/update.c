#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


#define OK        0
#define INIT_ERR  1
#define REQ_ERR   2


#define UPDATE_TIME_M_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/time/1"
#define UPDATE_TIME_A_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/time/2"
#define UPDATE_TIME_N_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/time/3"
#define UPDATE_TEMP_M_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/setpt/1"
#define UPDATE_TEMP_A_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/setpt/2"
#define UPDATE_TEMP_N_URL "http://ec2-54-177-45-63.us-west-1.compute.amazonaws.com:3000/update/setpt/3"


//this code is just going to be for updating the time or temperature
//if there is extra time more will be added


void display_help()
{
    //help
    printf("Hi there, here are a list of the commands that can be used:\n");
    printf("-h, --help, -u, -update\n");
    printf("\n");
    printf("The first two will display this message\n");
    printf("The second two are used to update the thermostat settings\n");
    printf("\n");
    printf("There are 3 set points: Morning, Afternoon and Night\n");
    printf("Currently there are only two settings that may be adjusted\n");
    printf("for each of the three set points: time and temperature\n");
    printf("To specify the part of the day you only need to use 'm' 'a' or 'n'\n");
    printf("to refer to either morning, afternoon or night.\n");
    printf("The time must be military time and the temperature must be a whole number no decimals.\n");
    printf("\n");
    printf("Examples: \n");
    printf("To adjust the time please follow the following format:\n");
    printf("./update -u temp <time of day> <temperature>\n");
    printf("./update -u temp m 70  <- this would reset the morning temperature to 70 degrees\n");
    printf("To adjust the temperature use the following:\n");
    printf("./update -u time <time of day> <hour> <minute>\n");
    printf("./update -u time a 14 30\n");
    printf("This would reset the afternoon time setting to 14:30.\n");
    printf("\n");
    printf("if more assistance is required please contact jdurvanejo@unm.edu");
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

static size_t callback(char *buff, size_t item_size, size_t item_number, void* additional)
{
    size_t num_bytes = item_size*item_number;
    printf("new Chunk (%zu bytes)\n", num_bytes);
    for (int i = 0; i < num_bytes; i++) {
	printf("%c",buff[i]);
    }
    printf("\n\n");
    return num_bytes;
}



int main(int argc, char** argv)
{
    //create array of verb strings to reference
    char in_strings[20][100] = { "-h","--help","-u","--update", "m", "a" "n" };

    int set_temp;
    int set_hour;
    int set_min;
    FILE* fptr;

    char* to_send = 0;
    long length;

    char* current_url;
    //filter the input

    if (argc != 2 && argc != 5 && argc != 6)
    {
        printf("I can't understand that command. There is an incorrect number of arguments see help (-h) for details.");
            return OK;
    }


    if (argc == 2)
    {
        //help
        if (strcmp(argv[1], in_strings[0]) == 0 || strcmp(argv[1], in_strings[1]) == 0)
        {
            display_help();
            return OK;
        }
        else
        {
            printf("I don't know that command see help (-h) for details.");
            return OK;
        }
    }

    if (argc == 5)
    {
        //temperature update
        if (strcmp(argv[1], in_strings[2]) == 0 || strcmp(argv[1], in_strings[3]) == 0)
        {
            if (strcmp(argv[2], "temp") == 0)
            {
                //check time of day
                if (strcmp(argv[3], in_strings[4]) == 0)
                {
                    //morning
                    current_url = UPDATE_TEMP_M_URL;
                    set_temp = atoi(argv[4]);
                    if (set_temp > 100 || set_temp < 30)
                    {
                        printf("Woah that's a pretty wild temperature, I'm going to ignore that.");
                    }
                }
                else if (strcmp(argv[3], in_strings[5]) == 0)
                {
                    //afternoon
                    current_url = UPDATE_TEMP_A_URL;
                    set_temp = atoi(argv[4]);
                    if (set_temp > 100 || set_temp < 30)
                    {
                        printf("Woah that's a pretty wild temperature, I'm going to ignore that.");
                    }
                }
                else if (strcmp(argv[3], in_strings[6]) == 0)
                {
                    //night
                    current_url = UPDATE_TEMP_N_URL;
                    set_temp = atoi(argv[4]);
                    if (set_temp > 100 || set_temp < 30)
                    {
                        printf("Woah that's a pretty wild temperature, I'm going to ignore that.");
                    }
                }
                else
                {
                    printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                    return OK;
                }

                //generate the string to send over
                fptr = fopen("/home/jason/Documents/temporary", "w");
                fprintf(fptr, "setpt=%i", set_temp);
                fclose(fptr);

                fptr = fopen("/home/jason/Documents/temporary", "rb");
                fseek(fptr, 0, SEEK_END);
                length = ftell(fptr);
                fseek(fptr, 0, SEEK_SET);
                to_send = malloc(length);
                if (to_send)
                {
                    fread(to_send, 1, length, fptr);
                }
                fclose(fptr);

                printf("%s", to_send);
		printf("%s", current_url);
                //send the command over
                //post_http(LOG_URL, to_send);
            }
            else
            {
                printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                return OK;
            }
        }
        else
        {
            printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
            return OK;
        }
    }

    if (argc == 7)
    {
        //time update
        if (strcmp(argv[1], in_strings[2]) == 0 || strcmp(argv[1], in_strings[3]) == 0)
        {
            if (strcmp(argv[2], "time") == 0)
            {
                //check time of day
                if (strcmp(argv[3], in_strings[4]) == 0)
                {
                    //morning
                    //check hour
                    current_url = UPDATE_TIME_M_URL;
                    set_hour = atoi(argv[4]);
                    if (set_hour < 0 || set_hour > 24)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                    //check min
                    set_min = atoi(argv[4]);
                    if (set_min < 0 || set_min > 59)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                }
                else if (strcmp(argv[3], in_strings[5]) == 0)
                {
                    //afternoon
                    //check hour
                    current_url = UPDATE_TIME_A_URL;
                    set_hour = atoi(argv[4]);
                    if (set_hour < 0 || set_hour > 24)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                    //check min
                    set_min = atoi(argv[4]);
                    if (set_min < 0 || set_min > 59)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                }
                else if (strcmp(argv[3], in_strings[6]) == 0)
                {
                    //night
                    //check hour
                    current_url = UPDATE_TIME_N_URL;
                    set_hour = atoi(argv[4]);
                    if (set_hour < 0 || set_hour > 24)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                    //check min
                    set_min = atoi(argv[4]);
                    if (set_min < 0 || set_min > 59)
                    {
                        printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                        return OK;
                    }
                }
                else
                {
                    printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                    return OK;
                }

                //generate the string to send over
                //fptr = fopen("/var/log/poster", "w");
                fptr = fopen("/home/jason/Documents/temporary","w");
                fprintf(fptr, "time=%i", set_hour);
                fprintf(fptr, ":%i", set_min);
                fclose(fptr);

                fptr = fopen("/home/jason/Documents/temporary", "rb");
                fseek(fptr, 0, SEEK_END);
                length = ftell(fptr);
                fseek(fptr, 0, SEEK_SET);
                to_send = malloc(length);
                if (to_send)
                {
                    fread(to_send, 1, length, fptr);
                }
                fclose(fptr);

                printf("%s",to_send);
                //send the command over




            }
            else
            {
                printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
                return OK;
            }
        }
        else
        {
            printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
            return OK;
        }
    }

}
