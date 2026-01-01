CC = g++
CFLAGS = -Wall -std=c++17 -I./include -I./include/core -I./include/vehicles -I./include/infrastructure -I./include/simulation -I/usr/local/include -I/usr/include
LDFLAGS = -L/usr/local/lib -lraylib -lm -lpthread -ldl
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
SRCS = $(wildcard $(SRC_DIR)/**/*.cpp $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/smart_city_emergency
all: directories $(TARGET)
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(BUILD_DIR)/core $(BUILD_DIR)/vehicles $(BUILD_DIR)/infrastructure $(BUILD_DIR)/simulation
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
run: all
	./$(TARGET)
debug: CFLAGS += -g -DDEBUG
debug: all
.PHONY: all clean run debug directories
