CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./core -I./gui -I./cli -I./utils -fPIC
QT_CXXFLAGS = $(shell pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui)
QT_LDFLAGS = $(shell pkg-config --libs Qt5Widgets Qt5Core Qt5Gui)

# Dossiers
BUILD_DIR = build
SRC_CORE = core
SRC_GUI = gui
SRC_CLI = cli
SRC_UTILS = utils

all: cli gui

cli:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_CLI)/main.cpp -o $(BUILD_DIR)/morphology_engine
	@echo "✓ CLI compilé: $(BUILD_DIR)/morphology_engine"

gui: $(BUILD_DIR)/gui_window.o $(BUILD_DIR)/gui_main.o $(BUILD_DIR)/moc_gui_window.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(BUILD_DIR)/gui_window.o $(BUILD_DIR)/gui_main.o $(BUILD_DIR)/moc_gui_window.o $(QT_LDFLAGS) -o $(BUILD_DIR)/morphology_gui
	@echo "✓ GUI compilé: $(BUILD_DIR)/morphology_gui"

$(BUILD_DIR)/gui_window.o: $(SRC_GUI)/gui_window.cpp $(SRC_GUI)/gui_window.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $(SRC_GUI)/gui_window.cpp -o $(BUILD_DIR)/gui_window.o

$(BUILD_DIR)/gui_main.o: $(SRC_GUI)/gui_main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $(SRC_GUI)/gui_main.cpp -o $(BUILD_DIR)/gui_main.o

$(BUILD_DIR)/moc_gui_window.cpp: $(SRC_GUI)/gui_window.h
	@mkdir -p $(BUILD_DIR)
	moc $(SRC_GUI)/gui_window.h -o $(BUILD_DIR)/moc_gui_window.cpp

$(BUILD_DIR)/moc_gui_window.o: $(BUILD_DIR)/moc_gui_window.cpp
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $(BUILD_DIR)/moc_gui_window.cpp -o $(BUILD_DIR)/moc_gui_window.o

clean:
	rm -f $(BUILD_DIR)/morphology_engine $(BUILD_DIR)/morphology_gui $(BUILD_DIR)/*.o $(BUILD_DIR)/moc_*.cpp
	@echo "✓ Nettoyage terminé"

run-cli: cli
	./$(BUILD_DIR)/morphology_engine

run-gui: gui
	./$(BUILD_DIR)/morphology_gui

.PHONY: all cli gui clean run-cli run-gui
