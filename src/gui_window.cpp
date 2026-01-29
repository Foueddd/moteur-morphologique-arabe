#include "gui_window.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>
#include <QtWidgets/QFrame>
#include <QtWidgets/QStatusBar>

MainWindow::MainWindow() {
    setWindowTitle("Moteur de Recherche Morphologique Arabe");
    resize(900, 650);

    // Thème sombre + typographie
    QString styleSheet =
        "QWidget { background-color: #0f172a; color: #e2e8f0; font-size: 14px; }"
        "QLineEdit, QTextEdit { background-color: #111827; color: #e2e8f0; border: 1px solid #334155; border-radius: 6px; padding: 6px; }"
        "QPushButton { background-color: #1f2937; color: #e2e8f0; border: 1px solid #334155; border-radius: 6px; padding: 6px 10px; }"
        "QPushButton:hover { background-color: #374151; }"
        "QGroupBox { border: 1px solid #334155; border-radius: 8px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 6px; }"
        "QTabWidget::pane { border: 1px solid #334155; }"
        "QTabBar::tab { background: #1f2937; padding: 8px 12px; border: 1px solid #334155; border-bottom: none; }"
        "QTabBar::tab:selected { background: #0b1220; }";
    qApp->setStyleSheet(styleSheet);
    QFont font("Noto Naskh Arabic", 11);
    font.setStyleStrategy(QFont::PreferAntialias);
    qApp->setFont(font);
    setLayoutDirection(Qt::RightToLeft);

    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    QLabel* title = new QLabel("محرك البحث الصرفي العربي — Arabic Morphology Engine");
    QFont titleFont = qApp->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("padding: 10px; background-color: #111827; border: 1px solid #334155; border-radius: 8px;");

    QTabWidget* tabs = new QTabWidget(this);
    tabs->addTab(createHomeTab(), "الصفحة الرئيسية | Accueil");
    tabs->addTab(createRootsTab(), "الجذور | Racines");
    tabs->addTab(createPatternsTab(), "الأوزان | Schèmes");
    tabs->addTab(createGenerationTab(), "التوليد | Génération");
    tabs->addTab(createValidationTab(), "التحقق | Validation");
    tabs->addTab(createFamilyTab(), "العائلة الصرفية | Famille");
    tabs->addTab(createAdvancedTab(), "بحث متقدم | Suggestions");
    tabs->addTab(createAllTab(), "عرض الكل | Tout afficher");

    mainLayout->addWidget(title);
    mainLayout->addWidget(tabs);
    setCentralWidget(central);
    statusBar()->showMessage("جاهز | Prêt");

    loadRootsFromFile();
    initDefaultPatterns();
}

void MainWindow::setOutputText(QTextEdit* output, const QString& text) {
    if (!output) return;
    output->clear();
    output->setPlainText(text);
}

QWidget* MainWindow::createHomeTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QLabel* welcome = new QLabel(
        "مرحباً بك في محرك البحث الصرفي العربي\n"
        "Welcome to the Arabic Morphology Engine");
    QFont wf = qApp->font();
    wf.setPointSize(13);
    wf.setBold(true);
    welcome->setFont(wf);
    welcome->setAlignment(Qt::AlignCenter);

    QLabel* subtitle = new QLabel(
        "واجهة واضحة وسهلة لإدارة الجذور والأوزان والتوليد والتحقق\n"
        "Interface claire pour gérer racines, schèmes, génération et validation");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #94a3b8; margin-bottom: 8px;");

    QFrame* card = new QFrame();
    card->setStyleSheet("background-color: #111827; border: 1px solid #334155; border-radius: 10px; padding: 12px;");
    QVBoxLayout* cardLayout = new QVBoxLayout(card);

    QLabel* tips = new QLabel(
        "• ابدأ بتحميل الجذور ثم تهيئة الأوزان الافتراضية\n"
        "• Start by loading roots then initialize default patterns\n"
        "• استخدم التوليد للتحقق من صحة المخرجات");
    tips->setStyleSheet("color: #cbd5e1;");

    QHBoxLayout* quick = new QHBoxLayout();
    QPushButton* initPatterns = new QPushButton("تهيئة الأوزان | Init");
    QPushButton* showRoots = new QPushButton("عرض الجذور | Roots");
    QPushButton* showAll = new QPushButton("عرض الكل | All");
    initPatterns->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    showRoots->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    showAll->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    quick->addWidget(initPatterns);
    quick->addWidget(showRoots);
    quick->addWidget(showAll);

    cardLayout->addWidget(tips);
    cardLayout->addLayout(quick);

    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);
    output->setPlaceholderText("نتائج سريعة | Quick results");

    layout->addWidget(welcome);
    layout->addWidget(subtitle);
    layout->addWidget(card);
    layout->addWidget(output);

    QObject::connect(initPatterns, &QPushButton::clicked, [=]() {
        initDefaultPatterns();
        setOutputText(output, "تمت تهيئة الأوزان الافتراضية | Schèmes initialisés.");
    });

    QObject::connect(showRoots, &QPushButton::clicked, [=]() {
        setOutputText(output, QString::fromStdString(engine.getAllRootsText()));
    });

    QObject::connect(showAll, &QPushButton::clicked, [=]() {
        setOutputText(output, QString::fromStdString(engine.getAllRootsAndDerivativesText()));
    });

    return tab;
}

