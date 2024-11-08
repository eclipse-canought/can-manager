#define CAN_INTERFACE_IN "vcan1"
#define CAN_INTERFACE_OUT "can0"
#define CAN_FRAME_MAX_DLEN 8
#define BUFFER_SIZE 100

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <pthread.h>
 
pthread_t rx_messagethread;

int s_in, s_out;
struct sockaddr_can can_addr_in, can_addr_out;
struct can_frame frame_in, frame_out;
struct ifreq ifr_in, ifr_out;

void *readRxFrame()
{
    struct can_frame rx_frame;
    struct can_frame tx_frame;
    struct sockaddr_can can_rx_sock;
    struct ifreq ifr;
    int s_rx;

    socklen_t len = sizeof(can_addr_out);

    while(1){

    recvfrom(s_out, &rx_frame, sizeof(struct can_frame),
                      0, (struct sockaddr*)&can_addr_out, &len);
    
    printf("Received Rx CAN message with ID: %08X\n", rx_frame.can_id & CAN_EFF_MASK);
    printf("Received Rx CAN data: ");
        for (int i = 0; i < rx_frame.can_dlc; ++i) {
            printf("%02X ", rx_frame.data[i]);
        }

     tx_frame.can_id = rx_frame.can_id  | CAN_EFF_FLAG;
        tx_frame.can_dlc = rx_frame.can_dlc;
        memcpy(tx_frame.data, rx_frame.data, rx_frame.can_dlc);
    printf("Transmitting Tx CAN message with ID: %08X\n", tx_frame.can_id & CAN_EFF_MASK);
    printf("Transmitting Tx CAN data: ");
        for (int i = 0; i < tx_frame.can_dlc; ++i) {
            printf("%02X ", tx_frame.data[i]);
        }
   
    sendto(s_in, &tx_frame, sizeof(struct can_frame),
                    0, (struct sockaddr*)&can_addr_in, sizeof(can_addr_in));

    }

}

int main() {
   char buffer[BUFFER_SIZE];
   
    // Create a socket for receiving frames from vcan0
    if ((s_in = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation error for receiving frames");
        exit(EXIT_FAILURE);
    }

    // Create a socket for sending frames to vcan1
    if ((s_out = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation error for sending frames");
        close(s_in);
        exit(EXIT_FAILURE);
    }

    // Create the virtual CAN interface for receiving frames (vcan0)
    strcpy(ifr_in.ifr_name, CAN_INTERFACE_IN);
    ioctl(s_in, SIOCGIFINDEX, &ifr_in);
    // Bind the receiving socket to the virtual CAN interface (vcan0)
    can_addr_in.can_family = AF_CAN;
    can_addr_in.can_ifindex = ifr_in.ifr_ifindex;
    if (bind(s_in, (struct sockaddr*)&can_addr_in, sizeof(can_addr_in)) < 0) {
        perror("Bind error for receiving frames");
        close(s_in);
        close(s_out);
        exit(EXIT_FAILURE);
    }

    strcpy(ifr_out.ifr_name, CAN_INTERFACE_OUT);
    ioctl(s_out, SIOCGIFINDEX, &ifr_out);
    // Bind the sending socket to the CAN interface (can0)
    can_addr_out.can_family = AF_CAN;
    can_addr_out.can_ifindex = ifr_out.ifr_ifindex;
    if (bind(s_out, (struct sockaddr*)&can_addr_out, sizeof(can_addr_out)) < 0) {
        perror("Bind error for sending frames");
        close(s_in);
        close(s_out);
        exit(EXIT_FAILURE);
    }
    pthread_create( &rx_messagethread, NULL, &readRxFrame, NULL);
    // Main loop to continuously receive and process CAN frames
    while (1) {
        // Receive CAN frame from vCAN bus (vcan0)
        if (read(s_in, &frame_in, sizeof(frame_in)) < 0) {
            perror("Read error");
            close(s_in);
            close(s_out);
            exit(EXIT_FAILURE);
        }

        // Print the received CAN message ID and data
        printf("Received Tx CAN message with ID: %08X\n", frame_in.can_id & CAN_EFF_MASK);
        printf("Received CAN message data: ");
        for (int i = 0; i < frame_in.can_dlc; ++i) {
            printf("%02X ", frame_in.data[i]);
        }
        printf("\n");
        frame_out.can_id = frame_in.can_id  | CAN_EFF_FLAG;
        frame_out.can_dlc = frame_in.can_dlc;
        memcpy(frame_out.data, frame_in.data, frame_in.can_dlc);

        if (write(s_out, &frame_out, sizeof(frame_out)) < 0) {
            perror("Write error");
            close(s_in);
            close(s_out);
            exit(EXIT_FAILURE);
        }

    }
    
    // Close the receiving socket and sending socket (this code will not be reached due to the infinite loop)
    close(s_in);
    close(s_out);
    pthread_join( rx_messagethread, NULL);
    return 0;
}