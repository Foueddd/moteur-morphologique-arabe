# ==============================================================================
# Moteur de Recherche Morphologique Arabe
# Makefile - Compilation CLI et GUI (Qt5)
# ==============================================================================

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I./core -I./utils -fPIC

# Flags Qt5 (uniquement pour la cible gui)
QT_CXXFLAGS = $(shell pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui 2>/dev/null)
QT_LDFLAGS  = $(shell pkg-config --libs   Qt5Widgets Qt5Core Qt5Gui 2>/dev/null)

# Dossiers
BUILD_DIR = build

# ==============================================================================
# Cibles principales
# ==============================================================================

all: cli gui

# --- Interface ligne de commande ---
cli:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) cli/main.cpp -o $(BUILD_DIR)/morphology_engine
	@echo "✓ CLI compilé : $(BUILD_DIR)/morphology_engine"

# --- Interface graphique Qt5 ---
gui: $(BUILD_DIR)/gui_window.o $(BUILD_DIR)/gui_main.o $(BUILD_DIR)/moc_gui_window.o
	$(CXX) $(CXXFLAGS) $^ $(QT_LDFLAGS) -o $(BUILD_DIR)/morphology_gui
	@echo "✓ GUI compilé : $(BUILD_DIR)/morphology_gui"

$(BUILD_DIR)/gui_window.o: gui/gui_window.cpp gui/gui_window.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c gui/gui_window.cpp -o $@

$(BUILD_DIR)/gui_main.o: gui/gui_main.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c gui/gui_main.cpp -o $@

$(BUILD_DIR)/moc_gui_window.cpp: gui/gui_window.h
	@mkdir -p $(BUILD_DIR)
	moc gui/gui_window.h -o $@

$(BUILD_DIR)/moc_gui_window.o: $(BUILD_DIR)/moc_gui_window.cpp
	$(CXX) $(CXXFLAGS) $(QT_CXXFLAGS) -c $< -o $@

# ==============================================================================
# Utilitaires
# ==============================================================================

clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Nettoyage terminé"

run-cli: cli
	./$(BUILD_DIR)/morphology_engine

run-gui: gui
	./$(BUILD_DIR)/morphology_gui

.PHONY: all cli gui clean run-cli run-gui
