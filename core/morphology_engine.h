#ifndef MORPHOLOGY_ENGINE_H
#define MORPHOLOGY_ENGINE_H

#include "structs.h"
#include "bst_tree.h"
#include "hash_table.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>

class MorphologyEngine {
private:
    BSTree rootTree;
    PatternHashTable patternTable;

    void collectNodesInOrder(BSTNode* node, std::vector<BSTNode*>& out) const {
        if (node == nullptr) return;
        collectNodesInOrder(node->left, out);
        out.push_back(node);
        collectNodesInOrder(node->right, out);
    }
    
    // Génère un mot dérivé à partir d'une racine et d'un schème
    // Méthode dynamique : les lettres ف/ع/ل dans le nom du schème
    // indiquent les positions de C1/C2/C3 de la racine.
    // Tout autre caractère du schème est copié tel quel.
    std::string applyPatternTransformation(const std::string& root, const std::string& patternName) {
        Pattern* pattern = patternTable.search(patternName);
        
        if (pattern == nullptr) {
            return "";
        }
        std::vector<std::string> r = Utils::utf8Split(root);
        if (r.size() < 3) return "";
        
        std::vector<std::string> p = Utils::utf8Split(patternName);
        std::string derived = "";
        
        for (size_t i = 0; i < p.size(); i++) {
            if (p[i] == "\xd9\x81") {        // ف → C1
                derived += r[0];
            } else if (p[i] == "\xd8\xb9") { // ع → C2
                derived += r[1];
            } else if (p[i] == "\xd9\x84") { // ل → C3
                derived += r[2];
            } else {
                derived += p[i];              // caractère littéral
            }
        }
        
        return derived;
    }
    // Extrait une racine selon un schème connu
    // Méthode dynamique : compare le mot et le schème caractère par caractère
    // Les positions de ف/ع/ل dans le schème indiquent où trouver C1/C2/C3
    std::string extractRootFromWord(const std::string& word, const std::string& patternName) {
        std::vector<std::string> w = Utils::utf8Split(word);
        std::vector<std::string> p = Utils::utf8Split(patternName);
        
        // Le mot et le schème doivent avoir la même longueur en caractères
        if (w.size() != p.size()) return "";
        
        std::string c1 = "", c2 = "", c3 = "";
        
        for (size_t i = 0; i < p.size(); i++) {
            if (p[i] == "\xd9\x81") {        // ف → C1
                c1 = w[i];
            } else if (p[i] == "\xd8\xb9") { // ع → C2
                c2 = w[i];
            } else if (p[i] == "\xd9\x84") { // ل → C3
                c3 = w[i];
            }
        }
        
        // Vérifier que les 3 radicaux ont été trouvés
        if (c1.empty() || c2.empty() || c3.empty()) return "";
        return c1 + c2 + c3;
    }
    
public:
    MorphologyEngine() {}
    // Ajoute une racine à l’ABR
    void addRoot(const std::string& root) {
        if (rootTree.contains(root)) {
            std::cout << "Racine '" << root << "' existe déjà." << std::endl;
        } else {
            rootTree.insert(root);
            std::cout << "Racine '" << root << "' ajoutée avec succès." << std::endl;
        }
    }
    // Construit un ABR équilibré à partir d’une liste
    void loadRootsBalanced(std::vector<std::string>& roots) {
        rootTree.buildBalanced(roots);
        std::cout << "✓ ABR équilibré construit avec " << rootTree.getSize()
                  << " racines." << std::endl;
    }
    // Supprime une racine
    bool removeRoot(const std::string& root) {
        if (!rootTree.contains(root)) {
            return false;
        }
        rootTree.remove(root);
        return true;
    }
    BSTNode* findRoot(const std::string& root) {
        return rootTree.search(root);
    }
    bool rootExists(const std::string& root) {
        return rootTree.contains(root);
    }
    void displayAllRoots() {
        rootTree.displayAll();
    }
    bool addPattern(const Pattern& pattern) {
        return patternTable.insert(pattern);
    }

    bool updatePattern(const Pattern& pattern) {
        return patternTable.insert(pattern);
    }

    bool removePattern(const std::string& patternName) {
        return patternTable.remove(patternName);
    }
    Pattern* findPattern(const std::string& patternName) {
        return patternTable.search(patternName);
    }
    void displayAllPatterns() {
        patternTable.displayAll();
    }
    int getPatternCount() {
        return patternTable.getSize();
    }
    double getHashTableLoadFactor() {
        return patternTable.getLoadFactor();
    }
    // Génère un mot et l’ajoute à la liste des dérivés
    std::string generateDerivedWord(const std::string& root, const std::string& patternName) {
        if (!Utils::isValidArabicRoot(root)) {
            std::cout << "Erreur: Racine invalide (doit être trilitérale)." << std::endl;
            return "";
        }
        if (!rootTree.contains(root)) {
            std::cout << "Erreur: Racine '" << root << "' non trouvée." << std::endl;
            return "";
        }
        if (!patternTable.contains(patternName)) {
            std::cout << "Erreur: Schème '" << patternName << "' non trouvé." << std::endl;
            return "";
        }
        std::string derived = applyPatternTransformation(root, patternName);
        
        if (derived.empty()) {
            std::cout << "Erreur: Impossible de générer le mot." << std::endl;
            return "";
        }
        addDerivedWordToRoot(root, derived, patternName);
        
        return derived;
    }
    // Ajoute un dérivé à la liste de la racine
    bool addDerivedWordToRoot(const std::string& root, const std::string& word, const std::string& pattern) {
        BSTNode* node = rootTree.search(root);
        
        if (node == nullptr) {
            std::cout << "Erreur: Racine '" << root << "' non trouvée." << std::endl;
            return false;
        }
        DerivedWord* current = node->derivedList;
        while (current != nullptr) {
            if (current->word == word) {
                current->frequency++;
                return true;
            }
            current = current->next;
        }
        DerivedWord* newWord = new DerivedWord(word, pattern, 1);
        newWord->next = node->derivedList;
        node->derivedList = newWord;
        
        return true;
    }
    void displayDerivedWordsOfRoot(const std::string& root) {
        BSTNode* node = rootTree.search(root);
        
        if (node == nullptr) {
            std::cout << "Racine '" << root << "' non trouvée." << std::endl;
            return;
        }
        
        std::cout << "\n=== Mots Dérivés de la Racine: " << root << " ===" << std::endl;
        
        if (node->derivedList == nullptr) {
            std::cout << "Aucun mot dérivé." << std::endl;
            return;
        }
        
        int count = 0;
        DerivedWord* current = node->derivedList;
        while (current != nullptr) {
            count++;
            std::cout << count << ". Mot: " << current->word 
                     << " | Schème: " << current->pattern 
                     << " | Fréquence: " << current->frequency << std::endl;
            current = current->next;
        }
    }

