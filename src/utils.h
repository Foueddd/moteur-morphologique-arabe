#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

/**
 * ============================================================================
 * FONCTIONS UTILITAIRES
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 */

namespace Utils {
    
    // ========================================================================
    // GESTION DES FICHIERS
    // ========================================================================
    
    /**
     * Charge les racines arabes à partir d'un fichier
     * Format du fichier: une racine par ligne
     * Complexité : O(n * k) où n = nombre de racines, k = longueur moyenne
     */
    inline std::vector<std::string> loadRootsFromFile(const std::string& filename) {
        std::vector<std::string> roots;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible d'ouvrir le fichier '" << filename << "'" << std::endl;
            return roots;
        }
        
        std::string root;
        while (std::getline(file, root)) {
            // Ignorer les lignes vides et les commentaires
            if (!root.empty() && root[0] != '#') {
                // Supprimer les espaces inutiles
                root.erase(0, root.find_first_not_of(" \t\r\n"));
                root.erase(root.find_last_not_of(" \t\r\n") + 1);
                
                if (!root.empty()) {
                    roots.push_back(root);
                }
            }
        }
        
        file.close();
        return roots;
    }
    
    /**
     * Sauvegarde les racines dans un fichier
     * Complexité : O(n)
     */
    inline bool saveRootsToFile(const std::string& filename, const std::vector<std::string>& roots) {
        std::ofstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Erreur: Impossible de créer le fichier '" << filename << "'" << std::endl;
            return false;
        }
        
        file << "# Fichier de racines arabes\n";
        file << "# Format: une racine par ligne\n";
        file << "# Généré automatiquement\n\n";
        
        for (const auto& root : roots) {
            file << root << "\n";
        }
        
        file.close();
        return true;
    }
    
    // ========================================================================
    // AFFICHAGE ET FORMATAGE
    // ========================================================================
    
    /**
     * Affiche une ligne de séparation
     */
    inline void printSeparator(int width = 70, char ch = '=') {
        for (int i = 0; i < width; i++) {
            std::cout << ch;
        }
        std::cout << std::endl;
    }
    
    /**
     * Affiche le titre du programme
     */
    inline void printHeader() {
        printSeparator(70, '=');
        std::cout << "   MOTEUR DE RECHERCHE MORPHOLOGIQUE ARABE" << std::endl;
        std::cout << "   Arbre AVL + Table de Hachage + Listes Chaînées" << std::endl;
        printSeparator(70, '=');
    }
    
    /**
     * Affiche un menu principal
     */
    inline void printMainMenu() {
        std::cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║           MENU PRINCIPAL                                         ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
        
        std::cout << "1. ▶ Gestion des racines" << std::endl;
        std::cout << "   1.1 Ajouter une racine" << std::endl;
        std::cout << "   1.2 Supprimer une racine" << std::endl;
        std::cout << "   1.3 Chercher une racine" << std::endl;
        std::cout << "   1.4 Afficher toutes les racines" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. ▶ Gestion des schèmes" << std::endl;
        std::cout << "   2.1 Afficher tous les schèmes" << std::endl;
        std::cout << "   2.2 Initialiser schèmes par défaut" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. ▶ Génération morphologique" << std::endl;
        std::cout << "   3.1 Générer un mot dérivé" << std::endl;
        std::cout << "   3.2 Générer tous les dérivés d'une racine" << std::endl;
        std::cout << "   3.3 Afficher les dérivés d'une racine" << std::endl;
        std::cout << std::endl;
        
        std::cout << "4. ▶ Validation morphologique" << std::endl;
        std::cout << "   4.1 Valider un mot (racine + mot)" << std::endl;
        std::cout << "   4.2 Trouver la racine d'un mot" << std::endl;
        std::cout << std::endl;
        
        std::cout << "5. ▶ Afficher les statistiques" << std::endl;
        std::cout << std::endl;
        
        std::cout << "0. ▶ Quitter" << std::endl;
        std::cout << "\n";
    }
    
    /**
     * Affiche les statistiques du système
     */
    inline void printStatistics(int rootCount, int patternCount, double loadFactor) {
        printSeparator(70, '-');
        std::cout << "STATISTIQUES DU SYSTÈME" << std::endl;
        printSeparator(70, '-');
        
        std::cout << "Nombre de racines (Arbre AVL): " << rootCount << std::endl;
        std::cout << "Nombre de schèmes (Table Hash): " << patternCount << std::endl;
        std::cout << "Facteur de charge (Hash): " << (loadFactor * 100) << "%" << std::endl;
        
        printSeparator(70, '-');
    }
    
    // ========================================================================
    // CONVERSION ET VÉRIFICATION DE CHAÎNES
    // ========================================================================

    /**
     * Découpe une chaîne UTF-8 en unités (codepoints) sous forme de strings.
     * Complexité : O(n) où n = nombre d'octets
     */
    inline std::vector<std::string> utf8Split(const std::string& str) {
        std::vector<std::string> result;
        size_t i = 0;
        while (i < str.size()) {
            unsigned char c = static_cast<unsigned char>(str[i]);
            size_t len = 1;

            if ((c & 0x80) == 0x00) {
                len = 1;
            } else if ((c & 0xE0) == 0xC0) {
                len = 2;
            } else if ((c & 0xF0) == 0xE0) {
                len = 3;
            } else if ((c & 0xF8) == 0xF0) {
                len = 4;
            } else {
                len = 1; // octet invalide, traiter comme un caractère
            }

            if (i + len > str.size()) {
                len = 1;
            }

            result.push_back(str.substr(i, len));
            i += len;
        }
        return result;
    }

    /**
     * Retourne la longueur UTF-8 (nombre de caractères)
     * Complexité : O(n)
     */
    inline size_t utf8Length(const std::string& str) {
        return utf8Split(str).size();
    }
    
    /**
     * Demande et récupère une entrée utilisateur
     */
    inline std::string getInput(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);
        
        // Supprimer les espaces inutiles
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        return input;
    }
    
    /**
     * Demande un choix numérique
     */
    inline int getChoice(int minOption, int maxOption) {
        int choice = -1;
        std::string input;
        
        while (true) {
            std::cout << "Votre choix [" << minOption << "-" << maxOption << "]: ";
            std::getline(std::cin, input);
            
            try {
                choice = std::stoi(input);
                if (choice >= minOption && choice <= maxOption) {
                    return choice;
                } else {
                    std::cout << "Erreur: Choix invalide. Réessayez." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cout << "Erreur: Entrée invalide. Réessayez." << std::endl;
            }
        }
    }
    
    /**
     * Vérifie si une chaîne est une racine trilitérale valide
     * (Très simplifiée - vérification de longueur basique)
     */
    inline bool isValidArabicRoot(const std::string& root) {
        // Vérification basique : racine trilitérale (3 caractères UTF-8)
        return utf8Length(root) == 3;
    }
    
    // ========================================================================
    // AFFICHAGE FORMATÉ POUR RÉSULTATS
    // ========================================================================
    
    /**
     * Affiche un résultat de validation
     */
    inline void printValidationResult(const std::string& word, const std::string& root, bool isValid, const std::string& pattern) {
        std::cout << "\n";
        printSeparator(70, '-');
        std::cout << "RÉSULTAT DE VALIDATION" << std::endl;
        printSeparator(70, '-');
        
        std::cout << "Mot: " << word << std::endl;
        std::cout << "Racine: " << root << std::endl;
        std::cout << "Valide: ";
        
        if (isValid) {
            std::cout << "✓ OUI" << std::endl;
            std::cout << "Schème trouvé: " << pattern << std::endl;
        } else {
            std::cout << "✗ NON" << std::endl;
            std::cout << "Le mot n'appartient pas morphologiquement à cette racine." << std::endl;
        }
        
        printSeparator(70, '-');
    }
    
    /**
     * Affiche un résultat de génération
     */
    inline void printGenerationResult(const std::string& root, const std::string& pattern, const std::string& derived) {
        std::cout << "\n";
        printSeparator(70, '-');
        std::cout << "RÉSULTAT DE GÉNÉRATION" << std::endl;
        printSeparator(70, '-');
        
        std::cout << "Racine: " << root << std::endl;
        std::cout << "Schème: " << pattern << std::endl;
        std::cout << "Mot généré: " << derived << std::endl;
        
        printSeparator(70, '-');
    }
    
    // ========================================================================
    // UTILITAIRES DE FICHIER
    // ========================================================================
    
    /**
     * Vérifie si un fichier existe
     */
    inline bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
    
    /**
     * Affiche un message d'attente
     */
    inline void waitForKeypress() {
        std::cout << "\nAppuyez sur Entrée pour continuer...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }
}

#endif // UTILS_H
