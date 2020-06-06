# variable

C_FLAGS := -m32 -std=c99 -D_GNU_SOURCE -z execstack -fno-stack-protector -mpreferred-stack-boundary=2 -Wno-format-security
CC := gcc

#
.PHONY: default

default:s e shellcode

s:server.c
	$(CC) $(C_FLAGS) -o $@ $^

e:exploit.c
	$(CC) $(C_FLAGS) -o $@ $^

shellcode:shellcode.o
	ld -m elf_i386 -s -o $@ $^

shellcode.o:shellcode.asm
	nasm -f elf32 -o $@ shellcode.asm

clean:
	rm -rf s e shellcode shellcode.o hs.txt
