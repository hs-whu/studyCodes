# variable
TARGET	= cacu
DIRS = src
FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,%.o,$(FILES))

#
.PHONY: clean new

#
$(TARGET):$(OBJS)
	gcc -o $@ $(OBJS)

$(OBJS):%.o:%.c
	gcc -o $@ -c $^

clean:
	-rm $(OBJS)

new:clean $(TARGET)






