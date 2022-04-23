.PHONY : server
server: 
	gcc -c -w shared.c -o shared.o
	gcc -c -w server-socket.c -o server-socket.o -lpthread
	gcc -c -w messages.c -o messages.o
	gcc -c -w players.c -o players.o
	gcc -w shared.o messages.o server-socket.o players.o -lpthread -lm -o server
	make clean
	
.PHONY : cmdclient	
cmdclient:
	gcc -c -w messages.c -o messages.o
	gcc -c -w client.c -o client.o -lpthread
	gcc -w client.o messages.o -lpthread -o cmdclient
	make clean
.PHONY : clean
clean :
	rm *.o
