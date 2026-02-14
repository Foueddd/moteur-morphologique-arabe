#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "structs.h"
#include <iostream>

class ArabicHashFunction {
public:
    // djb2 : hash simple pour chaînes UTF-8
    static unsigned int hash(const std::string& key) {
        unsigned int hash = 5381;
        
        for (unsigned char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        
        return hash;
    }
};
class PatternHashTable {
private:
    struct PatternNode {
        Pattern pattern;
        PatternNode* next;
        explicit PatternNode(const Pattern& p) : pattern(p), next(nullptr) {}
    };

    std::vector<PatternNode*> buckets;
    int size;
    static const int DEFAULT_CAPACITY = 50;
    static constexpr double MAX_LOAD_FACTOR = 0.75;
    // Index = hash % capacité
    unsigned int hashFunction(const std::string& key, int capacity) const {
        return ArabicHashFunction::hash(key) % capacity;
    }

    // Libère toutes les listes
    void clearBuckets() {
        for (auto* head : buckets) {
            PatternNode* current = head;
            while (current != nullptr) {
                PatternNode* next = current->next;
                delete current;
                current = next;
            }
        }
        buckets.clear();
    }

    // Redimensionne et réinsère tous les schèmes
    void rehash(int newCapacity) {
        std::vector<PatternNode*> newBuckets(static_cast<size_t>(newCapacity), nullptr);

        for (auto* head : buckets) {
            PatternNode* current = head;
            while (current != nullptr) {
                PatternNode* next = current->next;
                unsigned int index = hashFunction(current->pattern.name, newCapacity);

                current->next = newBuckets[index];
                newBuckets[index] = current;

                current = next;
            }
        }

        buckets = std::move(newBuckets);
    }
    
public:
    PatternHashTable() : buckets(DEFAULT_CAPACITY, nullptr), size(0) {}

    ~PatternHashTable() {
        clearBuckets();
    }
    
    // Insertion avec chaînage + rehash si facteur de charge dépasse le seuil
    bool insert(const Pattern& pattern) {
        if (buckets.empty()) {
            buckets.assign(DEFAULT_CAPACITY, nullptr);
        }

        unsigned int index = hashFunction(pattern.name, static_cast<int>(buckets.size()));
        PatternNode* current = buckets[index];
        while (current != nullptr) {
            if (current->pattern.name == pattern.name) {
                std::cout << "Schème '" << pattern.name << "' existe déjà. Mise à jour." << std::endl;
                current->pattern = pattern;
                return true;
            }
            current = current->next;
        }

        PatternNode* node = new PatternNode(pattern);
        node->next = buckets[index];
        buckets[index] = node;
        size++;

        if (getLoadFactor() > MAX_LOAD_FACTOR) {
            rehash(static_cast<int>(buckets.size()) * 2);
        }

        return true;
    }
    
    // Recherche dans la liste de la case
    Pattern* search(const std::string& patternName) {
        if (buckets.empty()) return nullptr;
        unsigned int index = hashFunction(patternName, static_cast<int>(buckets.size()));
        PatternNode* current = buckets[index];
        while (current != nullptr) {
            if (current->pattern.name == patternName) {
                return &current->pattern;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    bool contains(const std::string& patternName) {
        return search(patternName) != nullptr;
    }
    // Suppression dans la liste de la case
    bool remove(const std::string& patternName) {
        if (buckets.empty()) return false;
        unsigned int index = hashFunction(patternName, static_cast<int>(buckets.size()));
        PatternNode* current = buckets[index];
        PatternNode* prev = nullptr;

        while (current != nullptr) {
            if (current->pattern.name == patternName) {
                if (prev == nullptr) {
                    buckets[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                delete current;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }

        return false;
    }
    
    // Nombre de schèmes
    int getSize() const {
        return size;
    }
    // Capacité actuelle
    int getCapacity() const {
        return static_cast<int>(buckets.size());
    }
    // Facteur de charge = size / capacité
    double getLoadFactor() const {
        if (buckets.empty()) return 0.0;
        return static_cast<double>(size) / static_cast<double>(buckets.size());
    }
    void displayAll() {
        if (size == 0) {
            std::cout << "Aucun schème dans la table." << std::endl;
            return;
        }
        
        std::cout << "\n=== Schèmes Morphologiques Stockés ===" << std::endl;
        std::cout << "Total: " << size << " schèmes (Capacité: " << getCapacity() << ")" << std::endl;
        std::cout << "Facteur de charge: " << (getLoadFactor() * 100) << "%" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        int count = 0;
        for (size_t i = 0; i < buckets.size(); i++) {
            PatternNode* current = buckets[i];
            while (current != nullptr) {
                count++;
                std::cout << count << ". Nom: " << current->pattern.name
                          << " | Structure: " << current->pattern.structure
                          << " | Description: " << current->pattern.description << std::endl;
                current = current->next;
            }
        }
        std::cout << std::string(70, '-') << std::endl;
    }
    
    Pattern* getAllPatterns(int& count) {
        count = size;
        if (size == 0) {
            return new Pattern[0];
        }

        Pattern* patterns = new Pattern[size];
        int idx = 0;
        for (size_t i = 0; i < buckets.size(); i++) {
            PatternNode* current = buckets[i];
            while (current != nullptr) {
                patterns[idx++] = current->pattern;
                current = current->next;
            }
        }

        return patterns;
    }
};

#endif // HASH_TABLE_H
