all: libmem.so

libmem.so:
	# your compile commands
	gcc -c -fpic mem.c -Wall -Werror
	gcc -shared -o libmem.so mem.o

clean:
	rm -f libmem.so mem.o
# more compile commands
