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
    char payload[BUF_SIZE-28];
};

struct ip_header{
    char Ver_IHL;     
    char DS;
    short length;
    short identification;
    short flags_offset;
    char TTL; 
    char Transport_Proto;
    short Checksum;
    int src_ip;
    int dest_ip; 
    struct udp_header payload; 
};

int main(){
    int fd;   
    struct ip_header ip_msg_client, *ip_msg_server;
    char buf[BUF_SIZE];
    struct sockaddr_in server, client;
    socklen_t size = sizeof(struct sockaddr_in);

    fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); 

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(MAIN_PORT);

    int flag = 1;
    int tmp = setsockopt(fd, IPPROTO_IP, IP_HDRINCL, (void *)&flag, (socklen_t) sizeof(flag));
    
    ip_msg_client.Ver_IHL = 69; // 69 = 0100 0101 
    ip_msg_client.DS = 0;
    ip_msg_client.length = htons(BUF_SIZE);
    ip_msg_client.identification = 0;
    ip_msg_client.flags_offset = 0;
    ip_msg_client.TTL = 64;
    ip_msg_client.Transport_Proto = 17;
    ip_msg_client.Checksum = 0;
    ip_msg_client.src_ip = inet_addr("127.0.0.1");
    ip_msg_client.dest_ip = inet_addr("127.0.0.1"); 

    ip_msg_client.payload.src_port = htons(PORT);
    ip_msg_client.payload.des_port = htons(MAIN_PORT);
    ip_msg_client.payload.length = htons(BUF_SIZE-20);
    ip_msg_client.payload.check_sum = 0;
    strcat(ip_msg_client.payload.payload, "Hi"); 
 
    if(sendto(fd, &ip_msg_client, BUF_SIZE, 0, (struct sockaddr *) &server, size) == -1){
        printf("error send %d\n", errno);
        close(fd);
        return -1;
    }

    while(1){
        if(recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &server, &size) == -1){
            printf("RECV %s\n", buf); 
        }
        else{
            ip_msg_server = buf;
        }
        printf("Пакет получен");
        if(ip_msg_server->payload.des_port == ip_msg_client.payload.src_port){
            printf(", server - %s\n", ip_msg_server->payload.payload); 
            break;
        }
        else{
            printf("не тот сервер\n");
        }
    }

    close(fd);
    return 0;
}
