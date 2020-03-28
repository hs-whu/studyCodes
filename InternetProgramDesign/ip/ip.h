//
// Created by hs on 2020/2/22.
//

#ifndef TEST_IP_H
#define TEST_IP_H

#define BUFSIZE 65536
#define ICMP_DATALEN 56
#define MAXTIME 2000

char sendbuf[BUFSIZE];
char recvbuf[BUFSIZE];

uint16_t icmp_sequence = 1;
int sockfd = -1;
uint8_t ttl = 0;

struct sockaddr_in dest;
struct sockaddr_in from;
struct sockaddr_in local;
struct sigaction act_alarm;
struct sigaction act_int;

int recv_flag = 1;
struct timeval lasttime;
void send_ping();
void recv_reply();
uint16_t checksum(uint8_t* buf, int len);
void set_sighandler();
void bail(const char*);
void alarm_handler(int);
void int_handler(int);
int handler_pkt();

#endif //TEST_IP_H
