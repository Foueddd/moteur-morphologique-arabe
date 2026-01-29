#ifndef MORPHOLOGY_ENGINE_H
#define MORPHOLOGY_ENGINE_H

#include "structs.h"
#include "avl_tree.h"
#include "hash_table.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * ============================================================================
 * MOTEUR DE DÉRIVATION ET VALIDATION MORPHOLOGIQUE
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 * 
 * Responsabilités:
 * 1. Génération de mots dérivés (racine + schème = mot)
 * 2. Validation morphologique (mot + racine = est-ce valide?)
 * 3. Gestion des transformations morphologiques
 */

class MorphologyEngine {
private:
    AVLTree rootTree;                  // Arbre AVL des racines
    PatternHashTable patternTable;     // Table de hachage des schèmes

    void collectNodesInOrder(AVLNode* node, std::vector<AVLNode*>& out) const {
        if (node == nullptr) return;
        collectNodesInOrder(node->left, out);
        out.push_back(node);
        collectNodesInOrder(node->right, out);
    }
    
    /**
     * Applique la transformation d'un schème simple
     * Exemple : كتب + مفعول = مكتوب
     * 
     * Structure des schèmes :
     * - فاعل : place la racine trilatérale avec voyelle 'a'
     * - مفعول : préfixe 'م' + racine + suffixe 'ول'
     * - افتعل : préfixe 'ا' + racine + suffixe 'ل'
     * - تفعيل : préfixe 'ت' + racine + suffixe 'يل'
     * 
     * Complexité : O(k) où k = longueur du mot généré
     */
    std::string applyPatternTransformation(const std::string& root, const std::string& patternName) {
        Pattern* pattern = patternTable.search(patternName);
        
        if (pattern == nullptr) {
            return "";  // Schème non trouvé
        }
        
        // Extraction des lettres de la racine trilatérale (UTF-8)
        std::vector<std::string> r = Utils::utf8Split(root);
        std::string derived = "";
        
        if (patternName == "فاعل") {
            // فاعل : Premier radical + 'a' + Second radical + Second radical
            if (r.size() >= 3) {
                derived = r[0];
                derived += "ا";
                derived += r[1];
                derived += r[2];
            }
        }
        else if (patternName == "مفعول") {
            // مفعول : 'م' + Premier radical + Second radical + 'و' + Troisième radical
            if (r.size() >= 3) {
                derived = "م";
                derived += r[0];
                derived += r[1];
                derived += "و";
                derived += r[2];
            }
        }
        else if (patternName == "افتعل") {
            // افتعل : 'ا' + Premier radical + 'ت' + Second radical + Troisième radical
            if (r.size() >= 3) {
                derived = "ا";
                derived += r[0];
                derived += "ت";
                derived += r[1];
                derived += r[2];
            }
        }
        else if (patternName == "تفعيل") {
            // تفعيل : 'ت' + Premier radical + Second radical + 'ي' + Troisième radical
            if (r.size() >= 3) {
                derived = "ت";
                derived += r[0];
                derived += r[1];
                derived += "ي";
                derived += r[2];
            }
        }
        else if (patternName == "مفعال") {
            // مفعال : 'م' + Premier radical + Second radical + 'ا' + Troisième radical
            if (r.size() >= 3) {
                derived = "م";
                derived += r[0];
                derived += r[1];
                derived += "ا";
                derived += r[2];
            }
        }
        else if (patternName == "فعال") {
            // فعال : Premier + Second + 'ا' + Troisième (pluriel)
            if (r.size() >= 3) {
                derived = r[0];
                derived += r[1];
                derived += "ا";
                derived += r[2];
            }
        }
        else {
            // Schème personnalisé : retourner le nom du schème comme exemple
            derived = pattern->name;
        }
        
        return derived;
    }
    
    /**
     * Inverse la transformation pour extraire la racine d'un mot
     * Exemple : مكتوب + مفعول = كتب
     * Complexité : O(k)
     */
    std::string extractRootFromWord(const std::string& word, const std::string& patternName) {
        std::string root = "";
        std::vector<std::string> w = Utils::utf8Split(word);
        
        if (patternName == "فاعل") {
            // فاعل : لآخر (mot = فاعل) => racine = الحروف 1, 3, 4
            if (w.size() >= 4) {
                root = w[0];
                root += w[2];
                root += w[3];
            }
        }
        else if (patternName == "مفعول") {
            // مفعول : 'م' + R1 + R2 + 'و' + R3
            if (w.size() >= 5) {
                root = w[1];
                root += w[2];
                root += w[4];
            }
        }
        else if (patternName == "افتعل") {
            // افتعل : 'ا' + R1 + 'ت' + R2 + R3
            if (w.size() >= 5) {
                root = w[1];
                root += w[3];
                root += w[4];
            }
        }
        else if (patternName == "تفعيل") {
            // تفعيل : 'ت' + R1 + R2 + 'ي' + R3
            if (w.size() >= 5) {
                root = w[1];
                root += w[2];
                root += w[4];
            }
        }
        else if (patternName == "مفعال") {
            // مفعال : 'م' + R1 + R2 + 'ا' + R3
            if (w.size() >= 5) {
                root = w[1];
                root += w[2];
                root += w[4];
            }
        }
        else if (patternName == "فعال") {
            // فعال : R1 + R2 + 'ا' + R3
            if (w.size() >= 4) {
                root = w[0];
                root += w[1];
                root += w[3];
            }
        }
        
        return root;
    }
    
public:
    // ========================================================================
    // CONSTRUCTEUR
    // ========================================================================
    
