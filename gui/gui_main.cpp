#include <QApplication>
#include "gui_window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Support complet du RTL (Right-to-Left) pour l'arabe
    app.setLayoutDirection(Qt::LeftToRight); // L'interface reste LTR mais les champs seront RTL
    
    // Style moderne
    app.setStyle("Fusion");
    
    // Palette de couleurs professionnelle
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::WindowText, Qt::black);
    darkPalette.setColor(QPalette::Base, QColor(236, 240, 241));
    darkPalette.setColor(QPalette::AlternateBase, QColor(189, 195, 199));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);
    darkPalette.setColor(QPalette::Text, Qt::black);
    darkPalette.setColor(QPalette::Button, QColor(52, 73, 94));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(52, 152, 219));
    darkPalette.setColor(QPalette::Highlight, QColor(26, 188, 156));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);
    
    app.setPalette(darkPalette);
    
    MorphologyGUI window;
    window.show();
    
    return app.exec();
}
