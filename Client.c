#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define BUFFER 100000
#define PORT 80

extern int h_errno;

int main(int argc, char **argv){
    if(argc < 3){
        printf("\nUsage: ./a.out 'IP server' 'file name'\n");
        exit(1);
    }
    
    struct sockaddr_in sock_server;
    
    int sd_client=socket(AF_INET,SOCK_STREAM,0);
    int nchar;
    char buffer[BUFFER];
    
    if(sd_client < 0){
        perror("Socket");
        exit(1);
    }
    
    printf("\nIP server: %s\n",argv[1]);
    //printf("%8.8x\n",*((int *)(gethostbyname(argv[1])->h_addr_list[0])));
    
    unsigned int ipaddr = *((int *)(gethostbyname(argv[1])->h_addr_list[0]));
    unsigned int lsb0 = (ipaddr & 0x000000FF);
    unsigned int lsb1 = (ipaddr & 0x0000FF00)>>8;
    unsigned int lsb2 = (ipaddr & 0x00FF0000)>>16;
    unsigned int lsb3 = (ipaddr & 0xFF000000)>>24;
    
    printf("IP: %u.%u.%u.%u\n",lsb0,lsb1,lsb2,lsb3);
    
    sock_server.sin_family = AF_INET;
    sock_server.sin_port = PORT;
    sock_server.sin_addr.s_addr = ipaddr;
    
    if(connect(sd_client,(struct sockaddr *)&sock_server,sizeof(sock_server)) < 0 ){
        perror("Connect");
        exit(1);
    }
    
    if ((nchar = send(sd_client,argv[2],strlen(argv[2])+1,0)) < 0){
        perror("Send");
        exit(1);
    }
    
    recv(sd_client,buffer,sizeof(buffer),0);
    printf("\n%s\n",buffer);
    if(strcmp(&buffer[strlen(buffer)-4],"open")){
        printf("\nbo\n");
        exit(1);
    }else{
        int nbyte;
        char buf[BUFFER];
        int nb;
        while(1){
            recv(sd_client,&nbyte,4,0);
            if(nbyte){
                nb = recv(sd_client,buf,sizeof(buf),0);
                write(1,buf,nb);
            }else{
                break;
            }
        }
    }
    
    return 0;
}