    MorphologyEngine() {}
    
    // ========================================================================
    // GESTION DES RACINES
    // ========================================================================
    
    /**
     * Ajoute une racine à l'arbre AVL
     * Complexité : O(log n)
     */
    void addRoot(const std::string& root) {
        if (rootTree.contains(root)) {
            std::cout << "Racine '" << root << "' existe déjà." << std::endl;
        } else {
            rootTree.insert(root);
            std::cout << "Racine '" << root << "' ajoutée avec succès." << std::endl;
        }
    }

    /**
     * Supprime une racine de l'arbre AVL
     * Complexité : O(log n)
     */
    bool removeRoot(const std::string& root) {
        if (!rootTree.contains(root)) {
            return false;
        }
        rootTree.remove(root);
        return true;
    }
    
    /**
     * Cherche une racine
     * Complexité : O(log n)
     */
    AVLNode* findRoot(const std::string& root) {
        return rootTree.search(root);
    }
    
    /**
     * Vérifie si une racine existe
     * Complexité : O(log n)
     */
    bool rootExists(const std::string& root) {
        return rootTree.contains(root);
    }
    
    /**
     * Affiche toutes les racines
     * Complexité : O(n)
     */
    void displayAllRoots() {
        rootTree.displayAll();
    }
    
    // ========================================================================
    // GESTION DES SCHÈMES
    // ========================================================================
    
    /**
     * Ajoute un schème morphologique à la table de hachage
     * Complexité : O(1) en moyenne
     */
    bool addPattern(const Pattern& pattern) {
        return patternTable.insert(pattern);
    }

    /**
     * Met à jour un schème (réinsertion)
     * Complexité : O(1) en moyenne
     */
    bool updatePattern(const Pattern& pattern) {
        return patternTable.insert(pattern);
    }

    /**
     * Supprime un schème
     * Complexité : O(1) en moyenne
     */
    bool removePattern(const std::string& patternName) {
        return patternTable.remove(patternName);
    }
    
    /**
     * Cherche un schème
     * Complexité : O(1) en moyenne
     */
    Pattern* findPattern(const std::string& patternName) {
        return patternTable.search(patternName);
    }
    
    /**
     * Affiche tous les schèmes
     * Complexité : O(n)
     */
    void displayAllPatterns() {
        patternTable.displayAll();
    }
    
    /**
     * Retourne le nombre de schèmes
     */
    int getPatternCount() {
        return patternTable.getSize();
    }
    
    /**
     * Retourne le facteur de charge de la table
     */
    double getHashTableLoadFactor() {
        return patternTable.getLoadFactor();
    }
    
    // ========================================================================
    // GÉNÉRATION MORPHOLOGIQUE
    // ========================================================================
    
    /**
     * Génère un mot dérivé à partir d'une racine et d'un schème
     * Complexité : O(log n + k) où n = racines, k = longueur du mot
     */
    std::string generateDerivedWord(const std::string& root, const std::string& patternName) {
        if (!Utils::isValidArabicRoot(root)) {
            std::cout << "Erreur: Racine invalide (doit être trilitérale)." << std::endl;
            return "";
        }
        // Vérifier que la racine existe
        if (!rootTree.contains(root)) {
            std::cout << "Erreur: Racine '" << root << "' non trouvée." << std::endl;
            return "";
        }
        
        // Vérifier que le schème existe
        if (!patternTable.contains(patternName)) {
            std::cout << "Erreur: Schème '" << patternName << "' non trouvé." << std::endl;
            return "";
        }
        
        // Générer le mot
        std::string derived = applyPatternTransformation(root, patternName);
        
        if (derived.empty()) {
            std::cout << "Erreur: Impossible de générer le mot." << std::endl;
            return "";
        }
        
        return derived;
    }
    
