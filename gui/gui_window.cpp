#include "gui_window.h"
#include "bst_tree.h"
#include "hash_table.h"
#include "utils.h"
#include "morphology_engine.h"
#include "structs.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QTime>
#include <fstream>
#include <vector>
#include <sstream>

// Moteur global
static MorphologyEngine* globalEngine = nullptr;

MorphologyGUI::MorphologyGUI(QWidget *parent) : QMainWindow(parent) {
    // Initialiser le moteur
    if (!globalEngine) {
        globalEngine = new MorphologyEngine();
        
        // Ajouter les schèmes de base (IMPORTANT!)
        Pattern p1("فاعل", "VCCCVC", "Participe actif - Agent (celui qui fait)");
        Pattern p2("مفعول", "CVCCVC", "Participe passif - Patient (celui qui subit)");
        Pattern p3("افتعل", "VCVCCVC", "Forme VIII - Réflexive");
        Pattern p4("تفعيل", "VCVCCVC", "Forme II - Causatif");
        Pattern p5("مفعال", "CVCCVC", "Forme intensive");
        Pattern p6("فعال", "CVCVC", "Pluriel ou adjectif");
        
        globalEngine->addPattern(p1);
        globalEngine->addPattern(p2);
        globalEngine->addPattern(p3);
        globalEngine->addPattern(p4);
        globalEngine->addPattern(p5);
        globalEngine->addPattern(p6);
        
        // Charger les racines
        std::vector<std::string> roots = Utils::loadRootsFromFile("data/roots.txt");
        for (const auto& r : roots) {
            if (Utils::isValidArabicRoot(r)) {
                globalEngine->addRoot(r);
            }
        }
    }
    
    setupUI();
    setWindowTitle("المحرك الصرفي العربي - Moteur Morphologique Arabe");
    resize(1200, 800);
    
    // Afficher la première page
    showPage(0);
}

// Méthode helper pour configurer correctement les champs de saisie arabe en RTL
void MorphologyGUI::setupArabicInput(QLineEdit *lineEdit) {
    if (!lineEdit) return;
    
    // Forcer la direction RTL au niveau du widget
    lineEdit->setLayoutDirection(Qt::RightToLeft);
    
    // Aligner le texte à droite
    lineEdit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    // Activer le support des méthodes d'entrée
    lineEdit->setAttribute(Qt::WA_InputMethodEnabled);
    lineEdit->setInputMethodHints(Qt::ImhNoPredictiveText);
    
    // Intercepter la saisie pour ajouter le marqueur RLE (Right-to-Left Embedding)
    connect(lineEdit, &QLineEdit::textChanged, [lineEdit](const QString &text) {
        // Vérifier si le texte commence déjà par le marqueur RLE
        const QChar RLE(0x202B);  // Right-to-Left Embedding
        const QChar PDF(0x202C);  // Pop Directional Formatting
        
        if (!text.isEmpty() && text[0] != RLE) {
            // Bloquer temporairement les signaux pour éviter la récursion
            lineEdit->blockSignals(true);
            
            // Ajouter le marqueur RLE au début du texte
            QString correctedText = RLE + text + PDF;
            int cursorPos = lineEdit->cursorPosition();
            lineEdit->setText(correctedText);
            
            // Repositionner le curseur (ajuster pour le caractère RLE ajouté)
            lineEdit->setCursorPosition(cursorPos + 1);
            
            // Réactiver les signaux
            lineEdit->blockSignals(false);
        }
    });
}

// Fonction pour nettoyer le texte des marqueurs Unicode invisibles
QString MorphologyGUI::cleanArabicText(const QString &text) {
    QString cleaned = text;
    // Retirer les marqueurs de contrôle bidirectionnel
    cleaned.remove(QChar(0x202A)); // LRE (Left-to-Right Embedding)
    cleaned.remove(QChar(0x202B)); // RLE (Right-to-Left Embedding)
    cleaned.remove(QChar(0x202C)); // PDF (Pop Directional Formatting)
    cleaned.remove(QChar(0x202D)); // LRO (Left-to-Right Override)
    cleaned.remove(QChar(0x202E)); // RLO (Right-to-Left Override)
    cleaned.remove(QChar(0x200E)); // LRM (Left-to-Right Mark)
    cleaned.remove(QChar(0x200F)); // RLM (Right-to-Left Mark)
    return cleaned.trimmed();
}

MorphologyGUI::~MorphologyGUI() {
    // Ne pas supprimer globalEngine ici (global singleton)
}

void MorphologyGUI::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Créer le menu latéral
    createSideMenu();
    mainLayout->addWidget(sideMenuWidget);
    
    // Splitter pour la zone centrale et les logs
    mainSplitter = new QSplitter(Qt::Vertical);
    
    // Zone centrale avec pages
    stackedWidget = new QStackedWidget();
    stackedWidget->setStyleSheet("background: white;");
    createPages();
    mainSplitter->addWidget(stackedWidget);
    
    // Zone de logs
    QWidget *logWidget = new QWidget();
    QVBoxLayout *logLayout = new QVBoxLayout(logWidget);
    logLayout->setContentsMargins(10, 5, 10, 10);
    
    QLabel *logTitle = new QLabel("» سجل العمليات (Journal)");
    QFont logFont;
    logFont.setBold(true);
    logFont.setPointSize(10);
    logTitle->setFont(logFont);
    logLayout->addWidget(logTitle);
    
    logArea = new QTextEdit();
    logArea->setReadOnly(true);
    logArea->setMaximumHeight(150);
    logArea->setStyleSheet(
        "background: #2c3e50; color: #ecf0f1; "
        "border: none; padding: 8px; "
        "font-family: 'Courier New', monospace; font-size: 10pt;"
    );
    logLayout->addWidget(logArea);
    
    btnClearLogs = new QPushButton("✕ مسح السجل");
    btnClearLogs->setStyleSheet(
        "background: #95a5a6; color: white; "
        "padding: 6px; font-weight: bold; border-radius: 3px;"
    );
    connect(btnClearLogs, &QPushButton::clicked, this, &MorphologyGUI::onClearLogs);
    logLayout->addWidget(btnClearLogs);
    
    mainSplitter->addWidget(logWidget);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(mainSplitter);
    
    // Message initial
    logMessage("[OK] Système initialisé - Moteur Morphologique Arabe");
    logMessage("[OK] Structure: ABR (racines) + Hash Table (schèmes) + Listes chaînées (dérivés)");
    logMessage("[OK] " + QString::number(globalEngine->getRootCount()) + " racines chargées depuis data/roots.txt");
    logMessage("[OK] 6 patterns morphologiques disponibles");
}

void MorphologyGUI::createSideMenu() {
    sideMenuWidget = new QWidget();
    sideMenuWidget->setFixedWidth(250);
    sideMenuWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #34495e, stop:1 #2c3e50); "
        "border-right: 2px solid #1abc9c;"
    );
    
    sideMenuLayout = new QVBoxLayout(sideMenuWidget);
    sideMenuLayout->setSpacing(5);
    sideMenuLayout->setContentsMargins(10, 15, 10, 15);
    
    // Titre du menu
    QLabel *menuTitle = new QLabel("■ القائمة الرئيسية");
    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    menuTitle->setFont(titleFont);
    menuTitle->setStyleSheet("color: white; padding: 15px; text-align: center;");
    menuTitle->setAlignment(Qt::AlignCenter);
    sideMenuLayout->addWidget(menuTitle);
    
    sideMenuLayout->addSpacing(10);
    
    // Style des boutons de menu
    QString btnStyle = 
        "QPushButton {"
        "  background: #34495e; color: white; "
        "  padding: 15px; text-align: left; "
        "  border: none; border-radius: 5px; "
        "  font-size: 11pt; font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: #1abc9c;"
        "}"
        "QPushButton:pressed {"
        "  background: #16a085;"
        "}";
    
    // Boutons de menu
    btnPageRoots = new QPushButton("▸ Racines (إدارة الجذور)");
    btnPageRoots->setStyleSheet(btnStyle);
    connect(btnPageRoots, &QPushButton::clicked, [this]() { showPage(0); });
    sideMenuLayout->addWidget(btnPageRoots);
    
    btnPagePatterns = new QPushButton("▸ Schèmes (إدارة الأوزان)");
    btnPagePatterns->setStyleSheet(btnStyle);
    connect(btnPagePatterns, &QPushButton::clicked, [this]() { showPage(1); });
    sideMenuLayout->addWidget(btnPagePatterns);
    
    btnPageGenerate = new QPushButton("▸ Générer (توليد كلمة)");
    btnPageGenerate->setStyleSheet(btnStyle);
    connect(btnPageGenerate, &QPushButton::clicked, [this]() { showPage(2); });
    sideMenuLayout->addWidget(btnPageGenerate);
    
    btnPageValidate = new QPushButton("▸ Valider (التحقق من كلمة)");
    btnPageValidate->setStyleSheet(btnStyle);
    connect(btnPageValidate, &QPushButton::clicked, [this]() { showPage(3); });
    sideMenuLayout->addWidget(btnPageValidate);
    
    btnPageExplore = new QPushButton("▸ Explorer (استكشاف العائلة)");
    btnPageExplore->setStyleSheet(btnStyle);
    connect(btnPageExplore, &QPushButton::clicked, [this]() { showPage(4); });
    sideMenuLayout->addWidget(btnPageExplore);
    
    btnPageAll = new QPushButton("▸ Toutes données (كل البيانات)");
    btnPageAll->setStyleSheet(btnStyle);
    connect(btnPageAll, &QPushButton::clicked, [this]() { showPage(5); });
    sideMenuLayout->addWidget(btnPageAll);
    
    btnPageBonus = new QPushButton("★ Bonus (حلول إضافية)");
    btnPageBonus->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #f39c12, stop:1 #e67e22); "
        "color: white; padding: 15px; font-weight: bold; "
        "font-size: 11pt; border-radius: 5px; text-align: left; "
        "border: 2px solid #d68910;"
    );
    connect(btnPageBonus, &QPushButton::clicked, [this]() { showPage(6); });
    sideMenuLayout->addWidget(btnPageBonus);
    
    sideMenuLayout->addStretch();
    
    // Info en bas
    QLabel *infoLabel = new QLabel("Structures de données:\n• Arbre Binaire (ABR)\n• Table de Hachage\n• Listes Chaînées");
    infoLabel->setStyleSheet(
        "color: #bdc3c7; font-size: 9pt; "
        "padding: 10px; background: #2c3e50; "
        "border-radius: 5px;"
    );
    infoLabel->setWordWrap(true);
    sideMenuLayout->addWidget(infoLabel);
}

