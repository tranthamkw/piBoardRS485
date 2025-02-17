#include <libusb.h>
#include "calcPacket.h"

int main (int argc, char* argv[]){

    libusb_device **devs;

    libusb_device_handle *device_handle;

    int ret, i, num_devices;

    int endpoint = 0x01; // Example OUT endpoint address [2, 3]

    unsigned char command[BUFFER_SIZE];
    strcpy((char*)command,argv[1]);  // command to send to a GPIB device
    printf("Input string %s\n",command);

    packet(command,strlen(command));
    for (i=0;i<strlen(command);i++){
        printf("%02x ",command[i]);
    }
    printf("\n");
    
    printf("Payload \n");
    for (i=0;i<strlen(payload);i++){
        printf("%02x ",payload[i]);
    }
    printf("\n");

    // Initialize libusb

    int data_size = sizeof(payload);


    ret = libusb_init(&NULL);

    if (ret < 0) {

        fprintf(stderr, "Error initializing libusb\n");

        return 1;

    }



    // Find the desired USB device

    num_devices = libusb_get_device_list(NULL, &devs);

//    for (i = 0; i < num_devices; i++) {

        // Check vendor ID and product ID to find the correct device

  //      if (/* Check vendor ID and product ID here */) {

            device_handle = libusb_open(devs[0], 0); 

    //        break;

      //  }

   // }

    

    // Claim interface

    ret = libusb_claim_interface(device_handle, 0);

    if (ret < 0) {

        fprintf(stderr, "Error claiming interface\n");

        return 1;

    }



    // Send data

    ret = libusb_bulk_transfer(device_handle, endpoint, payload, data_size, 0, 0);

    if (ret < 0) {

        fprintf(stderr, "Error sending data\n");

    }



    // Cleanup

    libusb_close(device_handle);

    libusb_exit(NULL);

    return 0;

}

