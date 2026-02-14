#include "morphology_engine.h"
#include "utils.h"
#include <iostream>
#include <vector>

/**
 * ============================================================================
 * INTERFACE UTILISATEUR (CLI)
 * Moteur de Recherche Morphologique Arabe
 * Structures : ABR + Table de Hachage + Listes Chaînées
 * ============================================================================
 */

MorphologyEngine engine;

// ============================================================================
// FONCTIONS DE GESTION DES RACINES
// ============================================================================

void addRootMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "AJOUTER UNE RACINE" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string root = Utils::getInput("Entrez la racine arabe: ");
    
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }

    if (!Utils::isValidArabicRoot(root)) {
        std::cout << "Erreur: Racine invalide (doit être trilitérale)." << std::endl;
        return;
    }
    
    engine.addRoot(root);
}

void searchRootMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "CHERCHER UNE RACINE" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string root = Utils::getInput("Entrez la racine à chercher: ");
    
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }
    
    BSTNode* node = engine.findRoot(root);
    
    if (node == nullptr) {
        std::cout << "Racine '" << root << "' non trouvée." << std::endl;
    } else {
        std::cout << "✓ Racine '" << root << "' trouvée!" << std::endl;
        
        if (node->derivedList != nullptr) {
            std::cout << "  Mots dérivés associés: ";
            DerivedWord* current = node->derivedList;
            while (current != nullptr) {
                std::cout << current->word << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
}

void removeRootMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "SUPPRIMER UNE RACINE" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    std::string root = Utils::getInput("Entrez la racine à supprimer: ");
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }

    if (engine.removeRoot(root)) {
        std::cout << "✓ Racine supprimée." << std::endl;
    } else {
        std::cout << "Racine non trouvée." << std::endl;
    }
}

void displayRootsMenu() {
    engine.displayAllRoots();
}

// ============================================================================
// FONCTIONS DE GESTION DES SCHÈMES
// ============================================================================

void displayPatternsMenu() {
    engine.displayAllPatterns();
}

void initializePatternsMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "INITIALISER SCHÈMES PAR DÉFAUT" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    // Ajouter les schèmes de base
    Pattern p1("فاعل", "VCCCVC", "Participe actif - Agent (celui qui fait)");
    Pattern p2("مفعول", "CVCCVC", "Participe passif - Patient (celui qui subit)");
    Pattern p3("افتعل", "VCVCCVC", "Forme VIII - Réflexive");
    Pattern p4("تفعيل", "VCVCCVC", "Forme II - Causatif");
    Pattern p5("مفعال", "CVCCVC", "Forme intensive");
    Pattern p6("فعال", "CVCVC", "Pluriel ou adjectif");
    
    bool success = true;
    success &= engine.addPattern(p1);
    success &= engine.addPattern(p2);
    success &= engine.addPattern(p3);
    success &= engine.addPattern(p4);
    success &= engine.addPattern(p5);
    success &= engine.addPattern(p6);
    
    if (success) {
        std::cout << "\n✓ Schèmes initilialisés avec succès!" << std::endl;
        std::cout << "Total de schèmes: " << engine.getPatternCount() << std::endl;
    }
}

void addPatternMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "AJOUTER UN SCHÈME" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    std::string name = Utils::getInput("Nom du schème: ");
    std::string structure = Utils::getInput("Structure (ex: CVCVC): ");
    std::string description = Utils::getInput("Description: ");

    if (name.empty()) {
        std::cout << "Erreur: Nom vide." << std::endl;
        return;
    }

    Pattern p(name, structure, description);
    if (engine.addPattern(p)) {
        std::cout << "✓ Schème ajouté/actualisé." << std::endl;
    }
}

void updatePatternMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "MODIFIER UN SCHÈME" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    std::string name = Utils::getInput("Nom du schème à modifier: ");
    if (name.empty()) {
        std::cout << "Erreur: Nom vide." << std::endl;
        return;
    }

    std::string structure = Utils::getInput("Nouvelle structure: ");
    std::string description = Utils::getInput("Nouvelle description: ");

    Pattern p(name, structure, description);
    if (engine.updatePattern(p)) {
        std::cout << "✓ Schème modifié." << std::endl;
    }
}

void removePatternMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "SUPPRIMER UN SCHÈME" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    std::string name = Utils::getInput("Nom du schème à supprimer: ");
    if (name.empty()) {
        std::cout << "Erreur: Nom vide." << std::endl;
        return;
    }

    if (engine.removePattern(name)) {
        std::cout << "✓ Schème supprimé." << std::endl;
    } else {
        std::cout << "Schème non trouvé." << std::endl;
    }
}

