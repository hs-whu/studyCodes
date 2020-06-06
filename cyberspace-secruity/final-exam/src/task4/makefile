# variable

C_FLAGS := -m32 -std=c99 -D_GNU_SOURCE -z execstack -fno-stack-protector -mpreferred-stack-boundary=2 -Wno-format-security
CC := gcc

#
.PHONY: default

default:s e shellcode daemon recv exs install

s:server.c
	$(CC) $(C_FLAGS) -o $@ $^

e:exploit.c
	$(CC) $(C_FLAGS) -o $@ $^

shellcode:shellcode.o
	ld -m elf_i386 -s -o $@ $^

shellcode.o:shellcode.asm
	nasm -f elf32 -o $@ shellcode.asm

daemon:daemon.c
	gcc -o $@ $^ -lssl -lcrypto

recv:RecvFile.c
	gcc -o $@ $^ -lssl -lcrypto

exs:exs.c
	gcc -o $@ $^

install:
	sudo mv daemon /var/www/html/daemon

clean:
	rm -rf s e shellcode shellcode.o daemon recv exs
