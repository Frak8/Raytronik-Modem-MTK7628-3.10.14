#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> /* File control definitions */
#include <errno.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
//#include "serial.h"

int open_serial_port(char *dev)
{
	int fdSerial = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fdSerial == -1)
	{
		perror("open_serial_port!");
		exit(0);
	}
	else
	{
		printf("open tty ok! fd:%d\n", fdSerial);
		return (fdSerial);
	}

	if(isatty(fdSerial)==0)
	{
		printf("standard input is not a terminal device\n");
		perror("note:");
	}
}

int set_serial_port(int fdSerial, int nSpeed, int nBits, int nStop, char verify)
{
	struct termios newtio;
	bzero( &newtio, sizeof( newtio) );
	newtio.c_cflag |= CLOCAL | CREAD; 
	
	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		default:
			cfsetispeed(&newtio, B57600);
			cfsetospeed(&newtio, B57600);
			break;
	}
	
	newtio.c_cflag &= ~CSIZE; 
	switch( nBits )
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch( verify)
	{
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E': 
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N': 
			newtio.c_cflag &= ~PARENB;
			break;
	}

	if( nStop == 1 )
		newtio.c_cflag &= ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |= CSTOPB;

	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;
	tcflush(fdSerial,TCIOFLUSH);

	if((tcsetattr(fdSerial,TCSANOW,&newtio))!=0)
	{
		printf("set serial port failure!\n");
		perror("note:");
		return -1;
	}
	printf("setup tty ok!\n");
	return 0;
}
