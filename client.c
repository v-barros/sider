/*
 * client.c
 *
 *  Created on: 2021-11-21
 *      Author: @v-barros
 */
#include "client.h"
#include "commands.h"

int read_command(char *buff);

void func(int sockfd)
{
    char buff[MAX];
    char aux[MAX];
    int len;
    for (;;) {
        bzero(buff, sizeof(buff));
        bzero(aux,sizeof(aux));
        printf("\n>");
        len = read_command(buff);
        len = is_valid_get(buff,len);
        if(len!=-1){
            encode_get(buff,len,aux);
            printf("sending:\"%s\"%d\n",aux,len);
            write(sockfd, buff, len+4);
        }
        else{
            printf("invalid command\n"); 
            continue;
        }
            
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("\"%s\"", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}
//read stdin, put the string at str and return string length
//get abc\n
//       8   
int read_command(char *buff){
    int n =0;
    while ((buff[n++] = getchar()) != '\n');
    buff[n-1]=buff[n];
    return n-1;
}