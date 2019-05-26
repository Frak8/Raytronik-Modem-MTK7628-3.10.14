#include <stdio.h>
#include <stdlib.h>
void test_device(void)
{
 int test_device = system("AT --AT 2&>1 /dev/null");
	if (test_device == 0)
	{printf("Device is ready\n");}
	else{
	printf("Kernel Driver is not Correct or Module is not Present\n");	
	exit(1);}
}
 
	int data_collector(int argc, char *argv[])
	{ 
	FILE *fp;
        char path[1035];
	fp = popen("dmesg", "r");
        if (fp == NULL) {
        printf("Failed to run command\n" );
      //  exit(1);
		while (fgets(path, sizeof(path)-1, fp) != NULL) {
		    printf("%s", path);

	pclose(fp);
	//path=NULL;
	return 0; 
  }      


  } 


		}
int main( int argc, char *argv[] )
{
//	  data_collector();
//	printf("%s", out);
//		test_device();
//	int imsi =  get_IMSI();
//  	FILE *fp;
//  	char path[1035];

  /* Open the command for reading. */
//  	fp = popen("AT --IMSI", "r");
//  	if (fp == NULL) {
//    	printf("Failed to run command\n" );
//    	exit(1);
//  	}

  /* Read the output a line at a time - output it. */
//  while (fgets(path, sizeof(path)-1, fp) != NULL) {
//    printf("%s", path);
//  }

  /* close */
//  pclose(fp);

  return 0;
}
