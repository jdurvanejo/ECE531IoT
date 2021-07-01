#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define OK        0
#define INIT_ERR  1
#define REQ_ERR   2


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

//create array of verb strings to reference
char in_strings[20][100] = {"-o","--post","-g","--get","-p","--put","-d","--delete","-h","--help"};


int main(int argc, char **argv)
{
    int command = 50;
    int arg_cmd = 1;
    int arg_url = 2;
    FILE * data_src;
    char *file;
    struct stat file_info;
    CURL    *curl;
    CURLcode res;
    //curl_global_init(CURL_GLOBAL_ALL);
    size_t i = 0;

    //filter the input
    if (argc > 2)
    {
        if (strcmp(argv[1], "-h") == 0 | strcmp(argv[1], "--help") == 0)
        {
            command = 8;
        }
        else
        {
            printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
            return 1;
        }
    }
    else
    {
        //look for the url command if not entered then ignore the command
	    if (strcmp(argv[1],"-u") == 0 | strcmp(argv[1],"--url") == 0)
	    {
            arg_url = 1;
	    }
	    if (strcmp(argv[2],"-u") == 0 | strcmp(argv[2],"--url") == 0)
	    {
            arg_url = 2;
	    }
	    if (arg_url != 1 && arg_url != 2)
	    {
            //no url command in arguments
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
	        if (arg_url = 2)
	        {
		        arg_cmd = 1;
	        }
	        else
	        {
		        arg_cmd = 2;
	        }
	        if (strcmp(in_strings[i],argv[arg_cmd]) == 0)
	        {
        	    command = i;
	        }
	    }
    }
    
    
    //assume argv3 is the url and argv4 is the bit to do stuff with
    if (command == 0 | command == 1)
    {
	    //post
        if (argc < 5)
        {
            printf("ERROR: not enough arguments\n");
            return REQ_ERR;
        }
	    curl = curl_easy_init();
	    if(curl)
	    {
            curl_easy_setopt(curl, CURLOPT_URL, argv[3]);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, argv[4]);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) 
            {
                fprintf(stderr, "post failed: %s\n", curl_easy_strerror(res));
                return REQ_ERR;
            }
            else 
            {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);
        }
        else
        {
            printf("initialization error");
            return INIT_ERR;
        }

    }
    else if (command == 2 | command == 3)
    {
	    //get
	    curl = curl_easy_init();
	    if(curl) 
	    {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "get failed: %s\n", curl_easy_strerror(res));
                return REQ_ERR;
            }
	        else 
            {
	    	    printf("%s\n",res);
	        }
            curl_easy_cleanup(curl);
        }
        else
        {
	        printf("initialization error");
            return INIT_ERR;
        }
    }
    else if (command == 4 | command == 5)
    {
	    //put
	    if (argc < 5) 
        {
            printf("ERROR: not enough arguments\n");
            return REQ_ERR;
	    }
	    file = argv[4];
	    stat(file, &file_info);
	    data_src = fopen(argv[4], "rb");
	    curl = curl_easy_init();
	    if(curl) {
	        curl_easy_setopt(curl, CURLOPT_URL, argv[3]);
	        curl_easy_setopt(curl,CURLOPT_READFUNCTION,&callback);
	        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	        curl_easy_setopt(curl, CURLOPT_READDATA, data_src);
	        res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "put failed: %s\n", curl_easy_strerror(res));
                return REQ_ERR;
            }
            else {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);
	    }
	    else
        {
            printf("initialization error");
            return INIT_ERR;
        }
	    fclose(data_src);
	    curl_global_cleanup();
    }
    else if (command == 6 | command == 7)
    {
	    //delete
        if (argc < 5)
        {
            printf("ERROR: not enough arguments\n");
            return REQ_ERR;
        }
	    curl = curl_easy_init();
        if(curl) {
	        curl_easy_setopt(curl, CURLOPT_URL, argv[3]);
	        curl_easy_setopt(curl,CURLOPT_VERBOSE,1L);
	        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,"delete");
            //not totally sue if implemented correctly
            //Is more supposed to be sent with it?
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                fprintf(stderr, "put delete: %s\n", curl_easy_strerror(res));
                return REQ_ERR;
            }
            else {
                printf("%s\n",res);
            }
            curl_easy_cleanup(curl);

        }
        else
        {
            printf("initialization error");
            return INIT_ERR;
        }

    }
    else if (command == 8 | command == 9)
    {
	    //help
	    printf("Hi there, here are a list of the commands that can be used:\n");
	    printf("-o/--post -g/--get -p/--put -d/--delete\n");
	    printf("\n");
	    printf("In addition to the commnd the url must be specified as well\n");
	    printf("As an example, for a simple get command must be entered as follows: \n");
	    printf("./wk3-assignment -g -u <url to communicate with>\n");
	    printf("\n");
	    printf("This is the same as: \n");
	    printf("./wk3-assignment --get --url <url to communicate with>\n");
	    printf("if more assistance is required please contact jdurvanejo@unm.edu");
    }
    else if (command == 50)
    {
	    printf("There is an issue with the command you enetered, see help for assistance (-h)\n");
        return REQ_ERR;
    }
    curl_global_cleanup();
    return OK;
}


