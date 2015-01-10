# This Makefile is only for clent-codes/

all:
#	gcc 	-Wall  	-c client-main.c  client-main.h client-state.h
#	gcc 	-Wall 	-c client-main.h client-state.h client-parse.h client-mystring.h client-state.c 
#	gcc 	-Wall 	-c client-state.c client-main.c
#	gcc	-Wall	-pthread client-state.o client-main.o 	-o client
#	gcc	-Wall 	temp-server-main.c -pthread	-o server
	gcc 	-c client-state.c client-main.c
	gcc	-pthread client-state.o client-main.o 	-o client
	gcc 	temp-server-main.c -pthread	-o server
run-s:
	./server
run-c:
	./client
clean:
	rm -f *.h.gch
	rm -f *.o
	rm -f client
	rm -f server