// ============================================================================
// FONCTIONS DE GÉNÉRATION MORPHOLOGIQUE
// ============================================================================

void generateDerivedWordMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "GÉNÉRER UN MOT DÉRIVÉ" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string root = Utils::getInput("Entrez la racine: ");
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }
    
    if (!engine.rootExists(root)) {
        std::cout << "Erreur: Racine '" << root << "' n'existe pas." << std::endl;
        return;
    }
    
    std::string pattern = Utils::getInput("Entrez le schème: ");
    if (pattern.empty()) {
        std::cout << "Erreur: Schème vide." << std::endl;
        return;
    }
    
    std::string derived = engine.generateDerivedWord(root, pattern);
    
    if (!derived.empty()) {
        Utils::printGenerationResult(root, pattern, derived);
        // Le mot est déjà ajouté à la liste des dérivés par generateDerivedWord()
    }
}

void generateAllDerivativesMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "GÉNÉRER TOUS LES DÉRIVÉS D'UNE RACINE" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string root = Utils::getInput("Entrez la racine: ");
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }
    
    engine.generateAllDerivativesForRoot(root);
}

void displayDerivedWordsMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "AFFICHER DÉRIVÉS D'UNE RACINE" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string root = Utils::getInput("Entrez la racine: ");
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }
    
    engine.displayDerivedWordsOfRoot(root);
}

// ============================================================================
// FONCTIONS DE VALIDATION MORPHOLOGIQUE
// ============================================================================

void validateWordMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "VALIDER UN MOT" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string word = Utils::getInput("Entrez le mot à valider: ");
    if (word.empty()) {
        std::cout << "Erreur: Mot vide." << std::endl;
        return;
    }
    
    std::string root = Utils::getInput("Entrez la racine attendue: ");
    if (root.empty()) {
        std::cout << "Erreur: Racine vide." << std::endl;
        return;
    }
    
    ValidationResult result = engine.validateWord(word, root);
    Utils::printValidationResult(word, root, result.isValid, result.pattern);
}

void findRootOfWordMenu() {
    std::cout << "\n═══════════════════════════════════════════════════════════════\n";
    std::cout << "TROUVER LA RACINE D'UN MOT" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::string word = Utils::getInput("Entrez le mot: ");
    if (word.empty()) {
        std::cout << "Erreur: Mot vide." << std::endl;
        return;
    }
    
    ValidationResult result = engine.findRootOfWord(word);
    
    if (result.isValid) {
        std::cout << "\n";
        Utils::printSeparator(70, '-');
        std::cout << "RÉSULTAT" << std::endl;
        Utils::printSeparator(70, '-');
        std::cout << "Mot: " << word << std::endl;
        std::cout << "Racine trouvée: " << result.root << std::endl;
        std::cout << "Schème: " << result.pattern << std::endl;
        Utils::printSeparator(70, '-');
    } else {
        std::cout << "\nAucune racine trouvée pour le mot '" << word << "'." << std::endl;
    }
}

// ============================================================================
// AFFICHAGE STATISTIQUES
// ============================================================================

void displayStatisticsMenu() {
    Utils::printStatistics(engine.getRootCount(), engine.getPatternCount(), 
                          engine.getHashTableLoadFactor());
}

// ============================================================================
// MENU RACINES
// ============================================================================

void rootsMenu() {
    while (true) {
        std::cout << "\n┌────────────────────────────────────────┐\n";
        std::cout << "│   GESTION DES RACINES (Arbre ABR)     │\n";
        std::cout << "└────────────────────────────────────────┘\n\n";
        
        std::cout << "1. Ajouter une racine" << std::endl;
        std::cout << "2. Supprimer une racine" << std::endl;
        std::cout << "3. Chercher une racine" << std::endl;
        std::cout << "4. Afficher toutes les racines" << std::endl;
        std::cout << "0. Retour au menu principal" << std::endl;
        
        int choice = Utils::getChoice(0, 4);
        
        switch (choice) {
            case 1:
                addRootMenu();
                break;
            case 2:
                removeRootMenu();
                break;
            case 3:
                searchRootMenu();
                break;
            case 4:
                displayRootsMenu();
                break;
            case 0:
                return;
        }
        
        Utils::waitForKeypress();
    }
}

