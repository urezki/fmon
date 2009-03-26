#
# KBUILD build parameters.
#

MODULE_NAME := inode
SOURCE_OBJS := dentry.o
EXTRA_CFLAGS += -Wall -ggdb -D__KERNEL__ -DMODULE -DLINUX

ifneq ($(KERNELRELEASE),)
		obj-m := $(MODULE_NAME).o 
		$(MODULE_NAME)-objs := $(SOURCE_OBJS)
else
		KDIR	= /lib/modules/`uname -r`/build
		PWD	= $(shell pwd)

default:
		$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
		-rm -f $(SOURCE_OBJS)
		-rm -f $(MODULE_NAME).mod.*
		-rm -f $(MODULE_NAME).{o,ko}
endif
