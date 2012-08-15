

BSP_PREFIX = arm-fsl-linux-gnueabi-

ifndef LINUX_KERNEL_DIR
LINUX_KERNEL_DIR=../../linux
#LINUX_KERNEL_DIR= /lib/modules/$(shell uname -r)/build
endif

PHONY := all clean
all:
	make -C $(LINUX_KERNEL_DIR) M=`pwd` ARCH=arm CROSS_COMPILE=$(BSP_PREFIX) modules
	#make -C $(LINUX_KERNEL_DIR) M=`pwd` modules

clean:
	make -C $(LINUX_KERNEL_DIR) M=`pwd` ARCH=arm CROSS_COMPILE=$(BSP_PREFIX) clean

obj-m := gpiod.o


