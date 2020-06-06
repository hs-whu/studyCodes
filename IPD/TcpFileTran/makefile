# variable

C_FLAGS := 
CC := gcc

#
.PHONY: default

default:r s

r:RecvFile.c
	$(CC) $(C_FLAGS) -o $@ $^

s:SendFile.c
	$(CC) $(C_FLAGS) -o $@ $^

clean:
	rm -rf s r -R files
