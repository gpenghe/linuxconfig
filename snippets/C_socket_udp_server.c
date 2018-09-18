/* Converted to echo client/server with select() (timeout option). See udpClient.c */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>             /* close() */
#include <string.h>             /* memset() */

int main(int argc, char *argv[])
{
    int sd, rc, n, flags;
    unsigned int cliLen;
    struct sockaddr_in cliAddr, servAddr;
    char msg[80];
    int listen_port = -1;
    int verbose = 0;

    if (argc < 2 ) {
        printf("./udpServer <port> [any for verbose]\n");
        exit(0);
    }
    listen_port = atol(argv[1]);
    if(argc >2) {
        verbose = 1;
    }

    /* socket creation */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("%s: cannot open socket \n", argv[0]);
        exit(1);
    }

    /* bind local server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(listen_port);
    rc = bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (rc < 0)
    {
        printf("%s: cannot bind port number %d \n", argv[0], listen_port);
        exit(1);
    }

    printf("%s: waiting for data on port UDP %u\n", argv[0], listen_port);

    flags = 0;

    unsigned int expected_next_seq = 0;
    while (1)
    {
        unsigned int seq;
        unsigned int cmd;
        memset(msg, 0x0, 80);

        cliLen = sizeof(cliAddr);
        n = recvfrom(sd, msg, 80, flags, (struct sockaddr *) &cliAddr, &cliLen);

        if (n < 0) {
            printf("%s: cannot receive data \n", argv[0]);
            continue;
        }

        seq = *(unsigned int*) msg;
        cmd = *(unsigned int*) &msg[4];
        if(cmd == 0xFFFFFFFF) {
            expected_next_seq = 0;
        } 
        if(verbose) {
            printf("RX packet with %d bytes: cmd: 0x%X, seq: %u\n", n, cmd, seq);
        }
        if (expected_next_seq != seq ) {
            printf("Error seq number: %u, expected: %u\n", seq, expected_next_seq);
            exit(0);
        }

        expected_next_seq ++;
    }
    /* end of server infinite loop */

    return 0;

}

