#include "multisockets.h"

#ifdef __linux__
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


#include <stdio.h>
#include <stdlib.h>
#endif

int get_socket_domain(enum socket_domain domain) {
	switch(domain) {
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
#ifdef _WIN32
	WSADATA wsaData;
	int rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (rc != 0) { return -1; }
#endif
	return 0;
}

int sock_open(socket_t *sock, enum socket_domain domain, enum socket_type type) {
	sock->protocol = 0;
	if((sock->domain = get_socket_domain(domain)) == -1)
		{ return -1; }
	if((sock->type = get_socket_type(type)) == -1)
		{ return -2; }
#ifdef __linux__
	sock->fd = socket(sock->domain, sock->type, sock->protocol);
	if(sock->fd == -1) { return -3; }
#elif defined _WIN32
	sock->fd = socket(sock->domain, sock->type, sock->protocol);
	if(sock->fd == INVALID_SOCKET) { return -3; }
#endif
	return 0;
}
/*
int get_ipv4_address(socket_address_t* sa, const char* ip, const uint16_t port) {
	struct sockaddr_in* sasa = ((struct sockaddr_in*)(&(sa->sa)));
	sasa->sin_family = AF_INET;
	sa->sa_len = sizeof(struct sockaddr_in);
	sasa->sin_port = htons(port);

	int rc = inet_pton(AF_INET, ip, &sasa->sin_addr);

	if (rc == 1) { return 0; }
	else if(rc == 0) { return -1; }
	else { return -2; }
}

int get_ipv6_address(socket_address_t* sa, const char* ip, const uint16_t port) {
#ifdef __linux__
	int ip6_size; // size of actual ip (strlen(ip) without possible %iface suffixes)

	struct sockaddr_in6* sasa = ((struct sockaddr_in6*)(&(sa->sa)));
	sasa->sin6_family = AF_INET6;
	sa->sa_len = sizeof(struct sockaddr_in6);

	const char* interface_pointer = memchr(ip, '%', strlen(ip));
	if(interface_pointer == NULL) { // no %iface suffix
		sasa->sin6_flowinfo = 0;
		ip6_size = strlen(ip);
	}
	else { // %iface suffix found
		ip6_size = interface_pointer-ip;
		sasa->sin6_flowinfo = if_nametoindex(interface_pointer+1);
	}

	// copy only the IP
	char* ip6 = malloc(ip6_size+1);
	if(ip6 == NULL) { return -3; }
	strncpy(ip6, ip, ip6_size);
	ip6[ip6_size] = '\0';

	// set port and ip
	sasa->sin6_port = htons(port);
	int rc = inet_pton(AF_INET6, ip6, &sasa->sin6_addr);
	free(ip6);
	if(rc == 1) { return 0; }
	else if(rc == 0) { return -1; }
	else { return -2; }
#elif defined _WIN32
#pragma message ("get_ipv6_address: NOT IMPLEMENTED!")
	return -2;
#endif
}
*/
int sock_bind(socket_t *sock, socket_address_t* sa) {
#ifdef __linux__
	int opt = 1;
#elif defined _WIN32
	const char opt = 1;
#endif

	if(setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{ return -1; }
	if(bind(sock->fd, &sa->sa, sa->sa_len) == -1)
		{ return -2; }
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