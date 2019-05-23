#include<unistd.h>
#include<stdio.h>
#define AT_TTY "/dev/ttyUSB2"
#define USB0_OPERSTATE "/sys/class/net/usb0/operstate"

void check_env_print(int ret, char *ok, char *error)
{
    if( ret == 0)
    {
	printf("%s\n", ok);
    }
    else
    {
	printf("%s\n", error);
    }
}

void env_check()
{
    int ret;
    printf("starting to envrioment check....\n");
    ret = access(AT_TTY, R_OK | W_OK);
    check_env_print(ret, AT_TTY" is ok", AT_TTY" not exgist or not permission");
    ret = access(USB0_OPERSTATE, R_OK);
    check_env_print(ret, USB0_OPERSTATE" is ok",USB0_OPERSTATE" file can't read,may be you don't loader the usb netcard drive!");
}
