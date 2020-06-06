# variable

C_FLAGS := 
CC := gcc

#
.PHONY: default

default:ping

ping:ping.c
	$(CC) $(C_FLAGS) -o $@ $^

clean:
	rm -rf ping
