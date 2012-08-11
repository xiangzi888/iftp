ALL= iftp
CC=  gcc
CFLAGS= -g -Wall

all: $(ALL)

main.o: ftp.h 
ftp.o:  ftp.h 


iftp: main.o ftp.o
	$(CC) $(CFLAGS) -o iftp main.o ftp.o

clean:
	@rm -f *.o $(ALL) *~
