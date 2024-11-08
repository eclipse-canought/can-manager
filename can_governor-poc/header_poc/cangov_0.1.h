//Designing the can gov header file to be included in client

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define socket cgsocket
#define connect cgconnect
#define send cgsend
#define read cgread
#define close cgclose
//#define setsockopt cgsetsockopt
//#define bind cgbind

int cgsocket(int domain, int type, int protocol) {
    // Custom socket implementation here
    int result = socket(domain, type, protocol);
    // Additional custom logic if needed
    return result;
}

int cgconnect(int socket, const struct sockaddr *address,
    socklen_t address_len) {
    // Custom connect implementation here
    int result = connect(socket, address, address_len);
    // Additional custom logic if needed
    return result;
}

ssize_t cgsend(int socket, const void *buffer, size_t length, int flags) {
    // Custom send implementation here
    ssize_t result = send(socket, buffer, length, flags);
    // Additional custom logic if needed
    return result;
}

ssize_t cgread(int fd, void *buf, size_t count) {
    // Custom read implementation here
    ssize_t result = read(fd, buf, count);
    // Additional custom logic if needed
    return result;
}

int cgclose(int fd) {
    // Custom close implementation here
    int result = close(fd);
    // Additional custom logic if needed
    return result;
}

/*int cgsetsockopt(int socket, int level, int option_name,
       const void *option_value, socklen_t option_len) {
       //Custom setsockopt implementation here
       int result = setsockopt(socket, level, option_name, option_value, option_len);
       //Additional custom logic if needed
       return result;
}
*/

/*int cgbind(int socket, struct sockaddr *address, int address_len) {
	//Custom bind implementation here
	int result = bind(socket, address, address_len);
	// Additional custom logic if needed
	return result;
}
*/

// Undefine custom macros after defining the custom functions
#undef socket
#undef connect
#undef send
#undef read
#undef close

