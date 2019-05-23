#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#define BAUDRATE B115200
#define COM1 "/dev/ttyUSB3"
static int fd;
static struct termios oldtio,newtio;

//==============================================================
int tty_read(char *buf1,int nbytes)
{
int temp;
temp = read(fd,buf1,nbytes);
printf("Read string: %s\n", buf1);
return temp;
}
//==============================================================
int tty_end()
{
tcsetattr(fd,TCSANOW,&oldtio);
close(fd);
}
//==============================================================
int tty_writecmd(char *buf,int nbytes)
{ 

write(fd,buf,nbytes);

usleep(1000);
return tcdrain(fd);
}
//==============================================================
int baud = B115200;
int tty_init()
{
fd = open(COM1, O_RDWR );
if (fd <0) {
  perror(COM1);
  exit(1);
}

tcgetattr(fd,&oldtio);
bzero(&newtio, sizeof(newtio)); 
newtio.c_cflag = baud | CRTSCTS | CS8 | CLOCAL | CREAD ;
newtio.c_iflag = IGNPAR | ICRNL; 
newtio.c_oflag = 0; 
newtio.c_lflag = ICANON;
newtio.c_cc[VINTR]    = 0;     
newtio.c_cc[VQUIT]    = 0;     
newtio.c_cc[VERASE]   = 0;     
newtio.c_cc[VKILL]    = 0;    
newtio.c_cc[VEOF]     = 4;     
newtio.c_cc[VTIME]    = 0;
newtio.c_cc[VMIN]     = 1;
newtio.c_cc[VSWTC]    = 0;     
newtio.c_cc[VSTART]   = 0;     
newtio.c_cc[VSTOP]    = 0;
newtio.c_cc[VSUSP]    = 0; 
newtio.c_cc[VEOL]     = 0;
newtio.c_cc[VREPRINT] = 0; 
newtio.c_cc[VDISCARD] = 0;
newtio.c_cc[VWERASE]  = 0;
newtio.c_cc[VLNEXT]   = 0;
newtio.c_cc[VEOL2]    = 0; 
tcflush(fd, TCIFLUSH);
tcsetattr(fd,TCSANOW,&newtio);
return 0;
}

int main(int argc, char *argv[])
{ int wr,rd;
char *buff;
char recv[15];
char command[] = "AT\r\n";
//tty_init();
printf("Write: %d\n", tty_writecmd(command, sizeof(command)));
usleep(1000);
printf("Read: %d\n", tty_read(recv ,sizeof(recv)));
tty_end();
}