    std::string getDerivedWordsText(const std::string& root) {
        BSTNode* node = rootTree.search(root);
        if (node == nullptr) {
            return "Racine non trouvée.";
        }
        std::string result = "\n📚 MOTS DÉRIVÉS GÉNÉRÉS :\n";
        result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
        
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        int count = 0;
        for (int i = 0; i < patternCount; i++) {
            std::string derived = applyPatternTransformation(root, patterns[i].name);
            if (!derived.empty()) {
                count++;
                result += std::to_string(count) + ". ";
                result += derived;
                result += "  ← ";
                result += patterns[i].name;
                result += "\n   (" + patterns[i].description + ")\n\n";
            }
        }
        
        if (count == 0) {
            result += "Aucun mot dérivé généré.\n";
        } else {
            result += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
            result += "Total : " + std::to_string(count) + " mots dérivés\n";
        }
        
        return result;
    }
    
    ValidationResult validateWord(const std::string& word, const std::string& expectedRoot) {
        if (!rootTree.contains(expectedRoot)) {
            return ValidationResult(false, "", "");
        }
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        for (int i = 0; i < patternCount; i++) {
            std::string extractedRoot = extractRootFromWord(word, patterns[i].name);
            
            if (extractedRoot == expectedRoot) {
                std::string foundPattern = patterns[i].name;
                delete[] patterns;
                return ValidationResult(true, foundPattern, expectedRoot);
            }
        }
        
        delete[] patterns;
        return ValidationResult(false, "", "");
    }
    ValidationResult findRootOfWord(const std::string& word) {
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        for (int i = 0; i < patternCount; i++) {
            std::string extractedRoot = extractRootFromWord(word, patterns[i].name);
            
            if (!extractedRoot.empty() && rootTree.contains(extractedRoot)) {
                std::string foundPattern = patterns[i].name;
                delete[] patterns;
                return ValidationResult(true, foundPattern, extractedRoot);
            }
        }
        
        delete[] patterns;
        return ValidationResult(false, "", "");
    }
    std::string getAllRootsAndDerivativesText() {
        std::vector<BSTNode*> nodes;
        collectNodesInOrder(rootTree.getRoot(), nodes);

        if (nodes.empty()) {
            return "Aucune racine dans l'arbre.";
        }

        std::string result;
        for (const auto* node : nodes) {
            result += "Racine: " + node->root + "\n";
            if (node->derivedList == nullptr) {
                result += "  (Aucun dérivé)\n";
            } else {
                DerivedWord* current = node->derivedList;
                while (current != nullptr) {
                    result += "  - " + current->word + " [" + current->pattern + "] (freq=" + std::to_string(current->frequency) + ")\n";
                    current = current->next;
                }
            }
            result += "\n";
        }

        return result;
    }

    std::string getAllRootsText() {
        std::vector<BSTNode*> nodes;
        collectNodesInOrder(rootTree.getRoot(), nodes);
        if (nodes.empty()) {
            return "Aucune racine dans l'arbre.";
        }

        std::string result;
        int count = 0;
        for (const auto* node : nodes) {
            count++;
            result += std::to_string(count) + ". " + node->root + "\n";
        }
        return result;
    }

    std::string getAllPatternsText() {
        int count = 0;
        Pattern* patterns = patternTable.getAllPatterns(count);
        if (count == 0) {
            delete[] patterns;
            return "Aucun schème dans la table.";
        }

        std::string result;
        for (int i = 0; i < count; i++) {
            result += std::to_string(i + 1) + ". " + patterns[i].name + " | " + patterns[i].structure + " | " + patterns[i].description + "\n";
        }
        delete[] patterns;
        return result;
    }

    int getRootCount() {
        return rootTree.getSize();
    }
    void generateAllDerivativesForRoot(const std::string& root) {
        if (!rootTree.contains(root)) {
            std::cout << "Erreur: Racine '" << root << "' non trouvée." << std::endl;
            return;
        }
        
        std::cout << "\n=== Génération de tous les dérivés pour: " << root << " ===" << std::endl;
        
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        for (int i = 0; i < patternCount; i++) {
            std::string derived = generateDerivedWord(root, patterns[i].name);
            if (!derived.empty()) {
                // generateDerivedWord() ajoute déjà le dérivé à la liste
                std::cout << "  + " << patterns[i].name << " => " << derived << std::endl;
            }
        }
        
        delete[] patterns;
    }
};

#endif // MORPHOLOGY_ENGINE_H
