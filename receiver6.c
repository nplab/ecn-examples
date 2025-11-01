#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int
main(void) {
	char c;
	struct sockaddr_in6 addr = {
		.sin6_family = AF_INET6,
#if defined(__APPLE__) || defined(__FreeBSD__)
		.sin6_len = sizeof(struct sockaddr_in6),
#endif
		.sin6_addr = in6addr_any,
		.sin6_port = htons(1234)
	};
	char cmsgcmsg[CMSG_SPACE(sizeof(unsigned char))];
	struct iovec iov = {
		.iov_base = &c,
		.iov_len = sizeof(char)
	};
	struct msghdr msg = {
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = cmsgcmsg,
		.msg_controllen = CMSG_SPACE(sizeof(unsigned char))
	};
	struct cmsghdr *cmsg;
	int fd;
	const int on = 1, off = 0;
	int tclass;
#if defined(__linux__)
	unsigned char tos;
#endif

	if ((fd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		perror("socket");

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(int)) < 0)
		perror("IPV6_V6ONLY");

	if (bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in6)) < 0)
		perror("bind");

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_RECVTCLASS, &on, sizeof(int)) < 0)
		perror("IPV6_RECVTCLASS");
//#if defined(__linux__)
	if (setsockopt(fd, IPPROTO_IP, IP_RECVTOS, &on, sizeof(int)) < 0)
		perror("IP_RECVTOS");
//#endif

	for (;;) {
		if (recvmsg(fd, &msg, 0) < 0) {
			perror("recvmsg");
			continue;
		}
		for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg,cmsg)) {
#if defined(__linux__)
			if ((cmsg->cmsg_level == IPPROTO_IP) &&
			    (cmsg->cmsg_type == IP_TOS) &&
			    (cmsg->cmsg_len == CMSG_LEN(sizeof(unsigned char)))) {
				tos = *(unsigned char *)CMSG_DATA(cmsg);
				printf("tos = %u\n", tos);
			}
#endif
			if ((cmsg->cmsg_level == IPPROTO_IPV6) &&
			    (cmsg->cmsg_type == IPV6_TCLASS) &&
			    (cmsg->cmsg_len == CMSG_LEN(sizeof(int)))) {
				tclass = *(int *)CMSG_DATA(cmsg);
				printf("tclass = %d\n", tclass);
			}
		}
	}
	if (close(fd) < 0)
		perror("close");
	return (0);
}
