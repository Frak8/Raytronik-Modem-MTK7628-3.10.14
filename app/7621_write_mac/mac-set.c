#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#define FACTORY_PARTITION_FILE "/bin/factory-partition.bin"
//#define FACTORY_PARTITION_FILE "/home/zbt123/test/factory-partition.bin"

int main(int argc, char *argv[])
{
    printf("New mac: %s\n", argv[1]);

    unsigned char mac[7], ch;
    int factory, count;
    int i, k;
    factory = open(FACTORY_PARTITION_FILE, O_RDWR);

    if(factory < 0)
    {
	perror("open.");
    }


    for(k = i = 0; i < 6; i++)
    {
	mac[i] = 0;

	//hight bits
	if( isdigit(argv[1][k]) )
	{	
	    mac[i] += (argv[1][k] - 48) * 16;
	}else if(isupper(argv[1][k]))
	{
	    mac[i] += (argv[1][k] - 55) * 16;
	}else if(islower(argv[1][k]))
	{
	    mac[i] += (argv[1][k] -87) * 16;
	}else
	{
	    printf("address mac error.\n");
	}

	//low bits	
	if( isdigit(argv[1][k+1]) )
	{	
	    mac[i] += argv[1][k+1] - 48;
	}else if(isupper(argv[1][k+1]))
	{
	    mac[i] += argv[1][k+1] - 55;
	}else if(islower(argv[1][k+1]))
	{
	    mac[i] += argv[1][k+1] -87;
	}else
	{
	    printf("address mac error.\n");
	}
	k += 3;
    }

    lseek(factory, 57344, SEEK_SET);
    for(i = 0; i < 6 ; i++)
    {
	write(factory, &mac[i], 1);
	printf("%x:", mac[i]);
    }
    printf("\n");

    for(i = 0; i < 6 ; i++)
    {
	write(factory, &mac[i], 1);
	printf("%x:", mac[i]);
    }
    printf("\n");
    close(factory);
}
