#include "multisockets.h"

#ifdef __linux__
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


#include <stdio.h>
#include <stdlib.h>
#endif

int get_socket_domain(enum socket_domain domain) {
	switch(domain) {
	case UNSPEC:
		return AF_UNSPEC;
	case IPV4:
		return AF_INET;
	case IPV6:
		return AF_INET6;
	default:
		return -1;
	};
}

int get_socket_type(enum socket_type type) {
	switch(type) {
	case TCP:
		return SOCK_STREAM;
	case UDP:
		return SOCK_DGRAM;
	default:
		return -1;
	};
}


int sock_init() {
	int rc = 0;
#ifdef __linux__
	struct sigaction ignore_broken_pipe = {
		.sa_handler = SIG_IGN,
		.sa_flags = 0
	};
	sigemptyset(&ignore_broken_pipe.sa_mask);
	rc = sigaction(SIGPIPE, &ignore_broken_pipe, NULL);
#elif defined(_WIN32)
	WSADATA wsaData;
	rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	if(rc != 0) { return -1; }
	return 0;
}

int sock_open_and_bind(socket_t *sock, const char* host, uint16_t port) {
#ifdef __linux__
	int opt = 1;

	size_t strport_len = snprintf(NULL, 0, "%d", port)+1;
	char* strport = malloc(strport_len);
	if(strport == NULL) { return -1; }
	snprintf(strport, strport_len, "%d", port);
	strport[strport_len] = '\0';

	struct addrinfo *res, *rp, hints = {
		.ai_family = sock->domain,
		.ai_socktype = sock->type,
		.ai_protocol = sock->protocol,
		.ai_canonname = NULL,
		.ai_addr = NULL,
		.ai_next = NULL
	};

	if(getaddrinfo(host, strport, &hints, &res) == 0) {
		for(rp = res; rp != NULL; rp = rp->ai_next) {
			sock->fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if(sock->fd == -1) { continue; }

			if(setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
				close(sock->fd);
				sock->fd = -1;
				continue;
			}

			if(bind(sock->fd, res->ai_addr, res->ai_addrlen) == -1) {
				close(sock->fd);
				sock->fd = -1;
				continue;
			}

			sock->domain = res->ai_family;
			sock->type = res->ai_socktype;
			sock->protocol = res->ai_protocol;

			break;
		}
		freeaddrinfo(res);
	}
	free(strport);
#endif

	if(sock->fd == -1) { return -1; }
	return 0;
}

int sock_listen(socket_t *sock, int backlog) {
#if defined(__linux__) || defined(_WIN32)
	if(listen(sock->fd, backlog) == 0) { return 0; }
	else { return -1; }
#endif
}

int sock_accept(socket_t *sock, socket_t *remote_sock, socket_address_t *remote_sa) {
	remote_sock->domain = sock->domain;
	remote_sock->type = sock->type;
	remote_sock->protocol = sock->protocol;

#if defined(__linux__) || defined(_WIN32)
	remote_sock->fd = accept(sock->fd, &remote_sa->sa, &remote_sa->sa_len);
	if(remote_sock->fd == -1) { return -1; }
#endif
	return 0;
}

int is_socket(socket_t* sock) {
#ifdef __linux__
	if (sock->fd > -1) { return 0; }
	else { return -1; }
#elif defined _WIN32
	if (sock->fd == INVALID_SOCKET) { return -1; }
	else { return 0; }
#endif
}

void sock_close(socket_t *sock) {
#ifdef __linux__
	shutdown(sock->fd, SHUT_RDWR);
	close(sock->fd);
#elif defined _WIN32
	shutdown(sock->fd, SD_BOTH);
	closesocket(sock->fd);
#endif
}


TRANSMIT_SIZE sock_send(socket_t *sock, const void *buf, size_t len) {
#if defined(__linux__) || defined(_WIN32)
	return send(sock->fd, buf, len, 0);
#endif
}

TRANSMIT_SIZE sock_recv(socket_t *sock, void *buf, size_t len) {
#if defined(__linux__) || defined(_WIN32)
	return recv(sock->fd, buf, len, 0);
#endif
}
