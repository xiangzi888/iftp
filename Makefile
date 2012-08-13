ALL= iftp
CC=  gcc
CFLAGS= -g -Wall -DDEBUG

all: $(ALL)

main.o: ftp.h cmds.h
ftp.o:  ftp.h cmds.h
cmds.o: cmds.h


iftp: main.o ftp.o cmds.o
	$(CC) $(CFLAGS) -o iftp main.o ftp.o cmds.o

clean:
	@rm -f *.o $(ALL) *~
