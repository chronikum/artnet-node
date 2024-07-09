CC = c++

NAME = dmx-node

CFLAGS = -Werror -Wall -Wextra

OBJS := $(*.o)

ALL_FILES = $(wildcard *.cpp)

all: ${NAME}

${NAME}:
	${CC} ${CFLAGS} ${ALL_FILES} -o ${NAME}

dev:
	${CC} ${ALL_FILES} -o ${NAME}

clean:
	rm -rf *.o

fclean: clean
	rm -f ${OBJS} ${NAME}

re: fclean ${NAME}