void MorphologyGUI::createPages() {
    // ========== PAGE 1: RACINES ==========
    pageRoots = new QWidget();
    QVBoxLayout *rootsLayout = new QVBoxLayout(pageRoots);
    rootsLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *rootsTitle = new QLabel("■ إدارة الجذور العربية");
    QFont pageTitleFont;
    pageTitleFont.setPointSize(16);
    pageTitleFont.setBold(true);
    rootsTitle->setFont(pageTitleFont);
    rootsTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    rootsLayout->addWidget(rootsTitle);
    
    QGroupBox *rootsInputGroup = new QGroupBox("الإدخال (Entrée)");
    rootsInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *rootsInputLayout = new QVBoxLayout(rootsInputGroup);
    
    rootInput = new QLineEdit();
    rootInput->setPlaceholderText("أدخل جذراً عربياً (مثال: كتب، درس، قرأ)");
    rootInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    rootInput->setMinimumHeight(60);
    rootInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(rootInput);  // Configuration RTL
    rootsInputLayout->addWidget(rootInput);
    rootsLayout->addWidget(rootsInputGroup);
    
    QGroupBox *rootsActionsGroup = new QGroupBox("الإجراءات (Actions)");
    rootsActionsGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QHBoxLayout *rootsActionsLayout = new QHBoxLayout(rootsActionsGroup);
    
    btnAddRoot = new QPushButton("+ إضافة");
    btnAddRoot->setStyleSheet(
        "background: #27ae60; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnAddRoot, &QPushButton::clicked, this, &MorphologyGUI::onAddRoot);
    
    btnDeleteRoot = new QPushButton("✕ حذف");
    btnDeleteRoot->setStyleSheet(
        "background: #e74c3c; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnDeleteRoot, &QPushButton::clicked, this, &MorphologyGUI::onDeleteRoot);
    
    btnSearchRoot = new QPushButton("⊙ بحث");
    btnSearchRoot->setStyleSheet(
        "background: #3498db; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnSearchRoot, &QPushButton::clicked, this, &MorphologyGUI::onSearchRoot);
    
    btnShowAllRoots = new QPushButton("≡ عرض الكل");
    btnShowAllRoots->setStyleSheet(
        "background: #9b59b6; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnShowAllRoots, &QPushButton::clicked, this, &MorphologyGUI::onShowAllRoots);
    
    rootsActionsLayout->addWidget(btnAddRoot);
    rootsActionsLayout->addWidget(btnDeleteRoot);
    rootsActionsLayout->addWidget(btnSearchRoot);
    rootsActionsLayout->addWidget(btnShowAllRoots);
    rootsLayout->addWidget(rootsActionsGroup);
    
    QGroupBox *rootsDisplayGroup = new QGroupBox("النتائج (Résultats)");
    rootsDisplayGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *rootsDisplayLayout = new QVBoxLayout(rootsDisplayGroup);
    
    rootsDisplay = new QTextEdit();
    rootsDisplay->setReadOnly(true);
    rootsDisplay->setStyleSheet(
        "background: #ecf0f1; border: 2px solid #95a5a6; "
        "border-radius: 5px; padding: 10px; font-size: 12pt;"
    );
    rootsDisplayLayout->addWidget(rootsDisplay);
    rootsLayout->addWidget(rootsDisplayGroup);
    
    stackedWidget->addWidget(pageRoots);
    
    // ========== PAGE 2: SCHÈMES ==========
    pagePatterns = new QWidget();
    QVBoxLayout *patternsLayout = new QVBoxLayout(pagePatterns);
    patternsLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *patternsTitle = new QLabel("■ إدارة الأوزان الصرفية");
    patternsTitle->setFont(pageTitleFont);
    patternsTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    patternsLayout->addWidget(patternsTitle);
    
    QGroupBox *patternsInputGroup = new QGroupBox("Entrée du Schème");
    patternsInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *patternsInputLayout = new QVBoxLayout(patternsInputGroup);
    
    QLabel *patternNameLabel = new QLabel("Nom du schème:");
    patternNameInput = new QLineEdit();
    patternNameInput->setPlaceholderText("Ex: فاعل، مفعول، افتعل");
    patternNameInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    patternNameInput->setMinimumHeight(60);
    patternNameInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(patternNameInput);  // Configuration RTL
    
    QLabel *patternRepLabel = new QLabel("Représentation:");
    patternRepInput = new QLineEdit();
    patternRepInput->setPlaceholderText("Ex: ف-ا-ع-ل");
    patternRepInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    patternRepInput->setMinimumHeight(60);
    patternRepInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(patternRepInput);  // Configuration RTL
    
    patternsInputLayout->addWidget(patternNameLabel);
    patternsInputLayout->addWidget(patternNameInput);
    patternsInputLayout->addWidget(patternRepLabel);
    patternsInputLayout->addWidget(patternRepInput);
    patternsLayout->addWidget(patternsInputGroup);
    
    QGroupBox *patternsActionsGroup = new QGroupBox("Actions");
    patternsActionsGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QHBoxLayout *patternsActionsLayout = new QHBoxLayout(patternsActionsGroup);
    
    btnAddPattern = new QPushButton("+ إضافة");
    btnAddPattern->setStyleSheet(
        "background: #27ae60; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnAddPattern, &QPushButton::clicked, this, &MorphologyGUI::onAddPattern);
    
    btnModifyPattern = new QPushButton("✎ تعديل");
    btnModifyPattern->setStyleSheet(
        "background: #f39c12; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnModifyPattern, &QPushButton::clicked, this, &MorphologyGUI::onModifyPattern);
    
    btnDeletePattern = new QPushButton("✕ حذف");
    btnDeletePattern->setStyleSheet(
        "background: #e74c3c; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnDeletePattern, &QPushButton::clicked, this, &MorphologyGUI::onDeletePattern);
    
    btnShowAllPatterns = new QPushButton("≡ عرض الكل");
    btnShowAllPatterns->setStyleSheet(
        "background: #9b59b6; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnShowAllPatterns, &QPushButton::clicked, this, &MorphologyGUI::onShowAllPatterns);
    
    patternsActionsLayout->addWidget(btnAddPattern);
    patternsActionsLayout->addWidget(btnModifyPattern);
    patternsActionsLayout->addWidget(btnDeletePattern);
    patternsActionsLayout->addWidget(btnShowAllPatterns);
    patternsLayout->addWidget(patternsActionsGroup);
    
    QGroupBox *patternsDisplayGroup = new QGroupBox("Schèmes disponibles");
    patternsDisplayGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *patternsDisplayLayout = new QVBoxLayout(patternsDisplayGroup);
    
    patternsDisplay = new QTextEdit();
    patternsDisplay->setReadOnly(true);
    patternsDisplay->setStyleSheet(
        "background: #ecf0f1; border: 2px solid #95a5a6; "
        "border-radius: 5px; padding: 10px; font-size: 12pt;"
    );
    patternsDisplayLayout->addWidget(patternsDisplay);
    patternsLayout->addWidget(patternsDisplayGroup);
    
    stackedWidget->addWidget(pagePatterns);
    
    // ========== PAGE 3: GÉNÉRATION ==========
    pageGenerate = new QWidget();
    QVBoxLayout *generateLayout = new QVBoxLayout(pageGenerate);
    generateLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *generateTitle = new QLabel("■ توليد الكلمات المشتقة");
    generateTitle->setFont(pageTitleFont);
    generateTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    generateLayout->addWidget(generateTitle);
    
    QGroupBox *generateInputGroup = new QGroupBox("معاملات التوليد (Paramètres)");
    generateInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *generateInputLayout = new QVBoxLayout(generateInputGroup);
    
    QLabel *genRootLabel = new QLabel("الجذر (Racine):");
    genRootInput = new QLineEdit();
    genRootInput->setPlaceholderText("أدخل الجذر (مثال: كتب)");
    genRootInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    genRootInput->setMinimumHeight(60);
    genRootInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(genRootInput);  // Configuration RTL
    
    QLabel *genPatternLabel = new QLabel("الوزن الصرفي (Schème):");
    genPatternCombo = new QComboBox();
    genPatternCombo->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    genPatternCombo->setMinimumHeight(60);
    loadPatternsToComboBox();
    
    generateInputLayout->addWidget(genRootLabel);
    generateInputLayout->addWidget(genRootInput);
    generateInputLayout->addWidget(genPatternLabel);
    generateInputLayout->addWidget(genPatternCombo);
    generateLayout->addWidget(generateInputGroup);
    
    btnGenerate = new QPushButton("▶ توليد الكلمة");
    btnGenerate->setStyleSheet(
        "background: #e67e22; color: white; "
        "padding: 15px; font-weight: bold; font-size: 12pt; border-radius: 5px;"
    );
    connect(btnGenerate, &QPushButton::clicked, this, &MorphologyGUI::onGenerateWord);
    generateLayout->addWidget(btnGenerate);
    
    QGroupBox *genResultGroup = new QGroupBox("نتيجة التوليد (Résultat)");
    genResultGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *genResultLayout = new QVBoxLayout(genResultGroup);
    
    genResultLabel = new QLabel("في الانتظار...");
    genResultLabel->setStyleSheet(
        "padding: 30px; font-size: 24pt; font-weight: bold; "
        "background: #ecf0f1; border: 2px solid #95a5a6; border-radius: 5px; "
        "color: #7f8c8d;"
    );
    genResultLabel->setAlignment(Qt::AlignCenter);
    genResultLayout->addWidget(genResultLabel);
    generateLayout->addWidget(genResultGroup);
    
    generateLayout->addStretch();
    stackedWidget->addWidget(pageGenerate);
    
    // ========== PAGE 4: VALIDATION ==========
    pageValidate = new QWidget();
    QVBoxLayout *validateLayout = new QVBoxLayout(pageValidate);
    validateLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *validateTitle = new QLabel("■ التحقق الصرفي");
    validateTitle->setFont(pageTitleFont);
    validateTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    validateLayout->addWidget(validateTitle);
    
    QGroupBox *validateInputGroup = new QGroupBox("معاملات التحقق (Paramètres)");
    validateInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *validateInputLayout = new QVBoxLayout(validateInputGroup);
    
    QLabel *valWordLabel = new QLabel("الكلمة للتحقق (Mot):");
    valWordInput = new QLineEdit();
    valWordInput->setPlaceholderText("أدخل الكلمة المشتقة (مثال: كاتب)");
    valWordInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    valWordInput->setMinimumHeight(60);
    valWordInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(valWordInput);  // Configuration RTL
    
    QLabel *valRootLabel = new QLabel("الجذر المتوقع (اختياري):");
    valRootInput = new QLineEdit();
    valRootInput->setPlaceholderText("مثال: كتب (اترك فارغاً للكشف التلقائي)");
    valRootInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    valRootInput->setMinimumHeight(60);
    valRootInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(valRootInput);  // Configuration RTL
    
    validateInputLayout->addWidget(valWordLabel);
    validateInputLayout->addWidget(valWordInput);
    validateInputLayout->addWidget(valRootLabel);
    validateInputLayout->addWidget(valRootInput);
    validateLayout->addWidget(validateInputGroup);
    
    btnValidate = new QPushButton("✓ التحقق من الصحة");
    btnValidate->setStyleSheet(
        "background: #16a085; color: white; "
        "padding: 15px; font-weight: bold; font-size: 12pt; border-radius: 5px;"
    );
    connect(btnValidate, &QPushButton::clicked, this, &MorphologyGUI::onValidateWord);
    validateLayout->addWidget(btnValidate);
    
    QGroupBox *valResultGroup = new QGroupBox("نتيجة التحقق (Résultat)");
    valResultGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *valResultLayout = new QVBoxLayout(valResultGroup);
    
    valResultLabel = new QLabel("في الانتظار...");
    valResultLabel->setStyleSheet(
        "padding: 30px; font-size: 20pt; font-weight: bold; "
        "background: #ecf0f1; border: 2px solid #95a5a6; border-radius: 5px; "
        "color: #7f8c8d;"
    );
    valResultLabel->setAlignment(Qt::AlignCenter);
    valResultLayout->addWidget(valResultLabel);
    validateLayout->addWidget(valResultGroup);
    
    validateLayout->addStretch();
    stackedWidget->addWidget(pageValidate);
    
    // ========== PAGE 5: EXPLORER FAMILLE ==========
    pageExplore = new QWidget();
    QVBoxLayout *exploreLayout = new QVBoxLayout(pageExplore);
    exploreLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *exploreTitle = new QLabel("■ استكشاف العائلة الصرفية");
    exploreTitle->setFont(pageTitleFont);
    exploreTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    exploreLayout->addWidget(exploreTitle);
    
    QGroupBox *exploreInputGroup = new QGroupBox("الجذر للاستكشاف (Racine)");
    exploreInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *exploreInputLayout = new QVBoxLayout(exploreInputGroup);
    
    exploreRootInput = new QLineEdit();
    exploreRootInput->setPlaceholderText("أدخل جذراً (مثال: كتب)");
    exploreRootInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #3498db; border-radius: 5px;"
    );
    exploreRootInput->setMinimumHeight(60);
    exploreRootInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(exploreRootInput);  // Configuration RTL
    exploreInputLayout->addWidget(exploreRootInput);
    exploreLayout->addWidget(exploreInputGroup);
    
    btnExploreFamily = new QPushButton("→ عرض المشتقات");
    btnExploreFamily->setStyleSheet(
        "background: #8e44ad; color: white; "
        "padding: 15px; font-weight: bold; font-size: 12pt; border-radius: 5px;"
    );
    connect(btnExploreFamily, &QPushButton::clicked, this, &MorphologyGUI::onExploreFamilyForRoot);
    exploreLayout->addWidget(btnExploreFamily);
    
    QGroupBox *familyGroup = new QGroupBox("الكلمات المشتقة (Mots dérivés)");
    familyGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *familyLayout = new QVBoxLayout(familyGroup);
    
    familyList = new QListWidget();
    familyList->setStyleSheet(
        "background: #ecf0f1; border: 2px solid #95a5a6; "
        "border-radius: 5px; padding: 5px; font-size: 13pt;"
    );
    familyLayout->addWidget(familyList);
    exploreLayout->addWidget(familyGroup);
    
    stackedWidget->addWidget(pageExplore);
    
    // ========== PAGE 6: TOUTES LES DONNÉES ==========
    pageAll = new QWidget();
    QVBoxLayout *allLayout = new QVBoxLayout(pageAll);
    allLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *allTitle = new QLabel("■ كل الجذور ومشتقاتها");
    allTitle->setFont(pageTitleFont);
    allTitle->setStyleSheet("color: #2c3e50; padding: 10px; background: #ecf0f1; border-radius: 5px;");
    allLayout->addWidget(allTitle);
    
    btnRefreshAll = new QPushButton("↻ تحديث");
    btnRefreshAll->setStyleSheet(
        "background: #2980b9; color: white; "
        "padding: 12px; font-weight: bold; font-size: 11pt; border-radius: 5px;"
    );
    connect(btnRefreshAll, &QPushButton::clicked, this, &MorphologyGUI::onShowAllRootsAndDerivatives);
    allLayout->addWidget(btnRefreshAll);
    
    QGroupBox *allTreeGroup = new QGroupBox("شجرة الجذور → المشتقات");
    allTreeGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *allTreeLayout = new QVBoxLayout(allTreeGroup);
    
    allTreeWidget = new QTreeWidget();
    allTreeWidget->setHeaderLabels({"الجذر / المشتق", "النوع", "الوزن"});
    allTreeWidget->setAlternatingRowColors(true);
    allTreeWidget->setStyleSheet(
        "QTreeWidget { background-color: white; color: black; "
        "border: 2px solid #3498db; border-radius: 5px; font-size: 11pt; } "
        "QTreeWidget::item { padding: 5px; color: black; } "
        "QTreeWidget::item:selected { background-color: #3498db; color: white; } "
        "QHeaderView::section { background-color: #34495e; color: white; "
        "padding: 8px; font-weight: bold; border: none; }"
    );
    allTreeLayout->addWidget(allTreeWidget);
    allLayout->addWidget(allTreeGroup);
    
    stackedWidget->addWidget(pageAll);
    
    // ========== PAGE 7: SOLUTIONS BONUS ==========
    pageBonus = new QWidget();
    QVBoxLayout *bonusLayout = new QVBoxLayout(pageBonus);
    bonusLayout->setContentsMargins(20, 20, 20, 20);
    
    QLabel *bonusTitle = new QLabel("★ حلول إضافية - الأفعال المعتلة");
    bonusTitle->setFont(pageTitleFont);
    bonusTitle->setStyleSheet(
        "color: white; padding: 15px; "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #f39c12, stop:1 #e67e22); "
        "border-radius: 5px; font-size: 18pt;"
    );
    bonusLayout->addWidget(bonusTitle);
    
    // Description
    QLabel *bonusDesc = new QLabel(
        "هذه الوظيفة المتقدمة تحلل الأفعال المعتلة العربية. "
        "الأفعال المعتلة تحتوي على حروف علة (و، ي) في جذرها وتتبع قواعد صرفية خاصة."
    );
    bonusDesc->setWordWrap(true);
    bonusDesc->setStyleSheet(
        "background: #fff3cd; color: #856404; "
        "padding: 15px; border-radius: 5px; "
        "border-left: 4px solid #f39c12; font-size: 10pt;"
    );
    bonusLayout->addWidget(bonusDesc);
    
    // Zone d'entrée
    QGroupBox *bonusInputGroup = new QGroupBox("تحليل فعل (Analyse)");
    bonusInputGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *bonusInputLayout = new QVBoxLayout(bonusInputGroup);
    
    bonusVerbInput = new QLineEdit();
    bonusVerbInput->setPlaceholderText("أدخل جذر فعل (مثال: قول، باع، وقف، وعد)");
    bonusVerbInput->setStyleSheet(
        "padding: 15px 12px; font-size: 18pt; line-height: 1.5; "
        "border: 2px solid #f39c12; border-radius: 5px;"
    );
    bonusVerbInput->setMinimumHeight(60);
    bonusVerbInput->setTextMargins(5, 5, 5, 5);
    setupArabicInput(bonusVerbInput);  // Configuration RTL
    bonusInputLayout->addWidget(bonusVerbInput);
    bonusLayout->addWidget(bonusInputGroup);
    
    // Boutons d'action
    QHBoxLayout *bonusButtonsLayout = new QHBoxLayout();
    
    btnAnalyzeVerb = new QPushButton("⊕ تحليل النوع");
    btnAnalyzeVerb->setStyleSheet(
        "background: #f39c12; color: white; "
        "padding: 15px; font-weight: bold; font-size: 12pt; border-radius: 5px;"
    );
    connect(btnAnalyzeVerb, &QPushButton::clicked, this, &MorphologyGUI::onAnalyzeDefectiveVerb);
    bonusButtonsLayout->addWidget(btnAnalyzeVerb);
    
    btnShowExamples = new QPushButton("≡ عرض الأمثلة");
    btnShowExamples->setStyleSheet(
        "background: #e67e22; color: white; "
        "padding: 15px; font-weight: bold; font-size: 12pt; border-radius: 5px;"
    );
    connect(btnShowExamples, &QPushButton::clicked, this, &MorphologyGUI::onShowVerbExamples);
    bonusButtonsLayout->addWidget(btnShowExamples);
    
    bonusLayout->addLayout(bonusButtonsLayout);
    
    // Label de résultat du type
    bonusTypeLabel = new QLabel("في الانتظار...");
    bonusTypeLabel->setStyleSheet(
        "padding: 20px; font-size: 16pt; font-weight: bold; "
        "background: #ecf0f1; border: 2px solid #95a5a6; "
        "border-radius: 5px; color: #7f8c8d;"
    );
    bonusTypeLabel->setAlignment(Qt::AlignCenter);
    bonusLayout->addWidget(bonusTypeLabel);
    
    // Zone d'affichage des résultats
    QGroupBox *bonusResultGroup = new QGroupBox("التفاصيل والشرح (Détails)");
    bonusResultGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; }");
    QVBoxLayout *bonusResultLayout = new QVBoxLayout(bonusResultGroup);
    
    bonusResultDisplay = new QTextEdit();
    bonusResultDisplay->setReadOnly(true);
    bonusResultDisplay->setStyleSheet(
        "background: #ecf0f1; border: 2px solid #95a5a6; "
        "border-radius: 5px; padding: 10px; font-size: 11pt;"
    );
    bonusResultLayout->addWidget(bonusResultDisplay);
    bonusLayout->addWidget(bonusResultGroup);
    
    stackedWidget->addWidget(pageBonus);
}

void MorphologyGUI::showPage(int index) {
    stackedWidget->setCurrentIndex(index);
    
    QString pageName;
    switch(index) {
        case 0: pageName = "Gestion des Racines"; break;
        case 1: pageName = "Gestion des Schèmes"; break;
        case 2: pageName = "Génération de Mots"; break;
        case 3: pageName = "Validation Morphologique"; break;
        case 4: pageName = "Explorer Famille"; break;
        case 5: pageName = "Toutes les Données"; break;
        case 6: pageName = "Solutions Bonus - Verbes Défectueux"; break;
        default: pageName = "Page " + QString::number(index + 1);
    }
    
    logMessage("→ Navigation: " + pageName);
}

void MorphologyGUI::logMessage(const QString &message) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    logArea->append("[" + timestamp + "] " + message);
}

void MorphologyGUI::loadPatternsToComboBox() {
    genPatternCombo->clear();
    genPatternCombo->addItem("فاعل");
    genPatternCombo->addItem("مفعول");
    genPatternCombo->addItem("افتعل");
    genPatternCombo->addItem("تفعيل");
    genPatternCombo->addItem("مفعال");
    genPatternCombo->addItem("فعال");
}

// ========== PAGE 1: RACINES ==========
void MorphologyGUI::onAddRoot() {
    QString root = cleanArabicText(rootInput->text());
    
    if (root.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une racine");
        return;
    }
    
    std::string rootStr = root.toStdString();
    
    if (globalEngine->rootExists(rootStr)) {
        logMessage("[!] La racine '" + root + "' existe déjà dans l'ABR");
        rootsDisplay->clear();
        rootsDisplay->append("[X] Racine déjà présente: " + root + "\n");
        
        // Afficher quand même les mots dérivés
        std::string derived = globalEngine->getDerivedWordsText(rootStr);
        rootsDisplay->append(QString::fromStdString(derived));
        
        QMessageBox::information(this, "Info", "Cette racine existe déjà dans l'ABR");
        return;
    }
    
    globalEngine->addRoot(rootStr);
    logMessage("[OK] Racine '" + root + "' ajoutée à l'ABR");
    
    rootsDisplay->clear();
    rootsDisplay->append("[+] Racine ajoutée: " + root + "\n");
    
    // Générer et afficher les mots dérivés automatiquement
    std::string derived = globalEngine->getDerivedWordsText(rootStr);
    rootsDisplay->append(QString::fromStdString(derived));
    
    rootInput->clear();
    QMessageBox::information(this, "Succès", "Racine ajoutée avec succès !\nConsultez les mots dérivés ci-dessous.");
}

void MorphologyGUI::onDeleteRoot() {
    QString root = cleanArabicText(rootInput->text());
    
    if (root.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une racine à supprimer");
        return;
    }
    
    std::string rootStr = root.toStdString();
    
    if (!globalEngine->rootExists(rootStr)) {
        logMessage("[!] La racine '" + root + "' n'existe pas");
        rootsDisplay->append("[X] Racine non trouvée: " + root);
        QMessageBox::warning(this, "Erreur", "Cette racine n'existe pas dans l'ABR");
        return;
    }
    
    globalEngine->removeRoot(rootStr);
    logMessage("[OK] Racine '" + root + "' supprimée de l'ABR");
    rootsDisplay->append("[-] Racine supprimée: " + root);
    rootInput->clear();
    QMessageBox::information(this, "Succès", "Racine supprimée avec succès");
}

void MorphologyGUI::onSearchRoot() {
    QString root = cleanArabicText(rootInput->text());
    
    if (root.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une racine à chercher");
        return;
    }
    
    std::string rootStr = root.toStdString();
    
    if (globalEngine->rootExists(rootStr)) {
        logMessage("✓ Racine trouvée: " + root);
        rootsDisplay->clear();
        rootsDisplay->append("[OK] Racine trouvée dans l'ABR: " + root + "\n");
        
        std::string derived = globalEngine->getDerivedWordsText(rootStr);
        if (!derived.empty()) {
            rootsDisplay->append(QString::fromStdString(derived));
        } else {
            rootsDisplay->append("(Aucun mot dérivé généré pour cette racine)");
        }
        
        QMessageBox::information(this, "Trouvé", "Racine trouvée dans l'ABR");
    } else {
        logMessage("[X] Racine '" + root + "' non trouvée");
        rootsDisplay->clear();
        rootsDisplay->append("[X] Racine NON trouvée: " + root);
        QMessageBox::warning(this, "Non trouvé", "Cette racine n'existe pas dans l'ABR");
    }
}

void MorphologyGUI::onShowAllRoots() {
    logMessage("Affichage de toutes les racines (parcours inordre de l'ABR)");
    rootsDisplay->clear();
    rootsDisplay->append("═══ PARCOURS INORDRE DE L'ABR ═══\n");
    
    std::string rootsText = globalEngine->getAllRootsText();
    
    if (rootsText.empty() || globalEngine->getRootCount() == 0) {
        rootsDisplay->append("ABR vide - Aucune racine");
        return;
    }
    
    rootsDisplay->append(QString::fromStdString(rootsText));
    rootsDisplay->append("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    rootsDisplay->append("Total: " + QString::number(globalEngine->getRootCount()) + " racines");
    
    logMessage("[OK] " + QString::number(globalEngine->getRootCount()) + " racines affichées");
}

// ========== PAGE 2: SCHÈMES ==========
void MorphologyGUI::onAddPattern() {
    QString name = cleanArabicText(patternNameInput->text());
    QString rep = cleanArabicText(patternRepInput->text());
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال اسم الوزن\nVeuillez entrer le nom du schème");
        return;
    }
    
    if (rep.isEmpty()) {
        rep = "ف-ع-ل"; // Valeur par défaut
    }
    
    std::string nameStr = name.toStdString();
    std::string repStr = rep.toStdString();
    
    // Vérifier si le schème existe déjà
    if (globalEngine->findPattern(nameStr) != nullptr) {
        logMessage("[!] الوزن '" + name + "' موجود مسبقاً");
        QMessageBox::warning(this, "تحذير (Avertissement)", 
            "الوزن موجود مسبقاً\nCe schème existe déjà.\n\nUtilisez 'تعديل' pour le modifier.");
        return;
    }
    
    Pattern newPattern(nameStr, repStr, "Schème personnalisé");
    bool success = globalEngine->addPattern(newPattern);
    
    if (success) {
        logMessage("[+] ✓ تمت الإضافة: " + name + " (" + rep + ")");
        patternsDisplay->append("\n[+] Ajouté: " + name + " | " + rep);
        patternNameInput->clear();
        patternRepInput->clear();
        QMessageBox::information(this, "نجح (Succès)", 
            "تمت إضافة الوزن بنجاح!\nSchème ajouté avec succès!");
    } else {
        logMessage("[✗] فشل في إضافة: " + name);
        QMessageBox::critical(this, "خطأ (Erreur)", "فشل في الإضافة\nÉchec de l'ajout");
    }
}

void MorphologyGUI::onModifyPattern() {
    QString name = cleanArabicText(patternNameInput->text());
    QString rep = cleanArabicText(patternRepInput->text());
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال اسم الوزن\nVeuillez entrer le nom du schème");
        return;
    }
    
    if (rep.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال التمثيل\nVeuillez entrer la représentation");
        return;
    }
    
    std::string nameStr = name.toStdString();
    std::string repStr = rep.toStdString();
    
    // Vérifier si le schème existe
    if (globalEngine->findPattern(nameStr) == nullptr) {
        logMessage("[!] الوزن '" + name + "' غير موجود");
        QMessageBox::warning(this, "تحذير (Avertissement)", 
            "الوزن غير موجود\\nCe schème n'existe pas.");
        return;
    }
    
    Pattern updatedPattern(nameStr, repStr, "Schème modifié");
    bool success = globalEngine->updatePattern(updatedPattern);
    
    if (success) {
        logMessage("[~] ✓ تم التعديل: " + name + " (" + rep + ")");
        patternsDisplay->append("\n[~] Modifié: " + name + " | " + rep);
        patternNameInput->clear();
        patternRepInput->clear();
        QMessageBox::information(this, "نجح (Succès)", 
            "تم تعديل الوزن بنجاح!\\nSchème modifié avec succès!");
    } else {
        logMessage("[✗] فشل في التعديل: " + name);
        QMessageBox::critical(this, "خطأ (Erreur)", "فشل في التعديل\\nÉchec de la modification");
    }
}

void MorphologyGUI::onDeletePattern() {
    QString name = cleanArabicText(patternNameInput->text());
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال اسم الوزن\\nVeuillez entrer le nom du schème");
        return;
    }
    
    std::string nameStr = name.toStdString();
    
    // Vérifier si le schème existe
    if (globalEngine->findPattern(nameStr) == nullptr) {
        logMessage("[!] الوزن '" + name + "' غير موجود");
        QMessageBox::warning(this, "تحذير (Avertissement)", 
            "الوزن غير موجود\nCe schème n'existe pas.");
        return;
    }
    
    // Confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "تأكيد الحذف (Confirmation)",
        "هل تريد حذف الوزن '" + name + "'؟\nVoulez-vous supprimer ce schème ?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = globalEngine->removePattern(nameStr);
        
        if (success) {
            logMessage("[-] ✓ تم الحذف: " + name);
            patternsDisplay->append("\n[-] Supprimé: " + name);
            patternNameInput->clear();
            QMessageBox::information(this, "نجح (Succès)", 
                "تم حذف الوزن بنجاح!\\nSchème supprimé avec succès!");
        } else {
            logMessage("[✗] فشل في الحذف: " + name);
            QMessageBox::critical(this, "خطأ (Erreur)", "فشل في الحذف\\nÉchec de la suppression");
        }
    }
}

void MorphologyGUI::onShowAllPatterns() {
    logMessage("Affichage de tous les schèmes");
    patternsDisplay->clear();
    patternsDisplay->append("═══ SCHÈMES MORPHOLOGIQUES (Table de Hachage) ═══\n");
    
    std::string patternsText = globalEngine->getAllPatternsText();
    patternsDisplay->append(QString::fromStdString(patternsText));
    
    logMessage("[OK] " + QString::number(globalEngine->getPatternCount()) + " schèmes affichés");
}

// ========== PAGE 3: GÉNÉRATION ==========
void MorphologyGUI::onGenerateWord() {
    QString root = cleanArabicText(genRootInput->text());
    QString pattern = genPatternCombo->currentText();
    
    if (root.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال جذر\nVeuillez entrer une racine");
        return;
    }
    
    std::string rootStr = root.toStdString();
    std::string patternStr = pattern.toStdString();
    
    logMessage("→ توليد من: " + root + " + " + pattern);
    
    if (!globalEngine->rootExists(rootStr)) {
        logMessage("[X] الجذر '" + root + "' غير موجود");
        genResultLabel->setText("✗ الجذر غير موجود\nRacine non trouvée");
        genResultLabel->setStyleSheet(
            "padding: 30px; font-size: 18pt; font-weight: bold; "
            "background: #e74c3c; border: 2px solid #c0392b; border-radius: 5px; color: white;"
        );
        QMessageBox::warning(this, "خطأ (Erreur)", 
            "الجذر غير موجود في الشجرة\nCette racine n'existe pas dans l'ABR\n\n"
            "💡 أضف الجذر أولاً من صفحة 'إدارة الجذور'");
        return;
    }
    
    std::string derivedWord = globalEngine->generateDerivedWord(rootStr, patternStr);
    
    if (derivedWord.empty() || derivedWord == "ERREUR") {
        logMessage("[X] خطأ في التوليد");
        genResultLabel->setText("✗ خطأ في التوليد\nErreur de génération");
        genResultLabel->setStyleSheet(
            "padding: 30px; font-size: 18pt; font-weight: bold; "
            "background: #e74c3c; border: 2px solid #c0392b; border-radius: 5px; color: white;"
        );
        QMessageBox::warning(this, "خطأ (Erreur)", 
            "فشل توليد الكلمة\nÉchec de la génération");
        return;
    }
    
    logMessage("[OK] ✓ تم التوليد: " + QString::fromStdString(derivedWord));
    genResultLabel->setText(QString::fromStdString(derivedWord));
    genResultLabel->setStyleSheet(
        "padding: 30px; font-size: 32pt; font-weight: bold; "
        "background: #27ae60; border: 2px solid #229954; border-radius: 5px; color: white;"
    );
    
    QMessageBox::information(this, "✓ نجح (Succès)", 
        QString("تم توليد الكلمة بنجاح!\nMot généré avec succès!\n\n"
                "الجذر: %1\nالوزن: %2\n\n→ الكلمة المولدة: %3")
        .arg(root).arg(pattern).arg(QString::fromStdString(derivedWord)));
}

// ========== PAGE 4: VALIDATION ==========
void MorphologyGUI::onValidateWord() {
    QString word = cleanArabicText(valWordInput->text());
    QString rootHint = cleanArabicText(valRootInput->text());
    
    if (word.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال كلمة للتحقق منها\nVeuillez entrer un mot à valider");
        return;
    }
    
    std::string wordStr = word.toStdString();
    std::string rootStr = rootHint.toStdString();
    
    logMessage("→ تحقق من الكلمة: " + word);
    
    ValidationResult result;
    
    // Si une racine est fournie, valider avec cette racine
    if (!rootStr.empty()) {
        logMessage("→ مع الجذر المحدد: " + rootHint);
        result = globalEngine->validateWord(wordStr, rootStr);
    } else {
        // Sinon, chercher la racine automatiquement
        logMessage("→ كشف تلقائي للجذر...");
        result = globalEngine->findRootOfWord(wordStr);
    }
    
    if (result.isValid) {
        logMessage("[OK] ✓ كلمة صحيحة: " + word);
        valResultLabel->setText("✓ كلمة صحيحة صرفياً\nMOT VALIDE");
        valResultLabel->setStyleSheet(
            "padding: 30px; font-size: 20pt; font-weight: bold; "
            "background: #27ae60; border: 2px solid #229954; border-radius: 5px; color: white;"
        );
        
        std::string foundRoot = result.root;
        QString info = "الكلمة صحيحة صرفياً ✓\nLe mot est morphologiquement valide";
        
        if (!foundRoot.empty()) {
            info += "\n\n━━━━━━━━━━━━━━━━";
            info += "\nالجذر المكتشف: " + QString::fromStdString(foundRoot);
            info += "\nRacine détectée: " + QString::fromStdString(foundRoot);
            
            if (!result.pattern.empty()) {
                info += "\n\nالوزن الصرفي: " + QString::fromStdString(result.pattern);
                info += "\nSchème: " + QString::fromStdString(result.pattern);
            }
            
            logMessage("→ الجذر: " + QString::fromStdString(foundRoot) + 
                      " | الوزن: " + QString::fromStdString(result.pattern));
        } else {
            info += "\n\nملاحظة: لم يتم الكشف عن الجذر تلقائياً";
            info += "\nNote: Racine non détectée automatiquement";
        }
        
        QMessageBox::information(this, "✓ صحيح (Valide)", info);
        
        // إظهار الجذر في حقل الجذر إذا تم اكتشافه
        if (!foundRoot.empty() && rootHint.isEmpty()) {
            valRootInput->setText(QString::fromStdString(foundRoot));
        }
    } else {
        logMessage("[X] ✗ كلمة غير صحيحة: " + word);
        valResultLabel->setText("✗ كلمة غير صحيحة صرفياً\nMOT INVALIDE");
        valResultLabel->setStyleSheet(
            "padding: 30px; font-size: 20pt; font-weight: bold; "
            "background: #e74c3c; border: 2px solid #c0392b; border-radius: 5px; color: white;"
        );
        
        QString errorMsg = "هذه الكلمة ليست صحيحة صرفياً\nCe mot n'est pas morphologiquement valide";
        
        // إضافة نصائح
        errorMsg += "\n\n💡 نصائح (Conseils):";
        errorMsg += "\n• تأكد من الإدخال الصحيح";
        errorMsg += "\n• جرب إدخال الجذر يدوياً";
        errorMsg += "\n• تأكد أن الكلمة مشتقة من جذر موجود";
        
        QMessageBox::warning(this, "✗ غير صحيح (Invalide)", errorMsg);
    }
}

// ========== PAGE 5: EXPLORER FAMILLE ==========
void MorphologyGUI::onExploreFamilyForRoot() {
    QString root = cleanArabicText(exploreRootInput->text());
    
    if (root.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال جذر\nVeuillez entrer une racine");
        return;
    }
    
    std::string rootStr = root.toStdString();
    
    if (!globalEngine->rootExists(rootStr)) {
        logMessage("[X] الجذر '" + root + "' غير موجود");
        familyList->clear();
        familyList->addItem("✗ الجذر غير موجود في الشجرة");
        familyList->addItem("✗ Racine non trouvée dans l'ABR");
        familyList->addItem("");
        familyList->addItem("💡 أضف الجذر أولاً من صفحة 'إدارة الجذور'");
        QMessageBox::warning(this, "خطأ (Erreur)", 
            "الجذر غير موجود\nCette racine n'existe pas dans l'ABR");
        return;
    }
    
    logMessage("→ استكشاف عائلة: " + root);
    familyList->clear();
    familyList->addItem("▸ العائلة الصرفية للجذر: " + root);
    familyList->addItem("▸ Famille morphologique de: " + root);
    familyList->addItem("━━━━━━━━━━━━━━━━━━━━━━━━");
    
    std::string derived = globalEngine->getDerivedWordsText(rootStr);
    
    if (derived.empty()) {
        familyList->addItem("");
        familyList->addItem("⚠ لا توجد مشتقات مولدة لهذا الجذر");
        familyList->addItem("⚠ Aucun mot dérivé généré");
        familyList->addItem("");
        familyList->addItem("💡 نصيحة: استخدم صفحة 'توليد كلمة' لإنشاء مشتقات");
        familyList->addItem("💡 Conseil: Utilisez 'Générer un Mot' pour créer des dérivés");
    } else {
        // Afficher directement les dérivés (getDerivedWordsText inclut déjà le total)
        QString derivedQt = QString::fromStdString(derived);
        QStringList lines = derivedQt.split('\n');
        for (const QString& line : lines) {
            if (!line.trimmed().isEmpty()) {
                familyList->addItem(line);
            }
        }
    }
    
    logMessage("[OK] ✓ تم عرض العائلة: " + QString::number(familyList->count() - 3) + " عناصر");
}

// ========== PAGE 6: TOUT AFFICHER ==========
void MorphologyGUI::onShowAllRootsAndDerivatives() {
    logMessage("Actualisation de toutes les données...");
    allTreeWidget->clear();
    
    // Récupérer toutes les racines via un parcours
    std::string allRootsText = globalEngine->getAllRootsAndDerivativesText();
    
    // Parser le texte pour extraire les racines
    std::vector<std::string> allRoots;
    std::istringstream iss(allRootsText);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.find("Racine: ") != std::string::npos) {
            std::string root = line.substr(8); // Après "Racine: "
            allRoots.push_back(root);
        }
    }
    
    if (allRoots.empty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(allTreeWidget);
        item->setText(0, "Aucune racine dans l'ABR");
        item->setText(1, "-");
        item->setText(2, "-");
        return;
    }
    
    for (const auto& root : allRoots) {
        QTreeWidgetItem *rootItem = new QTreeWidgetItem(allTreeWidget);
        rootItem->setText(0, QString::fromStdString(root));
        rootItem->setText(1, "RACINE");
        rootItem->setText(2, "-");
        
        QFont boldFont;
        boldFont.setBold(true);
        boldFont.setPointSize(12);
        rootItem->setFont(0, boldFont);
        rootItem->setForeground(0, QBrush(QColor("#2c3e50")));
        rootItem->setBackground(0, QBrush(QColor("#ecf0f1")));
        
        // Récupérer les dérivés
        std::string derivedText = globalEngine->getDerivedWordsText(root);
        if (!derivedText.empty() && derivedText != "Aucun mot dérivé." && derivedText != "Racine non trouvée.") {
            // Parser les dérivés (format: "1. word | Schème: pattern | Fréquence: freq")
            QString derivedQt = QString::fromStdString(derivedText);
            QStringList lines = derivedQt.split('\n');
            for (const QString& line : lines) {
                if (line.contains("Schème:") && line.contains("|")) {
                    // Extraire le mot et le pattern
                    QStringList parts = line.split("|");
                    if (parts.size() >= 2) {
                        // Partie 0: "1. word"
                        QString wordPart = parts[0].trimmed();
                        // Enlever le numéro au début ("1. ")
                        int dotPos = wordPart.indexOf(".");
                        if (dotPos > 0) {
                            wordPart = wordPart.mid(dotPos + 1).trimmed();
                        }
                        
                        // Partie 1: "Schème: pattern"
                        QString patternPart = parts[1].trimmed();
                        QString pattern = patternPart.replace("Schème:", "").trimmed();
                        
                        QTreeWidgetItem *derivedItem = new QTreeWidgetItem(rootItem);
                        derivedItem->setText(0, wordPart);
                        derivedItem->setText(1, "DÉRIVÉ");
                        derivedItem->setText(2, pattern);
                        derivedItem->setForeground(0, QBrush(QColor("#16a085")));
                        derivedItem->setForeground(2, QBrush(QColor("#e67e22")));
                    }
                }
            }
        }
        
        rootItem->setExpanded(true);
    }
    
    allTreeWidget->expandAll();
    logMessage("[OK] " + QString::number(allRoots.size()) + " racines affichées avec leurs dérivés");
}

