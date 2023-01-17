NAME = serv
SRCS = main.cpp Socket.cpp EventQueue.cpp Client.cpp
OBJS = $(SRCS:.cpp=.o)
FLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic
CC = c++
INCLUDES = Socket.hpp EventQueue.hpp

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(FLAGS)

%.o: %.cpp %.tpp $(INCLUDES)
	$(CC) -o $@ -c $^ $(FLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

exe: all
	./$(NAME)

re: fclean all