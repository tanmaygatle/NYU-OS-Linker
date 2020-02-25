CC=g++

linker: linker.cpp 
	$(CC) -o linker linker.cpp

clean:
	rm -f linker
