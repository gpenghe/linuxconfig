#include <stdlib.h>             /* for exit() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>             /* memset() */
#include <sys/time.h>           /* select() */
#include <assert.h>
#include <sys/time.h>

/* BEGIN jcs 3/30/05 */

#define SOCKET_ERROR -1

int main(int argc, char *argv[])
{
    int sd, rc, flags;
    struct sockaddr_in cliAddr, remoteServAddr;
    struct hostent *h;
    int sleep_us;
    int adjusted_sleep_us;
    char txbuf[80];
    unsigned int seq, cmd;
    int packets = 0;
    int forever = 0;
    struct timeval start, end;

    /* check command line args */
    if (argc != 5)
    {
        printf("usage : %s <server> <port> <interval(us) <number>> \n", argv[0]);
        exit(1);
    }

    packets = atol(argv[4]);
    if(packets ==0 ) {
        forever = 1;
    }

    /* get server IP address (no check if input is IP address or DNS name */
    h = gethostbyname(argv[1]);
    if (h == NULL)
    {
        printf("%s: unknown host '%s' \n", argv[0], argv[1]);
        exit(1);
    }

    printf("%s: sending data to '%s' (IP : %s) \n", argv[0], h->h_name,
           inet_ntoa(*(struct in_addr *) h->h_addr_list[0]));

    remoteServAddr.sin_family = h->h_addrtype;
    memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    remoteServAddr.sin_port = htons(atol(argv[2]));

    /* socket creation */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("%s: cannot open socket \n", argv[0]);
        exit(1);
    }

    /* bind any port */
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliAddr.sin_port = htons(0);

    rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
    if (rc < 0)
    {
        printf("%s: cannot bind port\n", argv[0]);
        exit(1);
    }

    flags = 0;

    sleep_us = atol(argv[3]);
#define SEND_TIME_US 206
    adjusted_sleep_us = sleep_us - SEND_TIME_US;
    assert(adjusted_sleep_us >0);

    seq = 0;
    
    /* send data */
    gettimeofday(&start, NULL);
    while(forever || packets--){
        if (seq == 0) {
            cmd = 0xFFFFFFFF;
        } else {
            cmd = 0;
        }
        *(unsigned int*) (&txbuf[0])    = seq;
        *(unsigned int*) (&txbuf[4])    = cmd;
        rc = sendto(sd, txbuf, 8, flags,
                    (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));

        if (rc < 0)
        {
            printf("%s: cannot send data\n", argv[0]);
            close(sd);
            exit(1);
        }

        seq ++;
#define SEND_TIME 200
        usleep(adjusted_sleep_us);
    }
    gettimeofday(&end, NULL);
    printf("Total sending time (ms): %.3f\n", ((end.tv_sec-start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec))/1000.0);

    return 0;

}

