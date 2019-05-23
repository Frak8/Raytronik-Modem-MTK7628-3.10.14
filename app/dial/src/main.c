#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include <net/if.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<sys/ioctl.h>
#include <linux/if_ether.h>  
#include"serial.h"
#define UP 1
#define DOWN 0
#define YES 1
#define NO 0
#define FAILURE 1
#define SUCESS  0
#define WAN_IF "eth0.2"
#define USB0_OPERSTATE "/sys/class/net/usb0/operstate"
#define WAN_RX "/sys/class/net/eth0.2/statistics/rx_bytes"
#define PING_CMD "/bin/ping -c 1 180.97.33.108 | /usr/bin/wc -l"
extern void env_check(void);
extern int dial_up(int);
extern int hang_down(int);
typedef unsigned int bool;

int open_file(char *file)
{
    int operstate;
    operstate = open( file, O_RDONLY );
    if( operstate <= 0 )
    {
    	perror("open_file");
	printf("\nThe network interface usb0 is not exgist!");
    }
    return operstate;
}

bool get_usb0_operstate()
{
    int fd;
    fd = open_file( USB0_OPERSTATE );
    if( fd <= 0 )
    {
	return DOWN;
    }
    char status[5] = "\0";
    int count;
    count = read( fd, status, 4 );
    
    if (count == 3 )
    {
	close(fd);
	return UP;
    }
    if (count == 4 )
    {
	close(fd);
	return DOWN;
    }
    return DOWN;
}

void cmd_get(char *cmd, char *value)
{
    FILE *fdStreanPipe = popen(cmd, "r");
    fgets(value, 2, fdStreanPipe);
    printf("value:%s\n",value);
    pclose(fdStreanPipe);
}

static char or_rx[16] = {"0"};
bool wan_rx_added()
{
    FILE *fd_rx = fopen(WAN_RX, "r");
    char cur_rx[16] = {"0"};
    fgets(cur_rx, 16, fd_rx);
    fclose(fd_rx);
    printf("cur_rx:%s\n", cur_rx);
    if(!strcmp(or_rx, cur_rx))
    {
	printf("no,added\n");
	return NO;
    }else
    {
	printf("yes,added\n");
	strcpy(or_rx, cur_rx);
	return YES;
    } 
}

static int is_linked = NO;
bool check_wan_link_status(char *netdev)
{
    int inet_sock;
    struct ifreq ifr;
    struct in_addr addr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, netdev);
    if( is_linked == NO )
    {
	if (ioctl(inet_sock, SIOCGIFADDR, &ifr) < 0 || wan_rx_added() == NO)
	{
	    printf("ioctl < 0\n");
	    is_linked = NO;
	    return NO;
	}else
	{
	    printf("ioctl != 0 \n");
	    is_linked = YES;
	    return YES;
	}
    }else
    {
	char nline[2];
	cmd_get(PING_CMD, nline);
	
	int num = nline[0] - '0';
	printf("ping num:%d\n", num);
	if(num >= 5)
	{
	    is_linked = YES;
	    return YES;
	}else
	{
	    is_linked = NO;
	    return NO;
	}
    }	

}

int main()
{
    printf("\n************************************\n\n");
    env_check();
    printf("\n************************************\n\n");
    static bool usb0_operstate = DOWN;
    static bool wan_is_connect = NO;
    static bool have_dial = NO;
    int fdSerial;
    while(1)
    {
	//Get usb0 interface switch status
	usb0_operstate = get_usb0_operstate();
	wan_is_connect = check_wan_link_status(WAN_IF);

	sleep(3);

	if( usb0_operstate == UP && have_dial == NO && wan_is_connect == NO)
	{
	    fdSerial = open_serial_port(AT_TTY);
	    set_serial_port(fdSerial, 115200, 8, 1, 'N');
	    printf("now everything is ready!\n\n");
	    printf("****************************************\n\n");
	    //use at-command dial up
	    printf("read for dailing....\n");
	    int rt;
	    rt = dial_up(fdSerial);
	    if( rt == FAILURE )
	    {
		sleep(2);
		continue;
	    }else if( rt == SUCESS )
	    {
		printf("dail up sucessfull!\n");
		printf("************************************\n\n");
		have_dial = YES;
	    }
	    close(fdSerial);
	}
	if( have_dial == YES && wan_is_connect == YES)
	{
	    fdSerial = open_serial_port(AT_TTY);
	    set_serial_port(fdSerial, 115200, 8, 1, 'N');
	    printf("now everything is ready!\n\n");
	    printf("****************************************\n\n");
	    //use at-command hang up
	    printf("read for hanging....\n");
	    int rt;
	    rt = hang_down(fdSerial);
	    if( rt == FAILURE )
	    {
		sleep(2);
		continue;
	    }else if( rt == SUCESS )
	    {
		printf("hang up sucessfull!\n");
		printf("************************************\n\n");
		have_dial = NO;
	    }
	    close(fdSerial);
	}

	sleep(5);

    }

    return 0;
}

/*
int check_nic(void)  
{  
    struct ifreq ifr;  
    int skfd = socket(AF_INET, SOCK_DGRAM, 0);  

    strcpy(ifr.ifr_name, WAN_IF);  
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)  
    {  
	close(skfd);  
	return -1;  
    }  
    if(ifr.ifr_flags & IFF_RUNNING){  
	printf("link up\n");  
	close(skfd);  
	return 0; // 网卡已插上网线  
    }else {  
	printf("link down\n");  
	close(skfd);  
	return -1;  
    }  
}
*/
