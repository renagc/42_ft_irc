# Compiler and flags
CXX := c++
CXXFLAGS := -std=c++98 -Wall -Wextra -Werror

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

.PHONY: clean