target=c s server.o client.o
normal: $(target)
c: client.c
	gcc -g -Wall client.c -o c -lpthread
	gcc -c -Wall client.c -lpthread
s: server.c
	gcc -g -Wall server.c -o s -lpthread
	gcc -c -Wall server.c -lpthread
clean:
	rm  $(target)
