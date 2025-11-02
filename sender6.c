#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[]) {
	char c = 'A';
	struct sockaddr_in6 addr = {
		.sin6_family = AF_INET6,
#if defined(__APPLE__) || defined(__FreeBSD__)
		.sin6_len = sizeof(struct sockaddr_in6),
#endif
		.sin6_port = htons(1234)
	};
	struct iovec iov = {
		.iov_base = &c,
		.iov_len = 1
	};
	struct msghdr msg = {
		.msg_name = &addr,
		.msg_namelen = sizeof(struct sockaddr_in6),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	char cmsgbuf[CMSG_SPACE(sizeof(int))];
	struct cmsghdr *cmsg = (struct cmsghdr *)cmsgbuf;
	int fd;
	const int on = 1, off = 0;
	int tclass = 1;

	if (inet_pton(AF_INET6, argv[1], &addr.sin6_addr) != 1)
		printf("inet_pton() unsuccessful\n");

	if ((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		perror("socket");

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(int)) < 0)
		perror("IPV6_V6ONLY");

	/* Send the message with the default TCLASS. */
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Set the TCLASS to be used on the socket to 1 and send a message. */
#if defined(__linux__)
	if (IN6_IS_ADDR_V4MAPPED(&addr.sin6_addr)) {
		if (setsockopt(fd, IPPROTO_IP, IP_TOS, &tclass, sizeof(int)) < 0)
			perror("IP_TOS");
	} else
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_TCLASS, &tclass, sizeof(int)) < 0)
			perror("IPV6_TCLASS");
	}
#else
	if (setsockopt(fd, IPPROTO_IPV6, IPV6_TCLASS, &tclass, sizeof(int)) < 0)
		perror("IPV6_TCLASS");
#endif
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Send another message, this time with the TOS of 2. */
#if defined(__linux__)
	if (IN6_IS_ADDR_V4MAPPED(&addr.sin6_addr)) {
		cmsg->cmsg_level = IPPROTO_IP;
		cmsg->cmsg_type = IP_TOS;
	} else {
		cmsg->cmsg_level = IPPROTO_IPV6;
		cmsg->cmsg_type = IPV6_TCLASS;
	}
#else
	cmsg->cmsg_level = IPPROTO_IPV6;
	cmsg->cmsg_type = IPV6_TCLASS;
#endif
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*(int *)CMSG_DATA(cmsg) = 2;
	msg.msg_control = cmsgbuf;
	msg.msg_controllen = CMSG_SPACE(sizeof(int));
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Send a final message with the socket default of 1. */
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	if (close(fd) < 0)
		perror("close");

	return (0);
}
