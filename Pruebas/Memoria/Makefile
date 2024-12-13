KERNELRELEASE = $(shell uname -r)
EXTRA_CFLAGS += -I/lib/modules/$(shell uname -r)/build/include
EXTRA_CFLAGS += -I/lib/modules/$(shell uname -r)/build/kernel

obj-m += memoria.o

all: 
	make -C /lib/modules/$(KERNELRELEASE)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(KERNELRELEASE)/build M=$(PWD) clean
	
