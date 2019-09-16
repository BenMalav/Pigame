EXECUTABLE_NAME=game
CC=g++
EXEC_DIR=./
OBJ_DIR=./obj
SRCS= $(wildcard *.cpp)
OBJS= $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CFLAGS+=-I./include 
CFLAGS+=-I/usr/include/libdrm

CFLAGS+=-std=c++11 -Wall -fpermissive -g
LDFLAGS+=-ldrm -lgbm -lEGL -lGLESv2

all: $(EXEC_DIR)/$(EXECUTABLE_NAME) 

$(EXEC_DIR)/$(EXECUTABLE_NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

.PHONY:	clean
clean:
	rm $(OBJ_DIR)/*.o $(EXEC_DIR)/$(EXECUTABLE_NAME)
