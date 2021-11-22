
cliente: cliente.o wrappers.o
	gcc -o cliente cliente.o wrappers.o -lpthread

servidor: servidor.o wrappers.o
	gcc -o servidor servidor.o wrappers.o strmap.c -lpthread

wrappers.o: wrappers.c wrappers.h
	gcc -o wrappers.o wrappers.c -c -W -Wall

cliente.o: cliente.c wrappers.h
	gcc -o cliente.o cliente.c -c -Wall -W

servidor.o: servidor.c wrappers.h
	gcc -o servidor.o servidor.c -c -Wall -W

clean:
	rm -rf *.o *~ servidor *~ cliente

