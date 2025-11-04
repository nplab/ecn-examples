#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
#define IPTOS_DSCP_AF41 0x88
#define IPTOS_DSCP_AF42 0x90
#endif

int
main(int argc, char *argv[]) {
	char c = 'A';
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
#if defined(__APPLE__) || defined(__FreeBSD__)
		.sin_len = sizeof(struct sockaddr_in),
#endif
		.sin_port = htons(1234)
	};
	struct iovec iov = {
		.iov_base = &c,
		.iov_len = 1
	};
	struct msghdr msg = {
		.msg_name = &addr,
		.msg_namelen = sizeof(struct sockaddr_in),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
#if defined(__FreeBSD__)
	char cmsgbuf[CMSG_SPACE(sizeof(unsigned char))];
#else
	char cmsgbuf[CMSG_SPACE(sizeof(int))];
#endif
	struct cmsghdr *cmsg = (struct cmsghdr *)cmsgbuf;
	int fd;
	const int on = 1;
	int tos = IPTOS_DSCP_AF41 | IPTOS_ECN_ECT1;

	if (inet_pton(AF_INET, argv[1], &addr.sin_addr) != 1)
		printf("inet_pton() unsuccessful\n");

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		perror("socket");

	/* Send the message with the system wide default TOS. */
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Set the default TOS to be used on the socket to AF41 with ECT1. */
	if (setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(int)) < 0)
		perror("IP_TOS");

	/* Send a message with the default socket TOS (AF41 with ECT1). */
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Send another message, this time with the TOS of AF42 and CE. */
	cmsg->cmsg_level = IPPROTO_IP;
	cmsg->cmsg_type = IP_TOS;
#if defined(__FreeBSD__)
	cmsg->cmsg_len = CMSG_LEN(sizeof(unsigned char));
	*(unsigned char *)CMSG_DATA(cmsg) = IPTOS_DSCP_AF42 | IPTOS_ECN_CE;
#else
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*(int *)CMSG_DATA(cmsg) = IPTOS_DSCP_AF42 | IPTOS_ECN_CE;
#endif
	msg.msg_control = cmsgbuf;
#if defined(__FreeBSD__)
	msg.msg_controllen = CMSG_SPACE(sizeof(unsigned char));
#else
	msg.msg_controllen = CMSG_SPACE(sizeof(int));
#endif
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	/* Send a final message with the default socket TOS (AF41 with ECT1). */
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	if (sendmsg(fd, &msg, 0) < 0)
		perror("sendmsg");

	if (close(fd) < 0)
		perror("close");

	return (0);
}
