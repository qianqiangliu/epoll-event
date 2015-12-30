#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>

#include "poll.h"
#include "debug.h"
#include "socket.h"

#define BUFFSIZE	1024

#define UNUSED_PARAMS(x) (void)x

static char read_buf[BUFFSIZE];
static int read_flag;
static int read_num;

void read_cb(poll_event_t * poll_event, poll_event_element_t * node,
			 struct epoll_event ev)
{
	UNUSED_PARAMS(poll_event);
	UNUSED_PARAMS(ev);
	/* NOTE: read is also invoked on accept and connect */
	INFO("in read_cb");

	/* we just read data and print */
	read_num = read(node->fd, read_buf, BUFFSIZE);

	if (read_num > 0) {
		read_flag = 1;
		/* if we actually get data, print it */
		read_buf[read_num] = '\0';	/* null terminal */
		fprintf(stdout, "received data: %s\n", read_buf);
	}
}

void write_cb(poll_event_t * poll_event, poll_event_element_t * node,
			 struct epoll_event ev)
{
	UNUSED_PARAMS(poll_event);
	UNUSED_PARAMS(ev);
	INFO("in write_cb");

	if (read_flag) {
		read_flag = 0;
		if (write(node->fd, read_buf, read_num) == -1)
			perror("write");
	}
}

void close_cb(poll_event_t * poll_event, poll_event_element_t * node,
			  struct epoll_event ev)
{
	UNUSED_PARAMS(ev);
	INFO("in close_cb");

	/* close the socket, we are done with it */
	poll_event_remove(poll_event, node->fd);
}

void accept_cb(poll_event_t * poll_event, poll_event_element_t * node,
			   struct epoll_event ev)
{
	UNUSED_PARAMS(ev);
	INFO("in accept_cb");

	/* accept the connection */
	struct sockaddr_in clt_addr;
	socklen_t clt_len = sizeof(clt_addr);

	int connfd = socket_accept(node->fd, (struct sockaddr *)&clt_addr, &clt_len);
	socket_set_non_blocking(connfd);
	fprintf(stdout, "got the socket %d\n", connfd);

	/* set flags to check */
	uint32_t flags = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP;
	poll_event_element_t *p;

	/* add file descriptor to poll event */
	poll_event_add(poll_event, connfd, flags, &p);
	/* set function callbacks */
	p->read_callback = read_cb;
	p->write_callback = write_cb;
	p->close_callback = close_cb;
}

int timeout_cb(poll_event_t * poll_event)
{
	/* just keep a count */
	if (!poll_event->data) {
		/* no count initialised, then initialize it */
		INFO("init timeout counter");
		poll_event->data = calloc(1, sizeof(int));
	} else {
		/* increment and print the count */
		int *value = (int *)poll_event->data;

		*value += 1;
		LOG("time out number %d", *value);
	}
	return 0;
}

int main()
{
	int sockfd;
	sockfd = socket_create_and_bind("1991");
	socket_set_non_blocking(sockfd);
	socket_start_listening(sockfd);

	/* create a poll event object, with block indefinitely */
	poll_event_t *pe = poll_event_new(-1);

	/* set timeout callback */
	/* pe->timeout_callback = timeout_cb; */
	poll_event_element_t *p;

	/* add sockfd to poll event */
	poll_event_add(pe, sockfd, EPOLLIN, &p);

	/* set callbacks */
	p->accept_callback = accept_cb;

	/* enable accept callback */
	p->cb_flags |= ACCEPT_CB;

	/* start the event loop */
	poll_event_loop(pe);

	return 0;
}
