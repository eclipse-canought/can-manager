// Raw data bytes
#define CAN_INTERFACE "can0"
#define CAN_FRAME_LENGTH 8
#define CAN_ACM_DATA "\x00\x00\x00\x00\x00\x00\x00\x00"
#define CAN_FRAME_DATA "\x00\xEA\x00\x00\x00\x00\x00\x00"
#define CAN_FRAME_ACK_DATA "\xEB\xFE\x00\x00\x00\x00\x00\x00"
#define CAN_FRAME_J1939Req_DATA "\x11\x07\x01\xFF\xFF\xEB\xFE\x00"

//#define CAN_FRAME_BAM_DATA "\x20\x00\x0A\x02\xFF\xCA\xFE\x00"
//#define CAN_FRAME_TPDT_FIRST_DATA "\x01\xFF\xFF\xDC\xBE\x1A\x01\xAD"
//#define CAN_FRAME_TPDT_SECOND_DATA "\x02\xDE\xBE\x01\x55\x55\x55\x55"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "include/cangov.h"

void writeFrame(int socketno,struct can_frame *frame)
{
    if (write(socketno,frame, sizeof(struct can_frame)) < 0) {
            perror("Write error");
            close(socketno);
            exit(EXIT_FAILURE);
    }
    printf("Message sent with CAN ID: %08X\n", frame->can_id & CAN_EFF_MASK);
}

int main() {
    int nbytes;
    int sockfd;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;
    // Create socket
    if ((sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Enable loopback functionality
    int loopback = 1;  
    if (setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback)) < 0) {
        perror("Loopback enable error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Specify the CAN interface
    strcpy(ifr.ifr_name, CAN_INTERFACE);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Binding error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    frame.can_id = (0x18EEFB00 ) | CAN_EFF_FLAG; // Different CAN IDs for each message
    frame.can_dlc = CAN_FRAME_LENGTH;
    memcpy(frame.data, CAN_ACM_DATA, CAN_FRAME_LENGTH);
    writeFrame(sockfd,&frame);

    frame.can_id = (0x18EA00FB ) | CAN_EFF_FLAG; // Different CAN IDs for each message
    frame.can_dlc = CAN_FRAME_LENGTH;
    memcpy(frame.data, CAN_FRAME_DATA, CAN_FRAME_LENGTH);
    writeFrame(sockfd,&frame);

    // Send CAN frame using sendto
    frame.can_id = (0x18EA00FB ) | CAN_EFF_FLAG; // Different CAN IDs for each message
    frame.can_dlc = CAN_FRAME_LENGTH;
    memcpy(frame.data, CAN_FRAME_ACK_DATA, CAN_FRAME_LENGTH);
    writeFrame(sockfd,&frame);
    sleep(1);

    frame.can_id = (0x1CEC00FB ) | CAN_EFF_FLAG; // Different CAN IDs for each message
    frame.can_dlc = CAN_FRAME_LENGTH;
    memcpy(frame.data, CAN_FRAME_J1939Req_DATA, CAN_FRAME_LENGTH);
    writeFrame(sockfd,&frame);
  
   // Receive CAN frames
    while (1) {
        nbytes = read(sockfd, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            close(sockfd);
            exit(EXIT_FAILURE);
        } else {
            printf("Received message with CAN ID: %08X\n", frame.can_id & CAN_EFF_MASK);
             printf("Recieved CAN data: ");
            for (int i = 0; i < frame.can_dlc; ++i) {
            printf("%02X ", frame.data[i]);
        }
        }
    }

    // Close socket (This code will not be reached due to the infinite loop)
    close(sockfd);

    return 0;
}