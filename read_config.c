#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_config.h"

void del_line(char *src){
    if (src[strlen(src)-1] == '\n') src[strlen(src)-1] ='\0';
}
int find(char *src, char *key){
    int c;
    int b;
    char *str;
    str = strstr(src,key);
    if (str != NULL){
    c = (int) src;
    b = (int) str;
    return b-c;
    }
    else return -1;
}
char *trim(char *src, int start, int end){
    char *val;
    val = malloc(end-start+1);
    int i;
    for (i = start; i < end; i++){
        val[i-start] = (char)src[i]; 
    }
    return val;
}
void read_config(struct config *ar_config, char *dir)
{
    int num;
    FILE *fptr;
    int line_size;
    char str_line[300];
    int count = -1;;
    int index = 0;
    fptr = fopen(dir,"r");

    if(fptr == NULL)
    {
        printf("Error!");   
        exit(1);             
    }
    while(fgets(str_line, 300, fptr) != NULL){
        if (find(str_line, "account") != -1) count++;
        index = find(str_line, "=");
        if (index != -1){
            char *key = trim(str_line,0,index);
            char *value = trim(str_line, index + 1, strlen(str_line)-1);
            if (strcmp(key, "contact") == 0){
                strcpy(ar_config[count].contact, trim(str_line,index+1,find(str_line,";transport")));
            }
            else if (strcmp(key, "sip_port") == 0){
                ar_config[count].sip_port = atoi(value);
            }
            else if (strcmp(key, "sip_tcp_port") == 0){
                ar_config[count].sip_tcp_port = atoi(value);
            }
            else if (strcmp(key, "sip_tls_port") == 0){
                ar_config[count].sip_tls_port = atoi(value);
            }
            else if (strcmp(key, "inc_timeout") == 0){
                ar_config[count].inc_timeout = atoi(value);
            }
            else if (strcmp(key, "playback_dev_id") == 0){
                ar_config[count].playback_dev_id = atoi(value);
                printf("%d \n",ar_config[count].playback_dev_id);
            }
            else if (strcmp(key, "capture_dev_id") == 0){
                ar_config[count].capture_dev_id = atoi(value);
                printf("%d \n",ar_config[count].capture_dev_id);
            }
            else if (strcmp(key, "domain") == 0){
                strcpy(ar_config[count].domain, value);
                printf("%s \n",ar_config[count].domain);
            }
            else if (strcmp(key, "username") == 0){
                strcpy(ar_config[count].username, value);
                printf("%s \n",ar_config[count].username);
            }
            else if (strcmp(key, "secret") == 0){
                strcpy(ar_config[count].secret, value);
                printf("%s \n",ar_config[count].secret);
            }
            else {}
        }
    }
}
