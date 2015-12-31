#include <unistd.h>
#include <errno.h>

/* read "n" bytes to a descriptor */
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft = n;
	ssize_t nread;
	char *ptr = vptr;

	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;				/* and call read() again */
			else if (errno == EAGAIN)	/* read all data */
				break;
			else
				return -1;
		} else if (nread == 0) {
			break;						/* EOF */
		}

		nleft -= nread;
		ptr   += nread;
	}

	return n - nleft;					/* return >= 0 */
}
