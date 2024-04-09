# Compiler and flags
CXX := c++
CXXFLAGS := -std=c++98 -Wall -Wextra -Werror -fsanitize=address -g

# Directories
SRCDIR := srcs
INCDIR := incs
OBJDIR := objs

# Files
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Targets
TARGET := ircserv

# Build rules
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "building..."
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

all: $(TARGET)
$(TARGET): $(OBJS)
	@echo "Removing files..."
	@$(CXX) $(CXXFLAGS) $^ -o $@


clean:
	@rm -rf $(OBJDIR)
	@echo "$(OBJDIR) folder removed"

fclean: clean
	@rm -rf $(TARGET)
	@echo "$(TARGET) removed"

re: fclean all

valgrind: re
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./ircserv 1025 pw

run: re
	./ircserv 1025 pw

.PHONY: clean