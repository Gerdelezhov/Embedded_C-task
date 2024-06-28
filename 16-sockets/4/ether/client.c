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
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <net/ethernet.h> // For ETH_P_ALL

#define BUF_SIZE 47
#define MAIN_PORT 7780
#define PORT 7757

struct udp_header {
    short src_port;
    short des_port;
    short length;
    short check_sum;
    char payload[BUF_SIZE-42];
};

struct ip_header {
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

struct ethernet_header { 
    char des_mac[6];
    char src_mac[6];
    short type;
    struct ip_header payload;
};

int main() {
    int fd;   
    struct ethernet_header msg_client, *msg_server;
    char buf[BUF_SIZE]; 
    struct sockaddr_ll server;
    socklen_t size = sizeof(struct sockaddr_ll); 

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    server.sll_family = AF_PACKET;
    server.sll_protocol = htons(ETH_P_ALL);
    server.sll_ifindex = if_nametoindex("enp2s0");
    if (server.sll_ifindex == 0) {
        perror("if_nametoindex");
        close(fd);
        return -1;
    }
    server.sll_hatype = 0;
    server.sll_pkttype = 0;
    server.sll_halen = 6;
    server.sll_addr[0] = 0x08;
    server.sll_addr[1] = 0x8f;
    server.sll_addr[2] = 0xc3;
    server.sll_addr[3] = 0x37;
    server.sll_addr[4] = 0xf7;
    server.sll_addr[5] = 0x77;
  
    msg_client.des_mac[0] = 0x08;
    msg_client.des_mac[1] = 0x8f;
    msg_client.des_mac[2] = 0xc3;
    msg_client.des_mac[3] = 0x37;
    msg_client.des_mac[4] = 0xf7;
    msg_client.des_mac[5] = 0x77;

    msg_client.src_mac[0] = 0x08;
    msg_client.src_mac[1] = 0x8f;
    msg_client.src_mac[2] = 0xc3;
    msg_client.src_mac[3] = 0x37;
    msg_client.src_mac[4] = 0xf7;
    msg_client.src_mac[5] = 0x77;

    msg_client.type = htons(0x0800);

    msg_client.payload.Ver_IHL = 69; // 69 = 0100 0101 
    msg_client.payload.DS = 0;
    msg_client.payload.length = htons(BUF_SIZE-14);
    msg_client.payload.identification = 0;
    msg_client.payload.flags_offset = 0;
    msg_client.payload.TTL = 64;
    msg_client.payload.Transport_Proto = 17;
    msg_client.payload.Checksum = 0;
    msg_client.payload.src_ip = inet_addr("127.0.0.1");
    msg_client.payload.dest_ip = inet_addr("127.0.0.1"); 

    msg_client.payload.payload.src_port = htons(PORT);
    msg_client.payload.payload.des_port = htons(MAIN_PORT);
    msg_client.payload.payload.length = htons(BUF_SIZE-34);
    msg_client.payload.payload.check_sum = 0;
    strcpy(msg_client.payload.payload.payload, "Hi"); 
 
    if (sendto(fd, &msg_client, BUF_SIZE, 0, (struct sockaddr *) &server, size) == -1) {
        printf("ERR send %d\n", errno);
        close(fd);
        return -1;
    }

    while (1) {
        if (recvfrom(fd, buf, BUF_SIZE, 0, (struct sockaddr *) &server, &size) == -1) {
            printf("RECV %s\n", buf); 
        } else {
            msg_server = (struct ethernet_header *)buf;
        }
        printf("Пакет получен");
        if (msg_server->payload.payload.des_port == msg_client.payload.payload.src_port) {
            printf(", server - %s\n", msg_server->payload.payload.payload); 
            break;
        } else {
            printf("не тот сервер\n");
        }
    }

    close(fd);
    return 0;
}
