#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include "socket.h"

void socket_set_non_blocking(int sockfd)
{
	int flags;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		abort();
	}

	flags |= O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, flags) == -1) {
		perror("fcntl");
		abort();
	}
}

static void socket_reuse_endpoint(int sockfd)
{
	int reuse = 1;
	if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) < 0) {
		/*
		 * if we cannot set an option then there's something very wrong
		 * with the system, so we abort the application
		 */
		perror("setsockopt");
		abort();
	}
}


int socket_create_and_bind(char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int ret, sockfd;

	/* 
	 * Don't let the system abort the application when it tries to send bytes
	 * through a connection already closed by the client
	 */
	struct sigaction sa;

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;		/* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM;	/* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;		/* All interfaces */

	ret = getaddrinfo(NULL, port, &hints, &result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		abort();
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1)
			continue;
		socket_reuse_endpoint(sockfd);
		ret = bind(sockfd, rp->ai_addr, rp->ai_addrlen);
		if (ret == 0) {
			/* We managed to bind successfully */
			break;
		}

		if (close(sockfd) == -1) {
			perror("close");
			abort();
		}
	}

	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		abort();
	}

	freeaddrinfo(result);

	return sockfd;
}

void socket_start_listening(int sockfd)
{
	if (listen(sockfd, SOMAXCONN) == -1) {
		perror("listen");
		abort();
	}
}

int socket_accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen)
{
	int connfd;

	if ((connfd = accept(sockfd, cliaddr, addrlen)) == -1) {
		perror("accept");
		abort();
	}
	return connfd;
}
