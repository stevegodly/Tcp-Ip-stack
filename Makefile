CC = gcc
CFLAGS = -g
TARGET = test.exe
LIBS = -lpthread -L ./CommandParser -lcli

OBJS = gluethread/glthread.o \
    graph.o \
    topologies.o \
	net.o \
	comms.o \
	Layer2/layer2.o \
	nwcli.o \
	utils.o \
	CommandParser/libcli.a

$(TARGET): testapp.o $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) testapp.o -o $(TARGET) $(LIBS)

testapp.o: testapp.c 
	$(CC) $(CFLAGS) -c testapp.c -o testapp.o

glthread.o: gluethread/glthread.c
	$(CC) $(CFLAGS) -c -I gluethread gluethread/glthread.c -o gluethread/glthread.o

graph.o: graph.c
	$(CC) $(CFLAGS) -c -I . graph.c -o graph.o

topologies.o: topologies.c
	$(CC) $(CFLAGS) -c -I . topologies.c -o topologies.o

net.o: net.c
	$(CC) $(CFLAGS) -c -I . net.c -o net.o	

utils.o: utils.c
	$(CC) $(CFLAGS) -c -I . utils.c -o utils.o

nwcli.o: nwcli.c
	$(CC) $(CFLAGS) -c -I . nwcli.c -o nwcli.o

comms.o: comms.c
	$(CC) $(CFLAGS) -c -I . comms.c -o comms.o

Layer2/layer2.o:Layer2/layer2.c
	${CC} ${CFLAGS} -c -I Layer2 Layer2/layer2.c -o Layer2/layer2.o

CommandParser/libcli.a:
	cd CommandParser && make

clean:
	rm *.o	
	rm gluethread/*.o
	rm Layer2/*.o
	rm *.exe

cleanall:
	make clean
	cd CommandParser && make clean