#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include "simple.h"

static unsigned long tmp = 0U;

int main()
{
	struct para *contain=malloc(sizeof(struct para));
	contain->out_data = &tmp;
	contain->in_data = 0U;
	contain->field = 0U;

	int simple = open("/dev/simple",O_RDWR);

	if(simple < 0){
		perror("open /dev/simple");
		exit(1);	
	}

	contain->field = GUEST_CR0;
	if(ioctl(simple,VMREAD,contain) < 0){
		perror("read Guest cr0");
		exit(1);
	}
	printf("Guest cr0:%lx\n",*(contain->out_data));

	contain->field = EXCEPTION_BITMAP;
	if(ioctl(simple,VMREAD,contain)<0){
		perror("read Exception Bitmap");
		exit(1);
	}
	printf("Exception bitmap:%lx\n",*(contain->out_data));

	contain->in_data = 0xffffffff;
	if(ioctl(simple,VMWRITE,contain)<0){
		perror("write Exception Bitmap");
		exit(1);
	}
	if(ioctl(simple,VMREAD,contain)<0){
		perror("read Exception Bitmap");
		exit(1);
	}
	printf("Exception bitmap:%lx\n",*(contain->out_data));

	free(contain);
	contain = NULL;
	return 0;
}
