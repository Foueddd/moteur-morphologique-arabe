# ============================================================================
# MAKEFILE - Moteur de Recherche Morphologique Arabe
# ============================================================================
# Compiler : g++ avec support C++11

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
SRCDIR = src
BINDIR = bin
TARGET = $(BINDIR)/morphology_engine
GUI_TARGET = $(BINDIR)/morphology_gui

QT_CFLAGS = $(shell pkg-config --cflags Qt5Widgets 2>/dev/null)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets 2>/dev/null)

# Fichiers sources
SOURCES = $(SRCDIR)/main.cpp
HEADERS = $(SRCDIR)/structs.h $(SRCDIR)/avl_tree.h $(SRCDIR)/hash_table.h \
          $(SRCDIR)/morphology_engine.h $(SRCDIR)/utils.h

GUI_SOURCES = $(SRCDIR)/gui_main.cpp $(SRCDIR)/gui_window.cpp
GUI_HEADERS = $(SRCDIR)/gui_window.h

# Fichiers objets
OBJECTS = $(SOURCES:.cpp=.o)
GUI_OBJECTS = $(GUI_SOURCES:.cpp=.o)

# ============================================================================
# RÈGLES DE COMPILATION
# ============================================================================

all: $(TARGET)

# Créer le répertoire bin s'il n'existe pas
$(BINDIR):
	@mkdir -p $(BINDIR)

# Compilation de main.cpp
$(SRCDIR)/main.o: $(SRCDIR)/main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(SRCDIR)/gui_main.o: $(SRCDIR)/gui_main.cpp $(HEADERS) $(GUI_HEADERS)
	$(CXX) $(CXXFLAGS) -fPIC $(QT_CFLAGS) -c $< -o $@

$(SRCDIR)/gui_window.o: $(SRCDIR)/gui_window.cpp $(HEADERS) $(GUI_HEADERS)
	$(CXX) $(CXXFLAGS) -fPIC $(QT_CFLAGS) -c $< -o $@

# Édition des liens (créer l'exécutable)
$(TARGET): $(BINDIR) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@echo "✓ Compilation réussie!"
	@echo "  Exécutable: $(TARGET)"
	@echo "  Pour lancer: ./$(TARGET)"

gui: $(BINDIR) $(GUI_OBJECTS)
	$(CXX) $(CXXFLAGS) $(GUI_OBJECTS) -o $(GUI_TARGET) $(QT_LIBS)
	@echo "✓ GUI compilée!"
	@echo "  Exécutable: $(GUI_TARGET)"
	@echo "  Pour lancer: ./$(GUI_TARGET)"

# Nettoyage des fichiers objets
clean:
	@rm -f $(SRCDIR)/*.o
	@echo "✓ Fichiers objets supprimés."

# Nettoyage complet
distclean: clean
	@rm -rf $(BINDIR)
	@echo "✓ Répertoire bin supprimé."

# Compilation et lancement
run: $(TARGET)
	./$(TARGET)

# Compilation en mode debug
debug: CXXFLAGS += -g -DDEBUG
debug: clean $(TARGET)
	@echo "✓ Compilation en mode DEBUG terminée."

# Affiche l'aide
help:
	@echo "========================="
	@echo "Cibles disponibles:"
	@echo "========================="
	@echo "make           - Compiler le projet"
	@echo "make gui       - Compiler l'interface graphique Qt"
	@echo "make run       - Compiler et lancer l'application"
	@echo "make clean     - Supprimer les fichiers objets"
	@echo "make distclean - Supprimer tout"
	@echo "make debug     - Compiler en mode debug"
	@echo "make help      - Afficher cette aide"

.PHONY: all gui clean distclean run debug help
