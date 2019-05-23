#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include <linux/types.h>


struct mtd_info_user {
	u_char type;
	u_int32_t flags;
	u_int32_t size;
	u_int32_t erasesize;
	u_int32_t oobblock;
	u_int32_t oobsize;
	u_int32_t ecctype;
	u_int32_t eccsize;
};


struct erase_info_user {
	u_int32_t start;
	u_int32_t length;
};

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)




int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
				if ((ret = open(dev, flags)) < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}


#define min(x,y) ({ typeof(x) _x = (x); typeof(y) _y = (y); (void) (&_x == &_y); _x < _y ? _x : _y; })

int flash_write_default(char *buf, off_t to, size_t len)
{
	int fd, ret = 0;
	char *bak = NULL;
	struct mtd_info_user info;
	struct erase_info_user ei;

	fd = mtd_open("Factory", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &info)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}
	if (len > info.size) {
		fprintf(stderr, "Too many bytes: %d > %d bytes\n", len, info.erasesize);
		close(fd);
		return -1;
	}

	while (len > 0) {
		if ((len & (info.erasesize-1)) || (len < info.erasesize)) {
			int piece_size;
			unsigned int piece, bakaddr;

			bak = (char *)malloc(info.erasesize);
			if (bak == NULL) {
				fprintf(stderr, "Not enough memory\n");
				close(fd);
				return -1;
			}

			bakaddr = to & ~(info.erasesize - 1);
			lseek(fd, bakaddr, SEEK_SET);

			ret = read(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Reading from mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			piece = to & (info.erasesize - 1);
			piece_size = min(len, info.erasesize - piece);
			memcpy(bak + piece, buf, piece_size);

			ei.start = bakaddr;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			lseek(fd, bakaddr, SEEK_SET);
			ret = write(fd, bak, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			free(bak);
			buf += piece_size;
			to += piece_size;
			len -= piece_size;
		}
		else {
			ei.start = to;
			ei.length = info.erasesize;
			if (ioctl(fd, MEMERASE, &ei) < 0) {
				fprintf(stderr, "Erasing mtd failed\n");
				close(fd);
				return -1;
			}

			ret = write(fd, buf, info.erasesize);
			if (ret == -1) {
				fprintf(stderr, "Writing to mtd failed\n");
				close(fd);
				free(bak);
				return -1;
			}

			buf += info.erasesize;
			to += info.erasesize;
			len -= info.erasesize;
		}
	}

	close(fd);
	return ret;
}

char char_to_num(char c)
{
	if (c <= '9')
	{
		return (c-'0');
	}	
	else
	{
		return (c-'A'+10);
	}	
}	



int main(int argc, char *argv[])
{
	int i;
	int ret;
	
	unsigned int base_mac_h;
	unsigned int base_mac_l;
	char mac_buf[18];
	
	if (argc != 2)
	{
		printf("Bad command type!\n");
		printf("Usage: nvram_mac <mac>\n");
		return -1;
	}		
	
	if (strlen(argv[1]) != 12)
	{
		printf("Bad size of mac\n");
		printf("The mac must be 12 characters!\n");
		return -1;
	}	
	
	//check the mac number
	for (i=0; i<12; i++)
	{
		if ((argv[1][i]<'0') || ((argv[1][i]>'9')&&(argv[1][i]<'A')) || (argv[1][i]>'F'))
		{
			printf("Bad mac number!\n");
			printf("The mac must be '0-9' or 'A-F'\n");
			return -1;
		}				
	}	
	
	//Get the mac value
	base_mac_h = (((unsigned int)char_to_num(argv[1][0]))<<12) + (((unsigned int)char_to_num(argv[1][1]))<<8) + (((unsigned int)char_to_num(argv[1][2]))<<4) 
							+ ((unsigned int)char_to_num(argv[1][3]));
							
	base_mac_l = (((unsigned int)char_to_num(argv[1][4]))<<28) + (((unsigned int)char_to_num(argv[1][5]))<<24) + (((unsigned int)char_to_num(argv[1][6]))<<20) 
							+ (((unsigned int)char_to_num(argv[1][7]))<<16) + (((unsigned int)char_to_num(argv[1][8]))<<12) + (((unsigned int)char_to_num(argv[1][9]))<<8)  
							+ (((unsigned int)char_to_num(argv[1][10]))<<4) + ((unsigned int)char_to_num(argv[1][11]));
	
	//ra0 mac
	mac_buf[0] = (char)((base_mac_h >> 8) & 0xff);
	mac_buf[1] = (char)(base_mac_h & 0xff);
	
	for (i=2; i<6; i++)
	{
		mac_buf[i] = (char)((base_mac_l>>((5-i)*8)) & 0xff);
	}	
	
	//wan mac
	base_mac_l++;
	
	if (base_mac_l == 0)
	{
		base_mac_h++;
	}	
		
	mac_buf[12] = (char)((base_mac_h >> 8) & 0xff);
	mac_buf[13] = (char)(base_mac_h & 0xff);
	
	for (i=14; i<18; i++)
	{
		mac_buf[i] = (char)((base_mac_l>>((5+12-i)*8)) & 0xff);
	}	
	
	//lan mac
	base_mac_l++;
	
	if (base_mac_l == 0)
	{
		base_mac_h++;
	}	
		
	mac_buf[6] = (char)((base_mac_h >> 8) & 0xff);
	mac_buf[7] = (char)(base_mac_h & 0xff);
	
	for (i=8; i<12; i++)
	{
		mac_buf[i] = (char)((base_mac_l>>((5+6-i)*8)) & 0xff);
	}
		
	//Update ra0 mac	
	ret = flash_write_default(mac_buf, 0x04, 6);
	
	if (ret == 0x10000)
	{
		printf("Update ra0 mac success!\n");
	}			
	else
	{
		printf("Update ra0 mac failed!\n");
		return -1;
	}	
	
	//Update wan and lan mac
	ret = flash_write_default(&mac_buf[6], 0x28, 12);
	
	if (ret == 0x10000)
	{
		printf("Update lan mac success!\n");		
		printf("Update wan mac success!\n");
		return 0;
	}		
	else
	{
		printf("Update lan mac failed!\n");
		printf("Update wan mac failed!\n");
		return -1;
	}	
}	
