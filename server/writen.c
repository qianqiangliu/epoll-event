#include <unistd.h>
#include <errno.h>

/* write "n" bytes to a descriptor */
ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	const char *ptr = vptr;

	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nwritten = 0;			/* and call write() again */
			else if (errno == EAGAIN)	/* write all data */
				break;
			else
				return -1;
		} else if (nwritten == 0) {
			return -1;					/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return n;
}
