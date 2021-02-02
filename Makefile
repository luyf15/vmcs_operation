obj-m += simple.o
CONFIG_MODULE_SIG=n

#KERNEL := /home/zed/open-source-code/linux-5.4.0
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	#make -C $(KERNEL) M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	#make -C $(KERNEL) M=$(PWD) clean
