##
## EPITECH PROJECT, 2026
## G-OOP-400-MAR-4-1-arcade-10
## File description:
## Makefile
##

NAME    := raytracer
NAME_SFML := sfml_window
CXX     := g++
CXXFLAGS := -Wall -Wextra -Werror -std=c++17 -MMD -MP
LDFLAGS := -lconfig++ -lsfml-graphics -lsfml-window -lsfml-system
LDFLAGS_SFML := -lconfig++ -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR := .
OBJ_DIR := obj

# Sources for raytracer
SRCS    := $(shell find $(SRC_DIR) -name "*.cpp" -not -path "*/obj/*" -not -name "sfml_window.cpp")
OBJS    := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Sources for sfml_window
SRCS_SFML := $(shell find $(SRC_DIR) -name "*.cpp" -not -path "*/obj/*" -not -name "main.cpp")
OBJS_SFML := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS_SFML))

DEPS    := $(OBJS:.o=.d) $(OBJS_SFML:.o=.d)

all: $(NAME) $(NAME_SFML)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

$(NAME_SFML): $(OBJS_SFML)
	$(CXX) $(CXXFLAGS) -o $(NAME_SFML) $(OBJS_SFML) $(LDFLAGS_SFML)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(NAME)
	./$(NAME) scenes/demo_jolie.cfg > output.ppm

run_sfml: $(NAME_SFML)
	./$(NAME_SFML) scenes/demo_all.cfg

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(NAME_SFML) output.ppm

re: fclean all

-include $(DEPS)

.PHONY: all run run_sfml clean fclean re