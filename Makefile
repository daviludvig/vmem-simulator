CXX = g++
CXXFLAGS = -Wall -g -Iinclude

SRC = src
BUILD = build

OBJS = $(BUILD)/main.o $(BUILD)/page_table.o $(BUILD)/disk.o $(BUILD)/program.o $(BUILD)/page_replacement.o
TARGET = virtmem  # executável irá para a pasta raiz

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD)/main.o: $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)/main.cpp -o $(BUILD)/main.o

$(BUILD)/page_table.o: $(SRC)/page_table.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)/page_table.cpp -o $(BUILD)/page_table.o

$(BUILD)/page_replacement.o: $(SRC)/page_replacement.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)/page_replacement.cpp -o $(BUILD)/page_replacement.o

$(BUILD)/disk.o: $(SRC)/disk.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)/disk.cpp -o $(BUILD)/disk.o

$(BUILD)/program.o: $(SRC)/program.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)/program.cpp -o $(BUILD)/program.o

clean:
	rm -f $(BUILD)/*.o $(TARGET)
