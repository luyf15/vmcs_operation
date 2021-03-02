#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

unsigned int tmp=0U;

int main()
{
	int simple=open("/dev/simple",O_RDWR);

	if(simple<0){
		perror("open /dev/simple");
		exit(1);	
	}
	if(ioctl(simple,0x11,&tmp)<0){
		perror("read Guest cr3");
		exit(1);
	}
	printf("Guest cr3:%x\n",tmp);
	if(ioctl(simple,0x12,&tmp)<0){
		perror("read Exception Bitmap");
		exit(1);
	}
	printf("Exception bitmap:%x\n",tmp);
	if(ioctl(simple,0x13,&tmp)<0){
		perror("write Exception Bitmap");
		exit(1);
	}
	if(ioctl(simple,0x12,&tmp)<0){
		perror("read Exception Bitmap");
		exit(1);
	}
	printf("Exception bitmap:%x\n",tmp);
	if(ioctl(simple,0x14,&tmp)<0){
		perror("read PIN BASED");
		exit(1);
	}
	printf("PIN_BASED_VM_EXEC_CONTROL:%x\n",tmp);
	if(ioctl(simple,0x15,&tmp)<0){
		perror("read CPU BASED");
		exit(1);
	}
	printf("CPU_BASED_VM_EXEC_CONTROL:%x\n",tmp);
	if(ioctl(simple,0x16,&tmp)<0){
		perror("read SECONDARY");
		exit(1);
	}
	printf("SECONDARY_VM_EXEC_CONTROL:%x\n",tmp);
	if(ioctl(simple,0x17,&tmp)<0){
		perror("read cr0 G/H");
		exit(1);
	}
	printf("CR0 GUEST HOST MASK:%x\n",tmp);
	if(ioctl(simple,0x18,&tmp)<0){
		perror("read SECONDARY");
		exit(1);
	}
	printf("CR0 READ SHADOW:%x\n",tmp);
	return 0;
}
