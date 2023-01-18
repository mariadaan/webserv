NAME = test

CC = c++ -std=c++17
CFLAGS = -I $(shell brew --prefix criterion)/include
LIB_FLAGS = -lcriterion

TEST_SOURCES = $(shell find . -name '*.cpp')
PROGRAM_SOURCES = $(shell find ../src -name '*.cpp' -not -name 'main.cpp')
TEST_HEADERS = $(shell find . -name '*.hpp') $(shell find . -name '*.tpp')
PROGRAM_HEADERS = $(shell find ../src -name '*.hpp') $(shell find ../src -name '*.tpp')

OBJ_DIR = obj

OBJS = $(addprefix $(OBJ_DIR)/testing/, $(TEST_SOURCES:.cpp=.o)) $(addprefix $(OBJ_DIR)/program/, $(subst ../,,$(PROGRAM_SOURCES:.cpp=.o)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(LIB_FLAGS) -o $(NAME) $(OBJS)

$(OBJ_DIR)/testing/%.o: %.cpp $(TEST_HEADERS) $(PROGRAM_HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/program/%.o: ../%.cpp $(PROGRAM_HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(NAME) $(OBJ_DIR)

re: clean all

exe: all
	./test

# $(brew --prefix criterion)/include