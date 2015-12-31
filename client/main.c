/* 
 * client.c 
 * overred
 * notice: file's tail (return 0) will cause client to exit
 * the server will be getting a SIGPIPE signal while write buffer has unsent stuff
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "common.h"

#define MAXLINE		1024

void connect_flood(int loop)
{
	char *host = "127.0.0.1";
	int port = 1991;
	struct sockaddr_in servaddr;
	int sockfd;

	struct timeval start, end;
	gettimeofday(&start, NULL);

	int i;
	for (i = 0; i < loop; i++) {
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr(host);
		servaddr.sin_port = htons(port);

		Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		close(sockfd);
	}

	gettimeofday(&end, NULL);

	int time_cost = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	printf("connect: %d, cost %lf sec\n", loop, time_cost / 1000000.0);
}

void request_flood(int loop)
{
	char *host = "127.0.0.1";
	int port = 1991;
	struct sockaddr_in servaddr;
	char *split = "\r\n";
	char *cmd = "GET";

	char buf[MAXLINE] = "asdfalllllllllllllkasdfsahid";
	char msg[MAXLINE];

	strcpy(msg, cmd);

	strcat(msg, split);
	char c[32] = { 0 };

	sprintf(c, "%d", strlen(buf));

	strcat(msg, c);
	strcat(msg, split);
	strcat(msg, buf);
	msg[strlen(msg)] = '\0';

	int sockfd;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(host);
	servaddr.sin_port = htons(port);

	Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	int i;
	struct timeval start, end;
	gettimeofday(&start, NULL);

	for (i = 0; i < loop; i++) {
		if (write(sockfd, msg, sizeof(msg)) == -1)
			perror("write");
	}

	gettimeofday(&end, NULL);

	int time_cost = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	printf("connect: %d, cost %lf sec\n", loop, time_cost / 1000000.0);
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("please put loop num\n");
		exit(0);
	}

	request_flood(atoi(argv[1]));
	connect_flood(atoi(argv[1]));

	return 0;
}
