obj-m := message_slot.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: modules message_sender

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

message_sender:
	gcc -O3 -Wall -std=c11 message_sender.c -o message_sender

message_reader:
	gcc -O3 -Wall -std=c11 message_reader.c -o message_reader

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f message_sender
	rm -f message_reader