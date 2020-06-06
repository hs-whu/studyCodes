# variable

C_FLAGS := -m32 -z execstack -fno-stack-protector
CC := gcc

#
.PHONY: default

default:s c
	

s:server.c
	$(CC) $(C_FLAGS) -o $@ $^

c:client.c
	$(CC) $(C_FLAGS) -o $@ $^

clean:
	-rm s c
