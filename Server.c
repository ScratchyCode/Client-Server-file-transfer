// Coded by ScratchyCode
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define BUFFER 100000
#define NAME 1000
#define PORT 80

extern int h_errno;

int main(int argc, char **argv){
    struct sockaddr_in sock_server;
    struct sockaddr_in sock_client;
    int sock_leng = sizeof(struct sockaddr_in);
    int sd_client;
    int sd = socket(AF_INET,SOCK_STREAM,0);
    char name[NAME];
    
    if(sd < 0){
        perror("Socket");
        exit(1);
    }
    
    gethostname(name,sizeof(name));
    printf("\nHostname: %s\n",name);
    //printf("%8.8x\n",*((int *)(gethostbyname(name)->h_addr_list[0])));
    unsigned int ipaddr = *((int *)(gethostbyname(name)->h_addr_list[0]));
    
    // Operazini bitwise per trovare le cifre dell'indirizzo ip
    unsigned int lsb0 = (ipaddr & 0x000000FF);
    unsigned int lsb1 = (ipaddr & 0x0000FF00)>>8;
    unsigned int lsb2 = (ipaddr & 0x00FF0000)>>16;
    unsigned int lsb3 = (ipaddr & 0xFF000000)>>24;
    printf("IP server: %u.%u.%u.%u\nListening to port %d...\n",lsb0,lsb1,lsb2,lsb3,PORT);

/*
   0000 1010 1001 0101 0001 1001 1010 1100
   and
   0000 0000 0000 0000 0000 0000 1111 1111
   ==
   0000 0000 0000 0000 0000 0000 1010 1100
   
   lsb1 >> 8
*/

    sock_server.sin_family = AF_INET;
    sock_server.sin_port = PORT;
    sock_server.sin_addr.s_addr = ipaddr;
    
    if(bind(sd,(struct sockaddr *) &sock_server,sizeof(sock_server)) < 0 ){
        perror("Bind");
        exit(1);
    }
    
ASCOLTA:
    if(listen(sd,200) < 0){
        perror("Listen");
        exit(1);
    }
    
    if((sd_client = accept(sd,(struct sockaddr *)&sock_client,&sock_leng)) < 0){
        perror("Accept");
        exit(1);
    }
    
    ipaddr = sock_client.sin_addr.s_addr;
    
    //printf("\nIs calling: %8.8x\n",ipaddr);
    lsb0 = (ipaddr & 0x000000FF);
    lsb1 = (ipaddr & 0x0000FF00)>>8;
    lsb2 = (ipaddr & 0x00FF0000)>>16;
    lsb3 = (ipaddr & 0xFF000000)>>24;
    printf("\nCaller IP: %u.%u.%u.%u\n",lsb0,lsb1,lsb2,lsb3);
    
    if(fork()){
    /* parent */
        //printf("Parent: my pid is %d\n",getpid());
        close(sd_client);
        goto ASCOLTA;
    }else{
    /* child */
        char filename[NAME];
        int nchar;
        int fd;
        
        //printf("Child: my pid is %d ppid=%d\n",getpid(),getppid());
        close(sd);
        
        if((nchar = recv(sd_client,filename,sizeof(filename),0)) < 0){
            perror("Receive");
            exit(1);
        }
        
        printf("\nReceived %d chars.\nFile name: %s\n",nchar,filename);
        h_errno = 0;
        
        if((fd = open(filename,O_RDONLY)) < 0){
            filename[strlen(filename)] = 0x20;
            filename[strlen(filename)+1] = 0;
            perror("File name");
            printf("%s: %s\n",filename,strerror(errno));
            strcat(filename,strerror(errno));
            send(sd_client,filename,strlen(filename)+1,0);
            exit(1);
        }else{
            char buffer[BUFFER];
            int nbyte;
            
            strcat(filename,"open");
            send(sd_client,filename,strlen(filename)+1,0);
            while(1){
                nbyte = read(fd,buffer,sizeof(buffer));
                send(sd_client,&nbyte,4,0);
                if(nbyte){
                    send(sd_client,buffer,nbyte,0);
                }else{
                    break;
                }
            }
        }
    }
    
    return 0;
}

/*
struct in_addr {
    unsigned int s_addr;
};

struct sockaddr_in {
    int sin_family;
    int sin_port;struct 
    struct  in_addr sin_addr;
};
*/
