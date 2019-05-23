#include<stdio.h>
#include<string.h>
#define RV_LEN 64
#define FAILURE 1
#define SUCESS  0

int dial_up(int fdSerial)
{
    char at_dial_cmd[6][32] = 
    {
	"AT\r",
	"AT+CSQ\r",
	"AT+COPS?\r",
	"AT+PSRAT\r",
	"AT+CGDCONT=1,\"IP\",",
	"AT$QCRMCALL=1,1,1,2,1\r"
    };
    char rv[RV_LEN], *p = NULL;
    int n;
    printf("test something before dial up.\n");
/*    
    n= write(fdSerial, at_dial_cmd[0], 3);
    printf("AT:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(3);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);

    n= write(fdSerial, at_dial_cmd[1], 7);
    printf("AT+CSQ:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(3);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
*/
    n= write(fdSerial, at_dial_cmd[2], 9);
    printf("AT+COPS?:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(1);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
    printf("***auto select APN***\n");

    int founded_apn = 0;

    if( founded_apn == 0 )
    {
	p = strstr(rv, "CHINA TELECOM");
	if( p )
	{
	    strcat(at_dial_cmd[4],"\"CTLTE\"\r");
	    founded_apn = 1;
	}
    }
    if( founded_apn == 0 )
    {
	p = strstr(rv, "CHINA  MOBILE");
	if( p )
	{
	    strcat(at_dial_cmd[4],"\"CMNET\"\r");
	    founded_apn = 1;
	}
    }
    if( founded_apn == 0 )
    {
	p = strstr(rv, "CHN-UNICOM");
	if( p )
	{
	    strcat(at_dial_cmd[4],"\"3GNET\"\r");
	    founded_apn = 1;
	}
    }
    if( founded_apn == 0)
    {
	printf("error,unknow APN,retry!\n");
	return FAILURE;
    }
    p = NULL;
    
    n= write(fdSerial, at_dial_cmd[3], 9);
    printf("AT+PSRAT:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(1);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
    
    n= write(fdSerial, at_dial_cmd[4], 26);
    printf("AT+CGDCONT:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(1);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
    
    n= write(fdSerial, at_dial_cmd[5], 22);
    printf("AT$QCRMCALL:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(1);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
    p = strstr(rv, "OK");
    
    if( p != NULL )
    {
	sleep(1);
	system("/sbin/udhcpc -i usb0");
	return SUCESS;
    }

    return FAILURE;
}
