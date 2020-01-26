PROJECTNAME = VMPROTECT
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
SRC_DIR = src
OBJ_DIR = obj
PRSOURCES = $(wildcard $(SRC_DIR)/*.cpp)
PROBJECTS = $(PRSOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
PREXEC = $(PROJECTNAME).exe

.PHONY: all project

all: project
project: $(PREXEC)

$(PREXEC): $(PROBJECTS)
	$(CC) $(PROBJECTS) -o $(PREXEC) $(CFLAGS)

# telling make how to turn file.cpp into file.o for an arbitary file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -c $^ -o $@ $(CFLAGS)

$(OBJ_DIR):
	mkdir $@

clean:
	rm -rf $(OBJ_DIR)
	rm ./*.exe

run:
	./$(PREXEC)