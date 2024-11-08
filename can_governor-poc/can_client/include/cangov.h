//Designing the can gov header file to be included in client

#include <sys/ioctl.h>
#include <net/if.h>
#define CAN_INTERFACE "vcan1"
#define ioctl cgioctl

int cgioctl(int sockfd, unsigned long request, struct ifreq *ifr) {
    int result = 0;
    strcpy(ifr->ifr_name, CAN_INTERFACE);
    result = ioctl(sockfd, request, ifr);  
    return result;
}

#undef ioctl