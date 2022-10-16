CC = gcc
CFLAGS=-O2 -Wall -g
LDLIBS = -lpthread

all: multiclient stockclient stockserver

multiclient: multiclient.c csapp.c csapp.h
stockclient: stockclient.c csapp.c csapp.h
stockserver: stockserver.c stockserver.h echo.c csapp.c csapp.h micro_httpd.c micro_httpd.h

clean:
	rm -rf *~ multiclient stockclient stockserver *.o
