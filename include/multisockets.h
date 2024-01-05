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

typedef ssize_t TRANSMIT_SIZE;
#define DLL_EXPORT
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
struct socket_struct {
	SOCKET fd;
	int domain;
	int type;
	int protocol;
};

struct socket_address_struct {
	struct sockaddr sa;
	int sa_len;
};

typedef long long TRANSMIT_SIZE;
#define DLL_EXPORT __declspec(dllexport)
//#error "This OS is not supported"
#endif


typedef struct socket_struct socket_t;
typedef struct socket_address_struct socket_address_t;

enum socket_domain { UNSPEC, IPV4, IPV6 };
enum socket_type { TCP, UDP };

/*
Description:
	Translate values from "socket_domain" to socket API values

Parameters:
	domain: The value to translate.

Returns:
	On success: The translated domain
	When domain doesn't exist: -1
*/
DLL_EXPORT int get_socket_domain(enum socket_domain domain);

/*
Description:
	Translate values from "socket_type" to socket API values

Parameters:
	type: The value to translate.

Returns:
	On success: The translated type
	When type doesn't exist: -1
*/
DLL_EXPORT int get_socket_type(enum socket_type type);


/*
Description:
	Some OSes require initialization before allowing the usage of sockets.
	This function performs the required actions to initialize those sockets.

Returns:
	On success: 0
	On failure: -1
*/
DLL_EXPORT int sock_init();

/*
Description:
	Check if a socket descriptor is valid.

Parameters:
	sock: The socket object to check for validity.

Returns:
	When sock is a valid socket: 0
	When sock is NOT a valid socket: -1
*/
DLL_EXPORT int is_socket(socket_t* sock);

/*
Description:
	Open and Bind a socket to the specified address.

Parameters:
	sock: The socket object.
	host: A hostname or ip address.
	port: A port number.

Returns:
	On success: 0
	On failure: -1
*/
DLL_EXPORT int sock_open_and_bind(socket_t *sock, const char *host, uint16_t port);
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
DLL_EXPORT int sock_listen(socket_t* sock, int backlog);

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
DLL_EXPORT int sock_accept(socket_t* sock, socket_t* remote_sock, socket_address_t* remote_sa);

/*
Description:
	Shutdown and close a socket.

Parameters:
	sock: The socket object.
*/
DLL_EXPORT void sock_close(socket_t* sock);


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
DLL_EXPORT TRANSMIT_SIZE sock_send(socket_t* sock, const void *buf, size_t len);

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
DLL_EXPORT TRANSMIT_SIZE sock_recv(socket_t* sock, void *buf, size_t len);



#endif
