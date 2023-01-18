# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: mvan-wij <mvan-wij@student.codam.nl>         +#+                      #
#                                                    +#+                       #
#    Created: 2023/01/12 14:40:00 by mvan-wij      #+#    #+#                  #
#    Updated: 2023/01/18 17:16:52 by mdaan         ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

################################################################################

PROJECT		:= webserv
NAME		:= webserv
NAME_BONUS	:= webserv

CC			:= c++
CFLAGS		+= -Wall -Wextra -std=c++98 -pedantic -Ofast $(if $(DEBUG), -g3) $(if $(SANITIZE), -fsanitize=address -g3)

################################################################################

HEADERS += $(shell find src -name '*.[ht]pp')

SOURCES += $(shell find src -name '*.cpp')

################################################################################

ifdef BONUS
NAME	:= $(NAME_BONUS)
HEADERS +=
SOURCES	+=
CFLAGS	+= -DBONUS
endif

ifdef DEBUG
CFLAGS	+= -g3 -DDEBUG
endif

################################################################################

HEADERS := $(sort $(HEADERS))

OBJDIR := obj
OBJECTS := $(addprefix $(OBJDIR)/, $(SOURCES:cpp=o))
INCLUDE_FLAGS += $(addprefix -I, $(sort $(dir $(HEADERS))))

export CFLAGS := $(sort $(CFLAGS))

DATA_FILE := .make_data.txt
MAKE_DATA := $(shell uname) $(CFLAGS) $(INCLUDE_FLAGS) $(sort $(OBJECTS))
ifneq ($(shell echo $(MAKE_DATA)), $(shell cat "$(DATA_FILE)" 2> /dev/null))
PRE_RULES := clean
endif

################################################################################

BLUE_FG := \e[34m
RESET_COLOR := \e[0m
GREEN_FG := \e[32m
CYAN_FG := \e[36m
CLEAR_REST_OF_LINE := \e[K
print_prefix = printf "$(CYAN_FG)%-$(PROJECT_SPACING)s$(RESET_COLOR) $(GREEN_FG)%-$(RULE_SPACING)s$(RESET_COLOR) : " "[$(1)]" "$(2)"
exec_no_nl = printf "$(1)$(CLEAR_REST_OF_LINE)\n"; $(1); printf "\e[A"

################################################################################

all: $(PRE_RULES) $(NAME)

$(NAME): $(OBJECTS)
	@$(call print_prefix,"$(PROJECT)","make")
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)
	@$(call print_prefix,"$(PROJECT)","make")
	@printf "$(BLUE_FG)$(NAME)$(RESET_COLOR) created\n"
	@echo $(MAKE_DATA) > $(DATA_FILE)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@$(call print_prefix,"$(PROJECT)","make")
	@$(call exec_no_nl,$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@)

debug:
	@$(MAKE) DEBUG=1

clean:
	@$(call print_prefix,"$(PROJECT)","$@")
	rm -rf $(OBJDIR)

fclean: clean
	@$(call print_prefix,"$(PROJECT)","$@")
	rm -f $(NAME)
	@$(call print_prefix,"$(PROJECT)","$@")
	rm -f $(DATA_FILE)

re: fclean all

run: $(NAME)
	@./$(NAME) $(ARGS)

test:
	@$(MAKE) -C testing exe

.PHONY: all debug clean fclean re run test

################################################################################