// ========== UTILITAIRES ==========
void MorphologyGUI::onClearLogs() {
    logArea->clear();
    logMessage("Logs effacés");
}

// ========== PAGE 7: SOLUTIONS BONUS ==========
void MorphologyGUI::onAnalyzeDefectiveVerb() {
    QString verb = cleanArabicText(bonusVerbInput->text());
    
    if (verb.isEmpty()) {
        QMessageBox::warning(this, "خطأ (Erreur)", "الرجاء إدخال جذر فعل\\nVeuillez entrer une racine verbale");
        return;
    }
    
    std::string verbStr = verb.toStdString();
    bonusResultDisplay->clear();
    
    // Diviser le verbe en caractères UTF-8
    std::vector<std::string> chars = Utils::utf8Split(verbStr);
    
    // Filtrer les espaces, diacritiques et caractères vides
    std::vector<std::string> cleanChars;
    for (const auto& ch : chars) {
        // Ignorer les espaces, tabulations, retours à la ligne
        if (ch.empty() || ch == " " || ch == "\t" || ch == "\n") {
            continue;
        }
        
        // Ignorer les diacritiques arabes (harakat et shaddah)
        // Shaddah: 0x0651 (ّ), Fatha: 0x064E (َ), Damma: 0x064F (ُ), 
        // Kasra: 0x0650 (ِ), Sukun: 0x0652 (ْ), Tanwin, etc.
        if (ch == "\u0651" || ch == "\u064B" || ch == "\u064C" || ch == "\u064D" ||
            ch == "\u064E" || ch == "\u064F" || ch == "\u0650" || ch == "\u0652") {
            logMessage("→ Diacritique ignoré: " + QString::fromStdString(ch));
            continue;
        }
        
        cleanChars.push_back(ch);
    }
    chars = cleanChars;
    
    if (chars.size() < 2 || chars.size() > 4) {
        QMessageBox::warning(this, "خطأ (Erreur)", 
            "الرجاء إدخال جذر ثلاثي (2-4 أحرف)\\nVeuillez entrer une racine (2-4 lettres)");
        return;
    }
    
    // Log détaillé pour débogage
    QString debugInfo = "→ تحليل: " + verb + " | عدد الأحرف: " + QString::number(chars.size());
    for (size_t i = 0; i < chars.size(); i++) {
        debugInfo += " | L" + QString::number(i+1) + "=" + QString::fromStdString(chars[i]);
    }
    logMessage(debugInfo);
    
    // Vérifier si c'est un verbe مضعّف (doublement géminé)
    bool isDoubled = false;
    std::string repeatedLetter = "";
    
    if (chars.size() == 2) {
        // شد = ش + د → forme contractée de ش-د-د
        isDoubled = true;
        repeatedLetter = chars[1]; // La lettre doublée est la deuxième
        logMessage("→ Détection: 2 lettres → مضعّف implicite (L2=" + QString::fromStdString(repeatedLetter) + ")");
    } else if (chars.size() == 3 && chars[1] == chars[2]) {
        // شدد = ش + د + د (les deux dernières lettres identiques)
        isDoubled = true;
        repeatedLetter = chars[1];
        logMessage("→ Détection: 3 lettres avec L2=L3 → مضعّف explicite (L2=L3=" + QString::fromStdString(repeatedLetter) + ")");
    } else {
        logMessage("→ Pas de gémination détectée (size=" + QString::number(chars.size()) + 
                   ", L2≠L3=" + (chars.size() >= 3 ? QString::number(chars[1] != chars[2]) : "N/A") + ")");
    }
    
    if (isDoubled) {
        // فعل مضعّف
        bonusTypeLabel->setText("★ فعل مضعّف (Verbe Géminé)");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #16a085; border: 2px solid #138d75; "
            "border-radius: 5px; color: white;"
        );
        
        QString fullRoot = verb;
        if (chars.size() == 2) {
            // Reconstruire la racine complète
            fullRoot = QString::fromStdString(chars[0]) + QString::fromStdString(repeatedLetter) + QString::fromStdString(repeatedLetter);
        }
        
        bonusResultDisplay->append("═══ ANALYSE MORPHOLOGIQUE ═══\\n");
        bonusResultDisplay->append("Type: فعل مضعّف (Verbe Géminé/Doublé)");
        bonusResultDisplay->append("Racine entrée: " + verb);
        bonusResultDisplay->append("Racine complète: " + fullRoot + " (" + 
                                 QString::fromStdString(chars[0]) + "-" + 
                                 QString::fromStdString(repeatedLetter) + "-" + 
                                 QString::fromStdString(repeatedLetter) + ")\\n");
        bonusResultDisplay->append("Description:");
        bonusResultDisplay->append("• Le deuxième et troisième radicaux sont IDENTIQUES");
        bonusResultDisplay->append("• S'écrit souvent avec une shaddah (ّ)");
        bonusResultDisplay->append("• La racine complète est trilitère: " + fullRoot);
        bonusResultDisplay->append("• Conjugaison spéciale avec fusion possible\\n");
        
        bonusResultDisplay->append("Exemples de verbes مضعّف:");
        bonusResultDisplay->append("• شدّ (shadda) : serrer → ش-د-د");
        bonusResultDisplay->append("• مدّ (madda) : étendre → م-د-د");
        bonusResultDisplay->append("• عدّ ('adda) : compter → ع-د-د");
        bonusResultDisplay->append("• ردّ (radda) : répondre → ر-د-د");
        bonusResultDisplay->append("• فرّ (farra) : fuir → ف-ر-ر\\n");
        
        bonusResultDisplay->append("Règles morphologiques:");
        bonusResultDisplay->append("• Les deux lettres identiques fusionnent avec shaddah");
        bonusResultDisplay->append("• Au passé: شَدَّ (shadda) - forme contractée");
        bonusResultDisplay->append("• Au présent: يَشُدُّ (yashudd)");
        bonusResultDisplay->append("• Séparation possible: شَدَدْتُ (shadadtu)");
        bonusResultDisplay->append("• La shaddah représente deux consonnes identiques\\n");
        
        bonusResultDisplay->append("💡 Note importante:");
        bonusResultDisplay->append("Dans votre système, traitez ce verbe comme ayant");
        bonusResultDisplay->append("3 radicaux: " + fullRoot);
        
        logMessage("[OK] ✓ Analyse: " + verb + " → فعل مضعّف (" + fullRoot + ")");
        QMessageBox::information(this, "✓ مضعّف", 
            QString("Type: فعل مضعّف\\nVerbe Géminé (lettres doublées)\\n\\n") + 
            "Forme entrée: " + verb + "\\nRacine complète: " + fullRoot);
        return;
    }
    
    // Vérifier si la racine contient des lettres faibles
    int weakPos = -1;
    std::string weakLetter = "";
    
    for (size_t i = 0; i < chars.size(); i++) {
        if (chars[i] == "و" || chars[i] == "ي" || chars[i] == "ا" || chars[i] == "ى") {
            weakPos = i;
            weakLetter = chars[i];
            break;
        }
    }
    
    if (weakPos == -1) {
        // Verbe sain (صحيح)
        bonusTypeLabel->setText("✓ فعل صحيح (Verbe Sain)");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #27ae60; border: 2px solid #229954; "
            "border-radius: 5px; color: white;"
        );
        
        bonusResultDisplay->append("═══ ANALYSE MORPHOLOGIQUE ═══\\n");
        bonusResultDisplay->append("Type: فعل صحيح (Verbe Sain)");
        bonusResultDisplay->append("Racine: " + verb + "\\n");
        bonusResultDisplay->append("Description:");
        bonusResultDisplay->append("• Ce verbe ne contient PAS de lettres faibles (و، ي، ا)");
        bonusResultDisplay->append("• Il suit les règles morphologiques standard");
        bonusResultDisplay->append("• La conjugaison est régulière sans modifications\\n");
        bonusResultDisplay->append("Exemples de verbes sains:");
        bonusResultDisplay->append("• كتب (kataba) - écrire");
        bonusResultDisplay->append("• درس (darasa) - étudier");
        bonusResultDisplay->append("• فعل (fa'ala) - faire");
        
        logMessage("[OK] Analyse: " + verb + " → فعل صحيح");
        return;
    }
    
    // Analyser le type de verbe défectueux
    QString verbType;
    QString arabicType;
    QString description;
    QString examples;
    QString rules;
    
    // Déterminer le type selon la position de la lettre faible
    if (weakPos == 0) {
        // فعل مثال - Lettre faible au début (1ère radicale)
        verbType = "Verbe Assimilé (Mithal)";
        arabicType = "فعل مثال";
        description = "La première lettre de la racine est faible (" + QString::fromStdString(weakLetter) + ")";
        examples = "• وقف (waqafa) - s'arrêter\\n"
                  "• وعد (wa'ada) - promettre\\n"
                  "• وجد (wajada) - trouver\\n"
                  "• يسر (yasara) - faciliter";
        rules = "• La lettre faible se transforme souvent en ا\\n"
               "• Parfois elle disparaît complètement\\n"
               "• Ex: وقف → مضارع: يقف (le و disparaît)";
        
        bonusTypeLabel->setText("★ " + arabicType + " (" + verbType + ")");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #3498db; border: 2px solid #2980b9; "
            "border-radius: 5px; color: white;"
        );
        
    } else if (weakPos == 1) {
        // فعل أجوف - Lettre faible au milieu (2ème radicale)
        verbType = "Verbe Creux (Ajwaf)";
        arabicType = "فعل أجوف";
        description = "La deuxième lettre (médiane) de la racine est faible (" + QString::fromStdString(weakLetter) + ")";
        examples = "• قول → قال (qala) - dire\\n"
                  "• بيع → باع (ba'a) - vendre\\n"
                  "• نوم → نام (nama) - dormir\\n"
                  "• صوم → صام (sama) - jeûner";
        rules = "• La lettre faible se contracte souvent\\n"
               "• Elle devient ا au passé\\n"
               "• Ex: قَوَلَ → قال (و devient ا)\\n"
               "• Conjugaison: قال، يقول، قُلْ";
        
        bonusTypeLabel->setText("★ " + arabicType + " (" + verbType + ")");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #e74c3c; border: 2px solid #c0392b; "
            "border-radius: 5px; color: white;"
        );
        
    } else if (weakPos == 2 || weakPos == chars.size() - 1) {
        // فعل ناقص - Lettre faible à la fin (3ème radicale)
        verbType = "Verbe Défectif (Naqis)";
        arabicType = "فعل ناقص";
        description = "La dernière lettre de la racine est faible (" + QString::fromStdString(weakLetter) + ")";
        examples = "• رمي → رمى (rama) - lancer\\n"
                  "• دعو → دعا (da'a) - appeler\\n"
                  "• مشي → مشى (masha) - marcher\\n"
                  "• سعي → سعى (sa'a) - s'efforcer";
        rules = "• La lettre faible finale change selon la voyelle\\n"
               "• Elle devient ى ou ا selon le contexte\\n"
               "• Ex: رَمَيَ → رمى\\n"
               "• Conjugaison spéciale à l'impératif";
        
        bonusTypeLabel->setText("★ " + arabicType + " (" + verbType + ")");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #9b59b6; border: 2px solid #8e44ad; "
            "border-radius: 5px; color: white;"
        );
        
    } else {
        // Autre cas ou verbe doublement défectueux
        verbType = "Verbe Complexe";
        arabicType = "فعل لفيف أو مركب";
        description = "Verbe avec plusieurs lettres faibles (لفيف) ou cas particulier";
        examples = "• وفى (wafa) - tenir sa promesse\\n"
                  "• وقى (waqa) - protéger\\n"
                  "• وعى (wa'a) - comprendre";
        rules = "• Combine les règles de plusieurs types\\n"
               "• Traitement morphologique complexe\\n"
               "• Nécessite une analyse approfondie";
        
        bonusTypeLabel->setText("★ " + arabicType + " (" + verbType + ")");
        bonusTypeLabel->setStyleSheet(
            "padding: 20px; font-size: 16pt; font-weight: bold; "
            "background: #f39c12; border: 2px solid #e67e22; "
            "border-radius: 5px; color: white;"
        );
    }
    
    // Afficher les détails
    bonusResultDisplay->append("═══ ANALYSE MORPHOLOGIQUE ═══\\n");
    bonusResultDisplay->append("Type: " + arabicType + " (" + verbType + ")");
    bonusResultDisplay->append("Racine analysée: " + verb);
    bonusResultDisplay->append("Nombre de lettres: " + QString::number(chars.size()));
    bonusResultDisplay->append("Position lettre faible: " + QString::number(weakPos + 1) + " (" + QString::fromStdString(weakLetter) + ")\\n");
    
    bonusResultDisplay->append("Description:");
    bonusResultDisplay->append(description + "\\n");
    
    bonusResultDisplay->append("Exemples:");
    bonusResultDisplay->append(examples + "\\n");
    
    bonusResultDisplay->append("Règles morphologiques:");
    bonusResultDisplay->append(rules);
    
    logMessage("[★] Analyse Bonus: " + verb + " → " + arabicType);
    QMessageBox::information(this, "Analyse Terminée", 
        "Type détecté: " + arabicType + "\\n(" + verbType + ")");
}

