#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>

#include "morphology_engine.h"
#include "utils.h"

class MainWindow : public QMainWindow {
public:
    MainWindow();

private:
    MorphologyEngine engine;

    QWidget* createHomeTab();
    QWidget* createRootsTab();
    QWidget* createPatternsTab();
    QWidget* createGenerationTab();
    QWidget* createValidationTab();
    QWidget* createFamilyTab();
    QWidget* createAdvancedTab();
    QWidget* createAllTab();

    void initDefaultPatterns();
    void loadRootsFromFile();

    static void setOutputText(QTextEdit* output, const QString& text);
};

#endif // GUI_WINDOW_H
