#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include "calcPacket.h"

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr(fd, &tty) != 0)
        {
                printf("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
        {
                printf("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset(&tty, 0, sizeof tty);
        if (tcgetattr(fd, &tty) != 0)
        {
                printf("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr(fd, TCSANOW, &tty) != 0)
                printf("error %d setting term attributes", errno);
}

int main (int argc, char* argv[])
{

	char *portname = "/dev/ttyUSB0";

	int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0)
		{
        	printf("error %d opening %s: %s", errno, portname, strerror (errno));
        	return -1;
		}

	set_interface_attribs(fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking(fd, 0);                // set no blocking

	uint8_t i;
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



	write(fd, payload, strlen(payload));           // send 7 character greeting

	usleep((strlen(payload) + 25) * 100);             // sleep enough to transmit the 7 plus
                                     // receive 25:  approx 100 uS per char transmit
	char buf[BUFFER_SIZE];
	int n = read(fd, buf, sizeof buf);  // read up to 100 characters if ready to read

 	return 0 ;

}
