#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "structs.h"
#include <iostream>

/**
 * ============================================================================
 * TABLE DE HACHAGE POUR LA GESTION DES SCHÈMES MORPHOLOGIQUES
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 * 
 * Méthode : Adressage ouvert avec sondage linéaire
 * Fonction de hachage : Fonction simple basée sur les caractères
 * 
 * Complexité:
 * - Insertion : O(1) en moyenne, O(n) pire cas
 * - Recherche : O(1) en moyenne, O(n) pire cas
 * - Suppression : O(1) en moyenne, O(n) pire cas
 */

class ArabicHashFunction {
public:
    /**
     * Fonction de hachage simple pour chaînes (y compris arabes)
     * Utilise la technique djb2 (algorithme simple et efficace)
     * Complexité : O(k) où k = longueur de la clé
     */
    static unsigned int hash(const std::string& key) {
        unsigned int hash = 5381;  // Constante djb2
        
        for (unsigned char c : key) {
            // hash * 33 + c
            hash = ((hash << 5) + hash) + c;
        }
        
        return hash;
    }
};

/**
 * Table de hachage pour les schèmes morphologiques
 * Utilise le sondage linéaire pour résoudre les collisions
 */
class PatternHashTable {
private:
    Pattern table[50];          // Tableau des schèmes
    bool occupied[50];          // Indicateurs d'occupation
    int size;                   // Nombre d'éléments actuels
    static const int CAPACITY = 50;
    
    /**
     * Fonction de hachage locale
     * Complexité : O(k)
     */
    unsigned int hashFunction(const std::string& key) {
        return ArabicHashFunction::hash(key) % CAPACITY;
    }
    
public:
    // ========================================================================
    // CONSTRUCTEUR
    // ========================================================================
    
    PatternHashTable() : size(0) {
        for (int i = 0; i < CAPACITY; i++) {
            occupied[i] = false;
        }
    }
    
    // ========================================================================
    // OPÉRATIONS PUBLIQUES
    // ========================================================================
    
    /**
     * Insère un schème morphologique dans la table
     * Utilise le sondage linéaire pour gérer les collisions
     * Complexité : O(1) en moyenne, O(n) pire cas
     */
    bool insert(const Pattern& pattern) {
        if (size >= CAPACITY) {
            std::cerr << "Erreur: Table pleine! Impossible d'insérer." << std::endl;
            return false;
        }
        
        unsigned int index = hashFunction(pattern.name);
        
        // Sondage linéaire : chercher la première position libre
        int collisions = 0;
        while (occupied[index] && collisions < CAPACITY) {
            // Vérifier si la clé existe déjà
            if (table[index].name == pattern.name) {
                std::cout << "Schème '" << pattern.name << "' existe déjà. Mise à jour." << std::endl;
                table[index] = pattern;
                return true;
            }
            
            index = (index + 1) % CAPACITY;
            collisions++;
        }
        
        if (collisions >= CAPACITY) {
            std::cerr << "Erreur: Table pleine après sondage linéaire." << std::endl;
            return false;
        }
        
        // Insérer le schème
        table[index] = pattern;
        occupied[index] = true;
        size++;
        
        return true;
    }
    
    /**
     * Recherche un schème par son nom
     * Utilise le sondage linéaire
     * Complexité : O(1) en moyenne, O(n) pire cas
     */
    Pattern* search(const std::string& patternName) {
        unsigned int index = hashFunction(patternName);
        int probes = 0;
        
        // Sondage linéaire
        while (occupied[index] && probes < CAPACITY) {
            if (table[index].name == patternName) {
                return &table[index];
            }
            
            index = (index + 1) % CAPACITY;
            probes++;
        }
        
        return nullptr;  // Non trouvé
    }
    
    /**
     * Vérifie si un schème existe
     * Complexité : O(1) en moyenne
     */
    bool contains(const std::string& patternName) {
        return search(patternName) != nullptr;
    }
    
    /**
     * Supprime un schème par son nom
     * Complexité : O(1) en moyenne
     */
    bool remove(const std::string& patternName) {
        unsigned int index = hashFunction(patternName);
        int probes = 0;
        
        // Trouver le schème
        while (occupied[index] && probes < CAPACITY) {
            if (table[index].name == patternName) {
                occupied[index] = false;
                size--;
                
                // Réinsérer les éléments suivants pour mantenir l'intégrité
                reorganizeAfterDeletion(index);
                return true;
            }
            
            index = (index + 1) % CAPACITY;
            probes++;
        }
        
        return false;  // Non trouvé
    }
    
    /**
     * Réorganise la table après suppression
     * Pour maintenir la cohérence du sondage linéaire
     * Complexité : O(n) dans le pire cas
     */
    void reorganizeAfterDeletion(int deletedIndex) {
        int index = (deletedIndex + 1) % CAPACITY;
        
        while (occupied[index]) {
            Pattern temp = table[index];
            occupied[index] = false;
            size--;
            
            // Réinsérer l'élément
            insert(temp);
            
            index = (index + 1) % CAPACITY;
        }
    }
    
    /**
     * Retourne le nombre de schèmes dans la table
     */
    int getSize() const {
        return size;
    }
    
    /**
     * Retourne la capacité maximale
     */
    int getCapacity() const {
        return CAPACITY;
    }
    
    /**
     * Retourne le facteur de charge
     */
    double getLoadFactor() const {
        return static_cast<double>(size) / CAPACITY;
    }
    
    /**
     * Affiche tous les schèmes de la table
     * Complexité : O(n)
     */
    void displayAll() {
        if (size == 0) {
            std::cout << "Aucun schème dans la table." << std::endl;
            return;
        }
        
        std::cout << "\n=== Schèmes Morphologiques Stockés ===" << std::endl;
        std::cout << "Total: " << size << " schèmes (Capacité: " << CAPACITY << ")" << std::endl;
        std::cout << "Facteur de charge: " << (getLoadFactor() * 100) << "%" << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        int count = 0;
        for (int i = 0; i < CAPACITY; i++) {
            if (occupied[i]) {
                count++;
                std::cout << count << ". Nom: " << table[i].name 
                         << " | Structure: " << table[i].structure
                         << " | Description: " << table[i].description << std::endl;
            }
        }
        std::cout << std::string(70, '-') << std::endl;
    }
    
    /**
     * Retourne un pointeur vers un schème par index
     * (Utile pour les itérations)
     */
    Pattern* getPatternAt(int index) {
        if (index >= 0 && index < CAPACITY && occupied[index]) {
            return &table[index];
        }
        return nullptr;
    }
    
    /**
     * Retourne tous les schèmes dans un tableau
     * Complexité : O(n)
     */
    Pattern* getAllPatterns(int& count) {
        count = 0;
        Pattern* patterns = new Pattern[size];
        
        for (int i = 0; i < CAPACITY; i++) {
            if (occupied[i]) {
                patterns[count++] = table[i];
            }
        }
        
        return patterns;
    }
};

#endif // HASH_TABLE_H
