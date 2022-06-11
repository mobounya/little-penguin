obj-m += main.o

all:
	make -C /lib/modules/$(shell /usr/bin/uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell /usr/bin/uname -r)/build/ M=$(PWD) clean
