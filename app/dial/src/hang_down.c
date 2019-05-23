#include<stdio.h>
#include<string.h>
#define RV_LEN 64
#define FAILURE 1
#define SUCESS  0
char at_hang_cmd[1][32] =
{
    "AT$QCRMCALL=0,1\r"
};
int hang_down(int fdSerial)
{
    char at_hang_cmd[1][32] =
    {
	"AT$QCRMCALL=0,1\r"
    };
    char rv[RV_LEN], *p = NULL;
    int n;
    n= write(fdSerial, at_hang_cmd[0], 16);
    printf("AT$QCRMCALL:%d\n", n);
    bzero(rv, RV_LEN);
    sleep(3);
    read(fdSerial, rv, RV_LEN);
    printf("%s \n\n", rv);
    p = strstr(rv, "OK");
    if( p != NULL )
    {
	return SUCESS;
    }

    return FAILURE;
}
