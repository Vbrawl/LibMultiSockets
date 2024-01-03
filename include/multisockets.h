#ifndef MULTISOCKETS_H
#define MULTISOCKETS_H

#include <stdint.h>
//#include <stddef.h>

#ifdef __linux__
#include <sys/socket.h>

struct socket_struct {
	int fd;
	int domain;
	int type;
	int protocol;
};

struct socket_address_struct {
	struct sockaddr sa;
	socklen_t sa_len;
};
#else
#error "This OS is not supported"
#endif


typedef struct socket_struct socket_t;
typedef struct socket_address_struct socket_address_t;

enum socket_domain { IPV4, IPV6 };
enum socket_type { TCP, UDP };

int get_socket_domain(enum socket_domain domain);
int get_socket_type(enum socket_type type);

/*
Description:
	Open a socket file descriptor under the specified protocols.

Parameters:
	sock: The socket object.
	domain: The lower-level protocol to use. (eg. IPv4)
	type: The higher-level protocol to use. (eg. TCP)

Returns:
	On success: 0
	On invalid domain: -1
	On invalid type: -2
	On error when opening the socket: -3 and errno is set
*/
int sock_open(socket_t* sock, enum socket_domain domain, enum socket_type type);

/*
Description:
	Check if a socket descriptor is valid.

Parameters:
	sock: The socket object to check for validity.

Returns:
	When sock is a valid socket: 0
	When sock is NOT a valid socket: -1
*/
int is_socket(socket_t* sock);

/*
Description:
	Bind a socket to the specified address.

Parameters:
	sock: The socket object.
	sa: A socket_address_t object to fill with the address needed.

Returns:
	On success: 0
	On error when marking address as reusable: -1 and errno is set
	On error when binding address to socket: -2 and errno is set
*/
int sock_bind(socket_t* sock, socket_address_t* sa);

/*
Description:
	Listen for connections on a socket.

Parameters:
	sock: The socket object.
	backlog: The maximum length of the connection queue.

Returns:
	On success: 0
	On error: -1 and errno is set
*/
int sock_listen(socket_t* sock, int backlog);

/*
Description:
	Accept a connection to a socket.

Parameters:
	sock: The socket object.
	remote_sock: A socket object to store the incoming client.
	remote_sa: An address object to store the incoming client's address.

Returns:
	On success: 0
	On error: -1 and errno is set
*/
int sock_accept(socket_t* sock, socket_t* remote_sock, socket_address_t* remote_sa);

/*
Description:
	Shutdown and close a socket.

Parameters:
	sock: The socket object.
*/
void sock_close(socket_t* sock);

/*
Description:
	Fill an address object with the specified IP and PORT.

Parameters:
	sa: Address object.
	ip: An IP address.
	port: A PORT number.

Returns:
	On success: 0
	On invalid address: -1
	On unsupported protocol: -2
	On memory errors: -3 and errno is set (see NOTES)

NOTES:
	1) Some converters use the heap memory temporarily for processing,
	such converters may return -3 if an error with memory allocation happens.
*/
int get_ipv4_address(socket_address_t* sa, const char* ip, const uint16_t port);
int get_ipv6_address(socket_address_t* sa, const char* ip, const uint16_t port); // heap


/*
Description:
	Send data through a socket.

Parameters:
	sock: The socket object.
	buf: The buffer with the data.
	len: The length of the buffer.

Returns:
	On success: The number of bytes sent
	On error: -1 and errno is set
*/
ssize_t sock_send(socket_t* sock, const void *buf, size_t len);

/*
Description:
	Receive data through a socket.

Parameters:
	sock: The socket object.
	buf: The buffer to store the received data.
	len: The length of the buffer.

Returns:
	On success: The number of bytes received
	On error: -1 and errno is set

NOTES:
	Return value may be 0 when:
		1) Socket is disconnected.
		2) A zero-length datagram (in a domain that permits it) is received.
		3) The buffer length PARAMETER is 0.
*/
ssize_t sock_recv(socket_t* sock, void *buf, size_t len);



#endif
