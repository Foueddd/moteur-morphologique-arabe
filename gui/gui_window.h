#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QTreeWidget>
#include <QGroupBox>
#include <QSplitter>

class MorphologyGUI : public QMainWindow {
    Q_OBJECT

public:
    explicit MorphologyGUI(QWidget *parent = nullptr);
    ~MorphologyGUI();

private slots:
    // Navigation
    void showPage(int index);
    
    // Page 1: Racines
    void onAddRoot();
    void onDeleteRoot();
    void onSearchRoot();
    void onShowAllRoots();
    
    // Page 2: Schèmes
    void onAddPattern();
    void onModifyPattern();
    void onDeletePattern();
    void onShowAllPatterns();
    
    // Page 3: Génération
    void onGenerateWord();
    
    // Page 4: Validation
    void onValidateWord();
    
    // Page 5: Explorer famille
    void onExploreFamilyForRoot();
    
    // Page 6: Toutes racines et dérivés
    void onShowAllRootsAndDerivatives();
    
    // Page 7: Solutions Bonus
    void onAnalyzeDefectiveVerb();
    void onShowVerbExamples();
    
    // Utilitaires
    void onClearLogs();

private:
    void setupUI();
    void createMenuBar();
    void createSideMenu();
    void createPages();
    void logMessage(const QString &message);
    void loadPatternsToComboBox();
    void setupArabicInput(QLineEdit *lineEdit);  // Helper pour configurer RTL
    QString cleanArabicText(const QString &text);  // Nettoyer les marqueurs Unicode
    
    // Layout principal
    QWidget *centralWidget;
    QSplitter *mainSplitter;
    
    // Menu latéral
    QWidget *sideMenuWidget;
    QVBoxLayout *sideMenuLayout;
    QPushButton *btnPageRoots;
    QPushButton *btnPagePatterns;
    QPushButton *btnPageGenerate;
    QPushButton *btnPageValidate;
    QPushButton *btnPageExplore;
    QPushButton *btnPageAll;
    QPushButton *btnPageBonus;
    
    // Zone centrale
    QStackedWidget *stackedWidget;
    
    // Zone de logs
    QTextEdit *logArea;
    QPushButton *btnClearLogs;
    
    // ========== PAGE 1: Racines ==========
    QWidget *pageRoots;
    QLineEdit *rootInput;
    QPushButton *btnAddRoot;
    QPushButton *btnDeleteRoot;
    QPushButton *btnSearchRoot;
    QPushButton *btnShowAllRoots;
    QTextEdit *rootsDisplay;
    
    // ========== PAGE 2: Schèmes ==========
    QWidget *pagePatterns;
    QLineEdit *patternNameInput;
    QLineEdit *patternRepInput;
    QPushButton *btnAddPattern;
    QPushButton *btnModifyPattern;
    QPushButton *btnDeletePattern;
    QPushButton *btnShowAllPatterns;
    QTextEdit *patternsDisplay;
    
    // ========== PAGE 3: Génération ==========
    QWidget *pageGenerate;
    QLineEdit *genRootInput;
    QComboBox *genPatternCombo;
    QPushButton *btnGenerate;
    QLabel *genResultLabel;
    
    // ========== PAGE 4: Validation ==========
    QWidget *pageValidate;
    QLineEdit *valWordInput;
    QLineEdit *valRootInput;
    QPushButton *btnValidate;
    QLabel *valResultLabel;
    
    // ========== PAGE 5: Explorer famille ==========
    QWidget *pageExplore;
    QLineEdit *exploreRootInput;
    QPushButton *btnExploreFamily;
    QListWidget *familyList;
    
    // ========== PAGE 6: Tout afficher ==========
    QWidget *pageAll;
    QPushButton *btnRefreshAll;
    QTreeWidget *allTreeWidget;
    
    // ========== PAGE 7: Solutions Bonus ==========
    QWidget *pageBonus;
    QLineEdit *bonusVerbInput;
    QPushButton *btnAnalyzeVerb;
    QPushButton *btnShowExamples;
    QTextEdit *bonusResultDisplay;
    QLabel *bonusTypeLabel;
};

#endif