void MainWindow::loadRootsFromFile() {
    const std::string rootsFile = "data/roots.txt";
    if (Utils::fileExists(rootsFile)) {
        std::vector<std::string> roots = Utils::loadRootsFromFile(rootsFile);
        for (const auto& r : roots) {
            if (Utils::isValidArabicRoot(r)) {
                engine.addRoot(r);
            }
        }
    }
}

void MainWindow::initDefaultPatterns() {
    Pattern p1("فاعل", "VCCCVC", "Participe actif - Agent (celui qui fait)");
    Pattern p2("مفعول", "CVCCVC", "Participe passif - Patient (celui qui subit)");
    Pattern p3("افتعل", "VCVCCVC", "Forme VIII - Réflexive");
    Pattern p4("تفعيل", "VCVCCVC", "Forme II - Causatif");
    Pattern p5("مفعال", "CVCCVC", "Forme intensive");
    Pattern p6("فعال", "CVCVC", "Pluriel ou adjectif");

    engine.addPattern(p1);
    engine.addPattern(p2);
    engine.addPattern(p3);
    engine.addPattern(p4);
    engine.addPattern(p5);
    engine.addPattern(p6);
}

QWidget* MainWindow::createRootsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("إدارة الجذور | Gestion des racines");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* rootInput = new QLineEdit();
    rootInput->setPlaceholderText("مثال: كتب");
    rootInput->setToolTip("أدخل جذراً ثلاثياً باللغة العربية");
    form->addRow("الجذر | Racine:", rootInput);

    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("Ajouter");
    QPushButton* removeBtn = new QPushButton("Supprimer");
    QPushButton* searchBtn = new QPushButton("Rechercher");
    QPushButton* showAllBtn = new QPushButton("Afficher toutes les racines | عرض كل الجذور");
    addBtn->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    removeBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    searchBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    showAllBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    buttons->addWidget(addBtn);
    buttons->addWidget(removeBtn);
    buttons->addWidget(searchBtn);
    buttons->addWidget(showAllBtn);

    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addLayout(buttons);
    layout->addWidget(output);

    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        std::string root = rootInput->text().trimmed().toStdString();
        if (root.empty()) {
            setOutputText(output, "خطأ: الجذر فارغ | Racine vide.");
            return;
        }
        if (!Utils::isValidArabicRoot(root)) {
            setOutputText(output, "خطأ: الجذر غير صالح (ثلاثي) | Racine invalide.");
            return;
        }
        engine.addRoot(root);
        setOutputText(output, "تمت إضافة الجذر بنجاح | Racine ajoutée.");
    });

    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        std::string root = rootInput->text().trimmed().toStdString();
        if (root.empty()) {
            setOutputText(output, "خطأ: الجذر فارغ | Racine vide.");
            return;
        }
        if (engine.removeRoot(root)) {
            setOutputText(output, "تم حذف الجذر | Racine supprimée.");
        } else {
            setOutputText(output, "الجذر غير موجود | Racine non trouvée.");
        }
    });

    QObject::connect(searchBtn, &QPushButton::clicked, [=]() {
        std::string root = rootInput->text().trimmed().toStdString();
        if (root.empty()) {
            setOutputText(output, "Erreur: Racine vide.");
            return;
        }
        AVLNode* node = engine.findRoot(root);
        if (node == nullptr) {
            setOutputText(output, "الجذر غير موجود | Racine non trouvée.");
        } else {
            setOutputText(output, "تم العثور على الجذر | Racine trouvée: " + QString::fromStdString(root));
        }
    });

    QObject::connect(showAllBtn, &QPushButton::clicked, [=]() {
        setOutputText(output, QString::fromStdString(engine.getAllRootsText()));
    });

    return tab;
}

