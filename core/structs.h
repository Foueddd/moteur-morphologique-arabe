#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

/**
 * ============================================================================
 * STRUCTURES DE DONNÉES PRINCIPALES
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 */

// ============================================================================
// 1. STRUCTURE POUR UN MOT DÉRIVÉ VALIDÉ (Liste chaînée)
// ============================================================================
struct DerivedWord {
    std::string word;           // Mot arabe dérivé (ex: مكتوب)
    std::string pattern;        // Schème utilisé (ex: مفعول)
    int frequency;              // Fréquence d'apparition
    DerivedWord* next;          // Pointeur vers le mot suivant
    
    // Constructeur
    DerivedWord(const std::string& w, const std::string& p, int f = 1)
        : word(w), pattern(p), frequency(f), next(nullptr) {}
};

// ============================================================================
// 2. STRUCTURE POUR UN NŒUD DE L'ARBRE BINAIRE DE RECHERCHE (ABR)
// ============================================================================
struct BSTNode {
    std::string root;           // Racine arabe (ex: كتب)
    int height;                 // Hauteur du nœud (pour information)
    DerivedWord* derivedList;   // Liste chaînée des mots dérivés
    BSTNode* left;              // Pointeur fils gauche
    BSTNode* right;             // Pointeur fils droit
    
    // Constructeur
    BSTNode(const std::string& r)
        : root(r), height(1), derivedList(nullptr), left(nullptr), right(nullptr) {}
};

// ============================================================================
// 3. STRUCTURE POUR UN SCHÈME MORPHOLOGIQUE
// ============================================================================
struct Pattern {
    std::string name;           // Nom du schème (ex: مفعول, فاعل)
    std::string structure;      // Structure abstraite (ex: CVCCVC)
    std::string description;    // Description (ex: "Participe passé")
    
    // Constructeur
    Pattern()
        : name(""), structure(""), description("") {}
    
    Pattern(const std::string& n, const std::string& s, const std::string& d)
        : name(n), structure(s), description(d) {}
};

// ============================================================================
// 4. TABLE DE HACHAGE POUR LES SCHÈMES
// ============================================================================
struct HashTable {
    static const int CAPACITY = 50;  // Capacité initiale
    
    Pattern table[CAPACITY];         // Tableau des schèmes
    bool occupied[CAPACITY];         // Indicateurs d'occupation
    int size;                        // Nombre d'éléments actuels
    
    // Constructeur
    HashTable() : size(0) {
        for (int i = 0; i < CAPACITY; i++) {
            occupied[i] = false;
        }
    }
};

// ============================================================================
// 5. STRUCTURE POUR STOCKER UN RÉSULTAT DE VALIDATION
// ============================================================================
struct ValidationResult {
    bool isValid;               // OUI / NON
    std::string pattern;        // Schème trouvé (si valide)
    std::string root;           // Racine trouvée (si valide)
    
    // Constructeur
    ValidationResult(bool valid = false, const std::string& p = "", const std::string& r = "")
        : isValid(valid), pattern(p), root(r) {}
};

#endif // STRUCTS_H
