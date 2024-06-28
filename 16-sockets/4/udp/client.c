#include <stdio.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h> 
#include <errno.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h> 

#define BUF_SIZE 47
#define MAIN_PORT 7780
#define PORT 7757


struct udp_header{
    short src_port;
    short des_port;
    short length;
    short check_sum;
    char payload[BUF_SIZE-8];
};

int main(){
    int fd;  
    struct udp_header msg1, *msg2; 
    char buf[BUF_SIZE+20];
    struct sockaddr_in server;
    socklen_t size = sizeof(struct sockaddr_in);

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); 

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(MAIN_PORT);

    msg1.src_port = htons(PORT);
    msg1.des_port = htons(MAIN_PORT);
    msg1.length = htons(BUF_SIZE);
    msg1.check_sum = 0;
    strcat(msg1.payload, "Hi"); 
 
    if(sendto(fd, &msg1, BUF_SIZE, 0, (struct sockaddr *) &server, size) == -1){
        printf("error send %d\n", errno);
        close(fd);
        return -1;
    }

    while(1){
        if(recvfrom(fd, buf, BUF_SIZE+20, 0, (struct sockaddr *) &server, &size) == -1){
            printf("RECV %s\n", buf); 
        }
        else{
            msg2 = &buf[20];
        }
        printf("Пакет получен"); 
        if(msg2->des_port == msg1.src_port){
            printf(", server - %s\n", msg2->payload); 
            break;
        }
        else{
            printf("не тот сервер\n");
        }
    }

    close(fd);
    return 0;
}