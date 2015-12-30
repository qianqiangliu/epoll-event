#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>

void socket_set_non_blocking(int sockfd);
int socket_create_and_bind(char *port);
void socket_start_listening(int sockfd);
int socket_accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);

#endif	/* _SOCKET_H_ */
