# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = inc
LOG_DIR = tmp
DP1_DIR = DP-1
DP2_DIR = DP-2
DC_DIR	= DC
COMMON_DIR = Common
VSC_DIR = .vscode

# Compiler
CC = cc
DEBUG_FLAGS = -g     

# Targets
DP1_TARGET := $(DP1_DIR)/$(BIN_DIR)/dp1
DP2_TARGET := $(DP2_DIR)/$(BIN_DIR)/dp2
DC_TARGET  := $(DC_DIR)/$(BIN_DIR)/dc

# Flags
CLEAN_FLAG      = clean
CLEAN_LOG_FLAG  = clean_log
ALL_FLAG        = all

# Compile the source files
$(DP1_DIR)/$(OBJ_DIR)/%.o: $(DP1_DIR)/$(SRC_DIR)/dp1.c $(DP1_DIR)/$(INC_DIR)/dp1.h $(COMMON_DIR)/$(SRC_DIR)/*.c
	$(CC) -c $< -o $@ $(DEBUG_FLAGS)   

$(DP2_DIR)/$(OBJ_DIR)/%.o: $(DP2_DIR)/$(SRC_DIR)/dp2.c $(DP2_DIR)/$(INC_DIR)/dp2.h $(COMMON_DIR)/$(SRC_DIR)/*.c
	$(CC) -c $< -o $@ $(DEBUG_FLAGS)   

$(DC_DIR)/$(OBJ_DIR)/%.o: $(DC_DIR)/$(SRC_DIR)/dc.c $(DC_DIR)/$(INC_DIR)/dc.h $(COMMON_DIR)/$(SRC_DIR)/*.c
	$(CC) -c $< -o $@ $(DEBUG_FLAGS)   

# Create the DP-1 executable
$(DP1_TARGET): $(DP1_DIR)/$(OBJ_DIR)/dp1.o
	$(CC) $< -o $@ $(PTHREAD) $(DEBUG_FLAGS)   

# Create the DP-2 executable
$(DP2_TARGET): $(DP2_DIR)/$(OBJ_DIR)/dp2.o
	$(CC) $< -o $@ $(DEBUG_FLAGS)  

# Create the DC executable    
$(DC_TARGET): $(DC_DIR)/$(OBJ_DIR)/dc.o
	$(CC) $< -o $@ $(DEBUG_FLAGS) 

# Combine client and server compilation
$(ALL_FLAG): $(DP1_TARGET) $(DP2_TARGET) $(DC_TARGET)

# Clean object and binary files
$(CLEAN_FLAG):
	rm -rf $(DP1_DIR)/$(OBJ_DIR)/*.o $(DP1_DIR)/$(BIN_DIR)/* $(DP1_DIR)/$(VSC_DIR)
	rm -rf $(DP2_DIR)/$(OBJ_DIR)/*.o $(DP2_DIR)/$(BIN_DIR)/* $(DP2_DIR)/$(VSC_DIR)
	rm -rf $(DC_DIR)/$(OBJ_DIR)/*.o $(DC_DIR)/$(BIN_DIR)/* $(DC_DIR)/$(VSC_DIR)
	rm -rf $(VSC_DIR)
# Clean log files
$(CLEAN_LOG_FLAG):
	rm -f $(DP1_DIR)/$(LOG_DIR)/*.log
	rm -f $(DP2_DIR)/$(LOG_DIR)/*.log	
	rm -f $(DC_DIR)/$(LOG_DIR)/*.log
		
# Define target-specific variables
$(ALL_FLAG): CC   		:= $(CC)
$(CLEAN_FLAG): CC     	:= $(CC)
$(CLEAN_LOG_FLAG): CC	:= $(CC)