    /**
     * Ajoute un mot dérivé à la liste de la racine
     * Complexité : O(log n)
     */
    bool addDerivedWordToRoot(const std::string& root, const std::string& word, const std::string& pattern) {
        AVLNode* node = rootTree.search(root);
        
        if (node == nullptr) {
            std::cout << "Erreur: Racine '" << root << "' non trouvée." << std::endl;
            return false;
        }
        
        // Vérifier si le mot existe déjà
        DerivedWord* current = node->derivedList;
        while (current != nullptr) {
            if (current->word == word) {
                current->frequency++;
                return true;
            }
            current = current->next;
        }
        
        // Ajouter le mot en tête de liste
        DerivedWord* newWord = new DerivedWord(word, pattern, 1);
        newWord->next = node->derivedList;
        node->derivedList = newWord;
        
        return true;
    }
    
    /**
     * Affiche tous les mots dérivés d'une racine
     * Complexité : O(log n + m) où m = nombre de dérivés
     */
    void displayDerivedWordsOfRoot(const std::string& root) {
        AVLNode* node = rootTree.search(root);
        
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

    /**
     * Retourne la famille morphologique d'une racine sous forme textuelle
     * Complexité : O(log n + m)
     */
    std::string getDerivedWordsText(const std::string& root) {
        AVLNode* node = rootTree.search(root);
        if (node == nullptr) {
            return "Racine non trouvée.";
        }

        if (node->derivedList == nullptr) {
            return "Aucun mot dérivé.";
        }

        std::string result;
        int count = 0;
        DerivedWord* current = node->derivedList;
        while (current != nullptr) {
            count++;
            result += std::to_string(count) + ". ";
            result += current->word + " | Schème: " + current->pattern + " | Fréquence: " + std::to_string(current->frequency) + "\n";
            current = current->next;
        }
        return result;
    }
    
    // ========================================================================
    // VALIDATION MORPHOLOGIQUE
    // ========================================================================
    
    /**
     * Valide si un mot appartient morphologiquement à une racine donnée
     * Essaie tous les schèmes pour voir lequel reconstruit la racine
     * Complexité : O(m * k) où m = nombre de schèmes, k = longueur du mot
     */
    ValidationResult validateWord(const std::string& word, const std::string& expectedRoot) {
        // Vérifier que la racine existe
        if (!rootTree.contains(expectedRoot)) {
            return ValidationResult(false, "", "");
        }
        
        // Essayer chaque schème
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        for (int i = 0; i < patternCount; i++) {
            std::string extractedRoot = extractRootFromWord(word, patterns[i].name);
            
            if (extractedRoot == expectedRoot) {
                delete[] patterns;
                return ValidationResult(true, patterns[i].name, expectedRoot);
            }
        }
        
        delete[] patterns;
        return ValidationResult(false, "", "");
    }
    
    /**
     * Cherche la racine d'un mot en essayant tous les schèmes
     * Retourne la première racine trouvée
     * Complexité : O(m * n * k) où m = schèmes, n = racines, k = longueur
     */
    ValidationResult findRootOfWord(const std::string& word) {
        int patternCount = 0;
        Pattern* patterns = patternTable.getAllPatterns(patternCount);
        
        for (int i = 0; i < patternCount; i++) {
            std::string extractedRoot = extractRootFromWord(word, patterns[i].name);
            
            if (!extractedRoot.empty() && rootTree.contains(extractedRoot)) {
                delete[] patterns;
                return ValidationResult(true, patterns[i].name, extractedRoot);
            }
        }
        
        delete[] patterns;
        return ValidationResult(false, "", "");
    }

    /**
     * Retourne toutes les racines avec leurs dérivés (format texte)
     * Complexité : O(n + m)
     */
    std::string getAllRootsAndDerivativesText() {
        std::vector<AVLNode*> nodes;
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

    /**
     * Retourne la liste des racines (format texte)
     * Complexité : O(n)
     */
    std::string getAllRootsText() {
        std::vector<AVLNode*> nodes;
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

    /**
     * Retourne la liste des schèmes (format texte)
     * Complexité : O(n)
     */
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

    /**
     * Suggestions simples: racines contenant la requête
     * Complexité : O(n * k)
     */
    std::vector<std::string> suggestRoots(const std::string& query) {
        std::vector<std::string> suggestions;
        if (query.empty()) return suggestions;

        std::vector<AVLNode*> nodes;
        collectNodesInOrder(rootTree.getRoot(), nodes);
        for (const auto* node : nodes) {
            if (node->root.find(query) != std::string::npos) {
                suggestions.push_back(node->root);
            }
        }
        return suggestions;
    }
    
    // ========================================================================
    // UTILITAIRES
    // ========================================================================
    
    /**
     * Retourne le nombre total de racines
     */
    int getRootCount() {
        return rootTree.getSize();
    }
    
    /**
     * Génère et ajoute tous les dérivés d'une racine pour tous les schèmes
     * Complexité : O(log n + m * k)
     */
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
                addDerivedWordToRoot(root, derived, patterns[i].name);
                std::cout << "  + " << patterns[i].name << " => " << derived << std::endl;
            }
        }
        
        delete[] patterns;
    }
};

#endif // MORPHOLOGY_ENGINE_H