void MorphologyGUI::onShowVerbExamples() {
    logMessage("Affichage des exemples de verbes défectueux");
    
    bonusResultDisplay->clear();
    bonusTypeLabel->setText("📚 Classification des Verbes Arabes");
    bonusTypeLabel->setStyleSheet(
        "padding: 20px; font-size: 16pt; font-weight: bold; "
        "background: #34495e; border: 2px solid #2c3e50; "
        "border-radius: 5px; color: white;"
    );
    
    bonusResultDisplay->append("═══════════════════════════════════════════════════");
    bonusResultDisplay->append("   CLASSIFICATION COMPLÈTE DES VERBES ARABES");
    bonusResultDisplay->append("═══════════════════════════════════════════════════\n");
    
    bonusResultDisplay->append("1️⃣ الأفعال الصحيحة (VERBES SAINS)\n");
    bonusResultDisplay->append("• Ne contiennent AUCUNE lettre faible");
    bonusResultDisplay->append("• Conjugaison régulière et prévisible");
    bonusResultDisplay->append("• Exemples:");
    bonusResultDisplay->append("  - كتب (kataba) : écrire");
    bonusResultDisplay->append("  - درس (darasa) : étudier");
    bonusResultDisplay->append("  - فهم (fahima) : comprendre");
    bonusResultDisplay->append("  - شرب (shariba) : boire\n");
    
    bonusResultDisplay->append("2️⃣ الأفعال المضعّفة (VERBES GÉMINÉS) ⭐\n");
    bonusResultDisplay->append("• 2ème et 3ème radicaux IDENTIQUES");
    bonusResultDisplay->append("• S'écrivent avec shaddah (ّ)");
    bonusResultDisplay->append("• Traités comme trilitéraux");
    bonusResultDisplay->append("• Exemples:");
    bonusResultDisplay->append("  - شدّ (shadda) : serrer → ش-د-د");
    bonusResultDisplay->append("  - مدّ (madda) : étendre → م-د-د");
    bonusResultDisplay->append("  - عدّ ('adda) : compter → ع-د-د");
    bonusResultDisplay->append("  - ردّ (radda) : répondre → ر-د-د");
    bonusResultDisplay->append("  - فرّ (farra) : fuir → ف-ر-ر");
    bonusResultDisplay->append("• Règle: La shaddah = 2 lettres fusionnées");
    bonusResultDisplay->append("• Au passé: شَدَّ  Au présent: يَشُدُّ\n");
    
    bonusResultDisplay->append("3️⃣ الأفعال المعتلة (VERBES DÉFECTUEUX)\n");
    bonusResultDisplay->append("Contiennent des lettres faibles: و، ي، ا\n");
    
    bonusResultDisplay->append("A) فعل مثال (VERBE ASSIMILÉ)");
    bonusResultDisplay->append("   • Lettre faible au DÉBUT");
    bonusResultDisplay->append("   • Exemples:");
    bonusResultDisplay->append("     - وقف (waqafa) : s'arrêter");
    bonusResultDisplay->append("     - وعد (wa'ada) : promettre");
    bonusResultDisplay->append("     - وجد (wajada) : trouver");
    bonusResultDisplay->append("     - يسر (yasara) : faciliter\n");
    
    bonusResultDisplay->append("B) فعل أجوف (VERBE CREUX) ⭐");
    bonusResultDisplay->append("   • Lettre faible au MILIEU");
    bonusResultDisplay->append("   • Le plus complexe morphologiquement");
    bonusResultDisplay->append("   • Exemples:");
    bonusResultDisplay->append("     - قال (qala) : dire → ق-و-ل");
    bonusResultDisplay->append("     - باع (ba'a) : vendre → ب-ي-ع");
    bonusResultDisplay->append("     - نام (nama) : dormir → ن-و-م");
    bonusResultDisplay->append("     - صام (sama) : jeûner → ص-و-م");
    bonusResultDisplay->append("     - طار (tara) : voler → ط-ي-ر\n");
    
    bonusResultDisplay->append("C) فعل ناقص (VERBE DÉFECTIF)");
    bonusResultDisplay->append("   • Lettre faible à la FIN");
    bonusResultDisplay->append("   • Exemples:");
    bonusResultDisplay->append("     - رمى (rama) : lancer → ر-م-ي");
    bonusResultDisplay->append("     - دعا (da'a) : appeler → د-ع-و");
    bonusResultDisplay->append("     - مشى (masha) : marcher → م-ش-ي");
    bonusResultDisplay->append("     - سعى (sa'a) : s'efforcer → س-ع-ي\n");
    
    bonusResultDisplay->append("D) فعل لفيف (VERBE DOUBLEMENT DÉFECTIF)");
    bonusResultDisplay->append("   • DEUX lettres faibles");
    bonusResultDisplay->append("   • Très rare et complexe");
    bonusResultDisplay->append("   • Exemples:");
    bonusResultDisplay->append("     - وفى (wafa) : tenir sa promesse");
    bonusResultDisplay->append("     - وقى (waqa) : protéger");
    bonusResultDisplay->append("     - روى (rawa) : raconter\n");
    
    bonusResultDisplay->append("═══════════════════════════════════════════════════");
    bonusResultDisplay->append("💡 ASTUCES:");
    bonusResultDisplay->append("• Les verbes مضعّف: la shaddah = 2 lettres!");
    bonusResultDisplay->append("• Les verbes أجوف: les plus fréquents à maîtriser");
    bonusResultDisplay->append("• Testez: شدّ, مدّ, قول, وقف, رمي dans le champ");
    bonusResultDisplay->append("═══════════════════════════════════════════════════");
    
    logMessage("[OK] Exemples de classification affichés");
    QMessageBox::information(this, "Guide Complet", 
        "Classification complète des verbes arabes affichée.\n\n"
        "Utilisez le champ de saisie pour tester vos propres racines !");
}
