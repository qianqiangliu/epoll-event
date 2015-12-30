static int Socket(int family, int type, int protocol)
{
	int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket:");
		exit(1);
	}
	return sockfd;
}

static int Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
	if ((connect(sockfd, servaddr, addrlen)) == -1) {
		perror("server is not open or connect:");
		close(sockfd);
		exit(1);
	}
	return 0;
}

static int Bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen)
{
	if ((bind(sockfd, myaddr, addrlen)) == -1) {
		perror("binding:");
		exit(1);
	}
	return 0;
}

static int Listen(int sockfd, int backlog)
{
	if ((listen(sockfd, backlog)) == -1) {
		perror("listen:");
		exit(1);
	}
	return 0;
}

static int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t addrlen)
{
	int n;

	if ((n = accept(sockfd, cliaddr, &addrlen)) == -1) {
		perror("accept:");
		exit(1);
	}
	return n;
}
