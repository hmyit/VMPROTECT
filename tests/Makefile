CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -g
SOURCE = ./test01.cpp ../src/vmcpu.cpp
EXEC = vmtest01.exe

$(EXEC): $(SOURCE)
	$(CC) $(SOURCE) -o $(EXEC) $(CFLAGS)

clean:
	rm ./*.exe

run:
	./$(EXEC)