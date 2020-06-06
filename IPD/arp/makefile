# variable

C_FLAGS := 
CC := gcc

#
.PHONY: default

default:arp

arp:arp.c
	$(CC) $(C_FLAGS) -o $@ $^

clean:
	rm -rf arp
