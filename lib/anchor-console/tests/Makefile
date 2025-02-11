TARGET := test
BUILD_DIR_COMMON := build/
BUILD_DIR_HELP := $(BUILD_DIR_COMMON)/help
BUILD_DIR_NO_HELP := $(BUILD_DIR_COMMON)/no_help

C_SOURCES := \
	../src/console.c

C_DEFS :=

CXX_SOURCES_HELP := \
	main.cpp \
	test_console_help.cpp

CXX_SOURCES_NO_HELP := \
	main.cpp \
	test_console_no_help.cpp

CXX_INCLUDES := \
	-I../include

CC := gcc
CXX := g++

OBJECTS_HELP := $(addprefix $(BUILD_DIR_HELP)/,$(notdir $(CXX_SOURCES_HELP:.cpp=.o))) $(addprefix $(BUILD_DIR_HELP)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS_NO_HELP := $(addprefix $(BUILD_DIR_NO_HELP)/,$(notdir $(CXX_SOURCES_NO_HELP:.cpp=.o))) $(addprefix $(BUILD_DIR_NO_HELP)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES_HELP)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES_NO_HELP)))

CFLAGS := $(CXX_INCLUDES) -g3 -Werror $(addprefix -D,$(C_DEFS))
CPP_FLAGS :=
LDFLAGS := -lgtest -lpthread

ifeq ($(OS),Windows_NT)
$(error "Windows is not currently supported")
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
# Likely using clang
CFLAGS += -Wno-extern-c-compat
CPP_FLAGS += -std=c++14
else
CPP_FLAGS += -D_Static_assert=static_assert
endif
endif

$(BUILD_DIR_HELP)/%.o: %.c Makefile | $(BUILD_DIR_HELP)
	@echo "Compiling $(notdir $@)"
	@$(CC) -c $(CFLAGS) -DCONSOLE_HELP_COMMAND=1 -MD -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR_NO_HELP)/%.o: %.c Makefile | $(BUILD_DIR_NO_HELP)
	@echo "Compiling $(notdir $@)"
	@$(CC) -c $(CFLAGS) -DCONSOLE_HELP_COMMAND=0 -MD -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR_HELP)/%.o: %.cpp Makefile | $(BUILD_DIR_HELP)
	@echo "Compiling $(notdir $@)"
	@$(CXX) -c $(CFLAGS) -DCONSOLE_HELP_COMMAND=1 $(CPP_FLAGS) -MD -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR_NO_HELP)/%.o: %.cpp Makefile | $(BUILD_DIR_NO_HELP)
	@echo "Compiling $(notdir $@)"
	@$(CXX) -c $(CFLAGS) -DCONSOLE_HELP_COMMAND=0 $(CPP_FLAGS) -MD -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR_HELP)/$(TARGET): $(OBJECTS_HELP) Makefile | $(BUILD_DIR_HELP)
	@echo "Linking $(notdir $@)"
	@$(CXX) $(OBJECTS_HELP) -o $@ $(LDFLAGS)

$(BUILD_DIR_NO_HELP)/$(TARGET): $(OBJECTS_NO_HELP) Makefile | $(BUILD_DIR_NO_HELP)
	@echo "Linking $(notdir $@)"
	@$(CXX) $(OBJECTS_NO_HELP) -o $@ $(LDFLAGS)

$(BUILD_DIR_HELP):
	@mkdir -p $@

$(BUILD_DIR_NO_HELP):
	@mkdir -p $@

build: $(BUILD_DIR_HELP)/$(TARGET) $(BUILD_DIR_NO_HELP)/$(TARGET)

test: $(BUILD_DIR_HELP)/$(TARGET) $(BUILD_DIR_NO_HELP)/$(TARGET)
	@echo "Running test with help command"
	@$(BUILD_DIR_HELP)/$(TARGET)
	@echo "Running test without help command"
	@$(BUILD_DIR_NO_HELP)/$(TARGET)

clean:
	@echo "Deleting build folder"
	@rm -fR $(BUILD_DIR_COMMON)

-include $(wildcard $(BUILD_DIR_HELP)/*.d)
-include $(wildcard $(BUILD_DIR_NO_HELP)/*.d)
.PHONY: test clean build
.DEFAULT_GOAL := test