QWidget* MainWindow::createPatternsTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("إدارة الأوزان | Gestion des schèmes");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* nameInput = new QLineEdit();
    QLineEdit* structureInput = new QLineEdit();
    QLineEdit* descriptionInput = new QLineEdit();
    nameInput->setPlaceholderText("مثال: مفعول");
    structureInput->setPlaceholderText("مثال: CVCCVC");
    descriptionInput->setPlaceholderText("وصف قصير");
    nameInput->setToolTip("اسم الوزن (مثال: مفعول)");
    structureInput->setToolTip("البنية المجردة للوزن");
    descriptionInput->setToolTip("شرح مختصر للاستخدام");
    form->addRow("اسم الوزن | Nom:", nameInput);
    form->addRow("البنية | Structure:", structureInput);
    form->addRow("الوصف | Description:", descriptionInput);

    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("Ajouter");
    QPushButton* updateBtn = new QPushButton("Modifier");
    QPushButton* removeBtn = new QPushButton("Supprimer");
    QPushButton* showAllBtn = new QPushButton("Afficher tous les schèmes | عرض الأوزان");
    QPushButton* initBtn = new QPushButton("Init par défaut | افتراضي");
    addBtn->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    updateBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    removeBtn->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
    showAllBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    initBtn->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    buttons->addWidget(addBtn);
    buttons->addWidget(updateBtn);
    buttons->addWidget(removeBtn);
    buttons->addWidget(showAllBtn);
    buttons->addWidget(initBtn);

    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addLayout(buttons);
    layout->addWidget(output);

    QObject::connect(addBtn, &QPushButton::clicked, [=]() {
        std::string name = nameInput->text().trimmed().toStdString();
        std::string structure = structureInput->text().trimmed().toStdString();
        std::string description = descriptionInput->text().trimmed().toStdString();
        if (name.empty()) {
            setOutputText(output, "خطأ: الاسم فارغ | Nom vide.");
            return;
        }
        engine.addPattern(Pattern(name, structure, description));
        setOutputText(output, "تمت إضافة/تحديث الوزن | Schème ajouté.");
    });

    QObject::connect(updateBtn, &QPushButton::clicked, [=]() {
        std::string name = nameInput->text().trimmed().toStdString();
        std::string structure = structureInput->text().trimmed().toStdString();
        std::string description = descriptionInput->text().trimmed().toStdString();
        if (name.empty()) {
            setOutputText(output, "خطأ: الاسم فارغ | Nom vide.");
            return;
        }
        engine.updatePattern(Pattern(name, structure, description));
        setOutputText(output, "تم تعديل الوزن | Schème modifié.");
    });

    QObject::connect(removeBtn, &QPushButton::clicked, [=]() {
        std::string name = nameInput->text().trimmed().toStdString();
        if (name.empty()) {
            setOutputText(output, "خطأ: الاسم فارغ | Nom vide.");
            return;
        }
        if (engine.removePattern(name)) {
            setOutputText(output, "تم حذف الوزن | Schème supprimé.");
        } else {
            setOutputText(output, "الوزن غير موجود | Schème non trouvé.");
        }
    });

    QObject::connect(showAllBtn, &QPushButton::clicked, [=]() {
        setOutputText(output, QString::fromStdString(engine.getAllPatternsText()));
    });

    QObject::connect(initBtn, &QPushButton::clicked, [=]() {
        initDefaultPatterns();
        setOutputText(output, "Schèmes par défaut initialisés.");
    });

    return tab;
}

QWidget* MainWindow::createGenerationTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("توليد كلمة | Génération de mot");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* rootInput = new QLineEdit();
    QLineEdit* patternInput = new QLineEdit();
    rootInput->setPlaceholderText("مثال: كتب");
    patternInput->setPlaceholderText("مثال: مفعول");
    form->addRow("الجذر | Racine:", rootInput);
    form->addRow("الوزن | Schème:", patternInput);

    QPushButton* generateBtn = new QPushButton("Générer");
    generateBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addWidget(generateBtn);
    layout->addWidget(output);

    QObject::connect(generateBtn, &QPushButton::clicked, [=]() {
        std::string root = rootInput->text().trimmed().toStdString();
        std::string pattern = patternInput->text().trimmed().toStdString();
        if (root.empty() || pattern.empty()) {
            setOutputText(output, "خطأ: بيانات ناقصة | Entrée incomplète.");
            return;
        }
        std::string derived = engine.generateDerivedWord(root, pattern);
        if (derived.empty()) {
            setOutputText(output, "فشل التوليد | Échec de génération.");
            return;
        }
        engine.addDerivedWordToRoot(root, derived, pattern);
        QString result = "الكلمة المولدة: " + QString::fromStdString(derived) + "\n";
        result += "تم تحديث الجذر \"" + QString::fromStdString(root) + "\" بهذا المشتق.";
        setOutputText(output, result);
    });

    return tab;
}

