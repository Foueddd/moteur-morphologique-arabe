#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

// Mot dérivé associé à une racine (liste chaînée)
struct DerivedWord {
    std::string word;
    std::string pattern;
    int frequency;
    DerivedWord* next;

    DerivedWord(const std::string& w, const std::string& p, int f = 1)
        : word(w), pattern(p), frequency(f), next(nullptr) {}
};
// Nœud de l’ABR des racines
struct BSTNode {
    std::string root;
    int key;
    int height;
    DerivedWord* derivedList;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const std::string& r, int k)
        : root(r), key(k), height(1), derivedList(nullptr), left(nullptr), right(nullptr) {}
};
// Schème morphologique
struct Pattern {
    std::string name;
    std::string structure;
    std::string description;

    Pattern()
        : name(""), structure(""), description("") {}
    
    Pattern(const std::string& n, const std::string& s, const std::string& d)
        : name(n), structure(s), description(d) {}
};
// La table de hachage des schèmes est implémentée dans hash_table.h
// avec chaînage et rehash dynamique (classe PatternHashTable)
// Résultat de validation morphologique
struct ValidationResult {
    bool isValid;
    std::string pattern;
    std::string root;

    ValidationResult(bool valid = false, const std::string& p = "", const std::string& r = "")
        : isValid(valid), pattern(p), root(r) {}
};

#endif // STRUCTS_H
