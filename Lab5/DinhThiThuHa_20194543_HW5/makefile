# .PHONY: clear server client

all: clear server client

server: server.c
	gcc -o server server.c

client: client.c
	gcc -o client client.c

clear: 
	rm -f server client

# runMain: clearMain main
# main: main.c
# 	gcc -o main main.c
# clearMain:
# 	rm -f main