// ============================================================================
// MENU SCHÈMES
// ============================================================================

void patternsMenu() {
    while (true) {
        std::cout << "\n┌────────────────────────────────────────┐\n";
        std::cout << "│ GESTION DES SCHÈMES (Table de Hachage)│\n";
        std::cout << "└────────────────────────────────────────┘\n\n";
        
        std::cout << "1. Afficher tous les schèmes" << std::endl;
        std::cout << "2. Initialiser schèmes par défaut" << std::endl;
        std::cout << "3. Ajouter un schème" << std::endl;
        std::cout << "4. Modifier un schème" << std::endl;
        std::cout << "5. Supprimer un schème" << std::endl;
        std::cout << "0. Retour au menu principal" << std::endl;
        
        int choice = Utils::getChoice(0, 5);
        
        switch (choice) {
            case 1:
                displayPatternsMenu();
                break;
            case 2:
                initializePatternsMenu();
                break;
            case 3:
                addPatternMenu();
                break;
            case 4:
                updatePatternMenu();
                break;
            case 5:
                removePatternMenu();
                break;
            case 0:
                return;
        }
        
        Utils::waitForKeypress();
    }
}

// ============================================================================
// MENU GÉNÉRATION
// ============================================================================

void generationMenu() {
    while (true) {
        std::cout << "\n┌────────────────────────────────────────┐\n";
        std::cout << "│   GÉNÉRATION MORPHOLOGIQUE             │\n";
        std::cout << "└────────────────────────────────────────┘\n\n";
        
        std::cout << "1. Générer un mot dérivé" << std::endl;
        std::cout << "2. Générer tous les dérivés d'une racine" << std::endl;
        std::cout << "3. Afficher les dérivés d'une racine" << std::endl;
        std::cout << "0. Retour au menu principal" << std::endl;
        
        int choice = Utils::getChoice(0, 3);
        
        switch (choice) {
            case 1:
                generateDerivedWordMenu();
                break;
            case 2:
                generateAllDerivativesMenu();
                break;
            case 3:
                displayDerivedWordsMenu();
                break;
            case 0:
                return;
        }
        
        Utils::waitForKeypress();
    }
}

// ============================================================================
// MENU VALIDATION
// ============================================================================

void validationMenu() {
    while (true) {
        std::cout << "\n┌────────────────────────────────────────┐\n";
        std::cout << "│   VALIDATION MORPHOLOGIQUE             │\n";
        std::cout << "└────────────────────────────────────────┘\n\n";
        
        std::cout << "1. Valider un mot (mot + racine attendue)" << std::endl;
        std::cout << "2. Trouver la racine d'un mot" << std::endl;
        std::cout << "0. Retour au menu principal" << std::endl;
        
        int choice = Utils::getChoice(0, 2);
        
        switch (choice) {
            case 1:
                validateWordMenu();
                break;
            case 2:
                findRootOfWordMenu();
                break;
            case 0:
                return;
        }
        
        Utils::waitForKeypress();
    }
}

// ============================================================================
// MENU PRINCIPAL
// ============================================================================

int main() {
    try {
        Utils::printHeader();
        
        std::cout << "\n✓ Moteur morphologique initialisé." << std::endl;

        // Chargement des racines : Collecte → Encodage → Tri → Insertion médiane
        const std::string rootsFile = "data/roots.txt";
        if (Utils::fileExists(rootsFile)) {
            std::vector<std::string> roots = Utils::loadRootsFromFile(rootsFile);
            std::vector<std::string> validRoots;
            for (const auto& r : roots) {
                if (Utils::isValidArabicRoot(r)) {
                    validRoots.push_back(r);
                }
            }
            if (!validRoots.empty()) {
                engine.loadRootsBalanced(validRoots);
            }
        }
        
        // Boucle principale
        while (true) {
            Utils::printMainMenu();
            
            int choice = Utils::getChoice(0, 5);
            
            switch (choice) {
                case 1:
                    rootsMenu();
                    break;
                case 2:
                    patternsMenu();
                    break;
                case 3:
                    generationMenu();
                    break;
                case 4:
                    validationMenu();
                    break;
                case 5:
                    displayStatisticsMenu();
                    Utils::waitForKeypress();
                    break;
                case 0:
                    std::cout << "\n✓ Merci d'avoir utilisé le moteur morphologique. Au revoir!\n" << std::endl;
                    return 0;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Erreur fatale: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
