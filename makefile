# Directories
SRC_DIR = src
HDR_DIR = headers
OBJ_DIR = obj

# Compiler
CC = gcc

# Source Files
SRC_FILES = *.c

# Object Files
OBJ_FILES = *.o

# Compiler Flags
CFLAGS = -Wall -Wextra -Werror -pedantic -g

CLIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Compiler Output
COUT = Pong

all: app

all: move_objects
	${CC} ${CFLAGS} ${OBJ_DIR}/${OBJ_FILES} -o ${COUT} ${CLIBS}

objects: ${SRC_DIR}/${SRC_FILES}
	mkdir -p ${OBJ_DIR}; ${CC} ${CFLAGS} -c ${SRC_DIR}/${SRC_FILES} ${CLIBS}

move_objects: objects
	mv ${OBJ_FILES} ${OBJ_DIR}/

test: ${COUT}
	./${COUT}

clean:
	rm -rf ${COUT} ${OBJ_DIR} ${OBJ_FILES}

.PHONY: app objects move_objects test clean
