#
# KBUILD build parameters.
#

MODULE_NAME = fmon

SOURCE_OBJS = src/dentry.o src/inode.o src/main.o src/vfs.o src/device.o src/proc.o
SOURCE_HDRS	= include/dentry.h include/inode.h include/main.h include/vfs.h \
	include/device.h

EXTRA_CFLAGS = -Wall -g -D__KERNEL__ -DMODULE -DLINUX -I$(src)/include

KDIR	= /lib/modules/`uname -r`/build
PWD		= $(shell pwd)

obj-m = $(MODULE_NAME).o
$(MODULE_NAME)-objs = $(SOURCE_OBJS)
$(MODULE_NAME)-hdrs = $(SOURCE_HDRS)

default:
		$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
		-rm -rf $(SOURCE_OBJS) $(MODULE_NAME).{o,ko,mod.*} .depend .*.cmd *.ko *.mod.c \
		.tmp_versions modules.order Module.symvers src/.*.cmd
