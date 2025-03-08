# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = inc
LOG_DIR = tmp
COMMON_DIR = Common
VSC_DIR = .vscode

# Compiler
CC = cc
DEBUG_FLAGS = -g  

# Target names
DP2 = dp2
TARGET = $(BIN_DIR)/$(DP2)

# Flags
CLEAN_FLAG = clean
CLEAN_LOG_FLAG = clean_log
ALL_FLAG = all

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c ../Common/src/*.c
	$(CC) -c $< -o $@ $(DEBUG_FLAGS) -I$(INC_DIR) -I../Common/inc

# Link object files into the final executable
$(TARGET): $(OBJ_DIR)/$(DP2).o
	$(CC) $^ -o $@ $(DEBUG_FLAGS)

# Combine client and server compilation
$(ALL_FLAG): $(TARGET)

# Clean object and binary files
$(CLEAN_FLAG):
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* $(VSC_DIR)

# Clean log files
$(CLEAN_LOG_FLAG):
	rm -f $(LOG_DIR)/*.log

# Define target-specific variables
$(ALL_FLAG): CC := $(CC)
$(CLEAN_FLAG): CC := $(CC)
$(CLEAN_LOG_FLAG): CC := $(CC)
