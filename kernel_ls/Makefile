# program name: Makefile( Makefile for super_blocks.c )
# date: 2013-4-15

obj-m := file_info.o
CURRENT_PATH := $(shell pwd)
LINUX_KERNEL := $(shell uname -r)
LINUX_KERNEL_PATH := /usr/src/linux-headers-$(shell uname -r)

all: 
#	make -C $(LINUX_KERNEL_PATH) M= /home/lixin/Documents/practice/kernel modules
	make -C /usr/src/linux-headers-3.2.0-38-generic M=/home/lixin/Documents/practice/kernel/file_info modules
#	make -C /usr/src/linux-headers-3.2.0-38-generic M= $(CURRENT_PATH) modules
clean:
	make -C /usr/src/linux-headers-3.2.0-38-generic M=/home/lixin/Documents/practice/kernel/file_info clean
