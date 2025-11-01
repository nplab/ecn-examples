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
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
#if defined(__APPLE__) || defined(__FreeBSD__)
		.sin_len = sizeof(struct sockaddr_in),
#endif
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(1234)
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
	const int on = 1;
	unsigned char tos;

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		perror("socket");

	if (bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) < 0)
		perror("bind");

	if (setsockopt(fd, IPPROTO_IP, IP_RECVTOS, &on, sizeof(int)) < 0)
		perror("setsockopt");

	for (;;) {
		if (recvmsg(fd, &msg, 0) < 0) {
			perror("recvmsg");
			continue;
		}
		for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg,cmsg)) {
			if ((cmsg->cmsg_level == IPPROTO_IP) &&
#if defined(__linux__)
			    (cmsg->cmsg_type == IP_TOS) &&
#else
			    (cmsg->cmsg_type == IP_RECVTOS) &&
#endif
			    (cmsg->cmsg_len == CMSG_LEN(sizeof(unsigned char)))) {
				tos = *(unsigned char *)CMSG_DATA(cmsg);
				printf("tos = %u\n", tos);
			}
		}
	}
	if (close(fd) < 0)
		perror("close");
	return (0);
}
