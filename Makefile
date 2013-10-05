##############################################################################
#GOOS - Duck-typing for C
#Written in 2012 by Ryan Balsdon ryanbalsdon@gmail.com
#To the extent possible under law, the author(s) have dedicated 
#all copyright and related and neighboring rights to this software 
#to the public domain worldwide. This software is distributed 
#without any warranty.
#You should have received a copy of the CC0 Public Domain Dedication 
#along with this software. If not, see 
#<http://creativecommons.org/publicdomain/zero/1.0/>.
##############################################################################


CFLAGS=-std=gnu99 -c -Wall


all: test

test: goos.o test.o libs.o
	gcc goos.o test.o libs.o -o test

goos.o: goos.c goos.h
	gcc $(CFLAGS) goos.c

libs.o: MutablePointerArray.c
	gcc $(CFLAGS) MutablePointerArray.c -o libs.o

test.o: test.c goos.h
	gcc $(CFLAGS) test.c

clean:
	rm test *.o