QWidget* MainWindow::createValidationTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("التحقق الصرفي | Validation morphologique");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* wordInput = new QLineEdit();
    QLineEdit* rootInput = new QLineEdit();
    wordInput->setPlaceholderText("مثال: مكتوب");
    rootInput->setPlaceholderText("مثال: كتب");
    form->addRow("الكلمة | Mot:", wordInput);
    form->addRow("الجذر المتوقع | Racine attendue:", rootInput);

    QHBoxLayout* buttons = new QHBoxLayout();
    QPushButton* validateBtn = new QPushButton("Valider");
    QPushButton* findRootBtn = new QPushButton("Trouver la racine");
    validateBtn->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    findRootBtn->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
    buttons->addWidget(validateBtn);
    buttons->addWidget(findRootBtn);

    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addLayout(buttons);
    layout->addWidget(output);

    QObject::connect(validateBtn, &QPushButton::clicked, [=]() {
        std::string word = wordInput->text().trimmed().toStdString();
        std::string root = rootInput->text().trimmed().toStdString();
        if (word.empty() || root.empty()) {
            setOutputText(output, "خطأ: بيانات ناقصة | Entrée incomplète.");
            return;
        }
        ValidationResult result = engine.validateWord(word, root);
        if (result.isValid) {
            QString msg = "نعم | OUI - Schème: " + QString::fromStdString(result.pattern);
            setOutputText(output, msg);
        } else {
            setOutputText(output, "لا | NON - Mot invalide pour cette racine.");
        }
    });

    QObject::connect(findRootBtn, &QPushButton::clicked, [=]() {
        std::string word = wordInput->text().trimmed().toStdString();
        if (word.empty()) {
            setOutputText(output, "خطأ: كلمة فارغة | Mot vide.");
            return;
        }
        ValidationResult result = engine.findRootOfWord(word);
        if (result.isValid) {
            QString msg = "الجذر: " + QString::fromStdString(result.root) +
                          "\nالوزن: " + QString::fromStdString(result.pattern);
            setOutputText(output, msg);
        } else {
            setOutputText(output, "لا يوجد جذر مطابق | Aucune racine trouvée.");
        }
    });

    return tab;
}

QWidget* MainWindow::createFamilyTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("العائلة الصرفية | Famille morphologique");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* rootInput = new QLineEdit();
    rootInput->setPlaceholderText("مثال: كتب");
    form->addRow("الجذر | Racine:", rootInput);

    QPushButton* showBtn = new QPushButton("Afficher la famille");
    showBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addWidget(showBtn);
    layout->addWidget(output);

    QObject::connect(showBtn, &QPushButton::clicked, [=]() {
        std::string root = rootInput->text().trimmed().toStdString();
        if (root.empty()) {
            setOutputText(output, "خطأ: جذر فارغ | Racine vide.");
            return;
        }
        setOutputText(output, QString::fromStdString(engine.getDerivedWordsText(root)));
    });

    return tab;
}

QWidget* MainWindow::createAdvancedTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QGroupBox* formBox = new QGroupBox("بحث متقدم واقتراحات | Recherche avancée");
    QFormLayout* form = new QFormLayout(formBox);
    QLineEdit* queryInput = new QLineEdit();
    queryInput->setPlaceholderText("جزء من الجذر أو الحروف");
    form->addRow("البحث | Requête:", queryInput);

    QPushButton* suggestBtn = new QPushButton("Suggérer");
    suggestBtn->setIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));
    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(formBox);
    layout->addWidget(suggestBtn);
    layout->addWidget(output);

    QObject::connect(suggestBtn, &QPushButton::clicked, [=]() {
        std::string query = queryInput->text().trimmed().toStdString();
        if (query.empty()) {
            setOutputText(output, "خطأ: حقل فارغ | Requête vide.");
            return;
        }
        std::vector<std::string> suggestions = engine.suggestRoots(query);
        if (suggestions.empty()) {
            setOutputText(output, "لا توجد اقتراحات | Aucune suggestion.");
            return;
        }
        std::string result;
        for (size_t i = 0; i < suggestions.size(); i++) {
            result += std::to_string(i + 1) + ". " + suggestions[i] + "\n";
        }
        setOutputText(output, QString::fromStdString(result));
    });

    return tab;
}

QWidget* MainWindow::createAllTab() {
    QWidget* tab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QPushButton* showBtn = new QPushButton("عرض كل الجذور والمشتقات | Tout afficher");
    showBtn->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    QTextEdit* output = new QTextEdit();
    output->setReadOnly(true);

    layout->addWidget(showBtn);
    layout->addWidget(output);

    QObject::connect(showBtn, &QPushButton::clicked, [=]() {
        setOutputText(output, QString::fromStdString(engine.getAllRootsAndDerivativesText()));
    });

    return tab;
}
