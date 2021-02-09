#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

unsigned int tmp=0U;

int main(int argc, char **argv)
{
	int simple=open("/dev/simple",O_RDWR);
	int para,opt;
	if(simple<0){
		perror("open /dev/simple");
		exit(1);	
	}

	while ((para = getopt(argc, argv, "1234")) != -1) {
		switch (para) {
		case '1':
			opt = 0x11;
			break;

		case '2':
			opt = 0x12;
			break;

		case '3':
			opt = 0x13;
			break;

		case '4':
			opt = 0x14;
			break;
		default:
			fprintf(stderr, "Usage: %s [ -1 | -2 | -3 | -4 ]\n",
				argv[0]);
			return 1;
		}
		switch(opt){
		case 0x11:
			if(ioctl(simple,0x11,&tmp)<0){
				perror("read Guest cr3");
				exit(1);
			}
			printf("Host rip=:%x\n",tmp);
			break;

test:		case 0x12:
			if(ioctl(simple,0x12,&tmp)<0){
				perror("read Exception Bitmap");
				exit(1);
			}
			printf("Exception bitmap:%x\n",tmp);
			break;
		
		case 0x13:
			if(ioctl(simple,0x13,&tmp)<0){
				perror("write Exception Bitmap");
				exit(1);
			}
			goto test;
		case 0x14:
			if(ioctl(simple,0x14,&tmp)<0){
				perror("read PIN BASED");
				exit(1);
			}
			printf("PIN_BASED_VM_EXEC_CONTROL:%x\n",tmp);
			break;
		}
	}

	return 0;
}
