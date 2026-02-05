#ifndef BST_TREE_H
#define BST_TREE_H

#include "structs.h"
#include <algorithm>
#include <iostream>

/**
 * ============================================================================
 * IMPLÉMENTATION DE L'ARBRE BINAIRE DE RECHERCHE (ABR) SIMPLE
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 * 
 * ABR CLASSIQUE SANS ÉQUILIBRAGE
 * 
 * Complexité:
 * - Insertion : O(h) où h = hauteur (O(n) pire cas, O(log n) cas moyen)
 * - Recherche : O(h) où h = hauteur (O(n) pire cas, O(log n) cas moyen)
 * - Suppression : O(h) où h = hauteur
 * - Parcours : O(n)
 */

class BSTree {
private:
    BSTNode* root;
    
    // ========================================================================
    // FONCTIONS UTILITAIRES PRIVÉES POUR ABR SIMPLE
    // ========================================================================
    
    /**
     * Insère une racine dans le sous-arbre (ABR SIMPLE - SANS ÉQUILIBRAGE)
     * Complexité : O(h) où h = hauteur de l'arbre
     */
    BSTNode* insertNode(BSTNode* node, const std::string& rootKey) {
        // Insertion standard BST (sans équilibrage)
        if (node == nullptr) {
            return new BSTNode(rootKey);
        }
        
        if (rootKey < node->root) {
            node->left = insertNode(node->left, rootKey);
        } else if (rootKey > node->root) {
            node->right = insertNode(node->right, rootKey);
        } else {
            // Racine existe déjà
            return node;
        }
        
        // 2. Mettre à jour la hauteur (pour information seulement)
        int leftHeight = (node->left != nullptr) ? node->left->height : 0;
        int rightHeight = (node->right != nullptr) ? node->right->height : 0;
        node->height = 1 + std::max(leftHeight, rightHeight);
        
        // PAS D'ÉQUILIBRAGE - C'EST UN ABR SIMPLE !
        return node;
    }
    
    /**
     * Recherche une racine dans le sous-arbre
     * Complexité : O(h) où h = hauteur (O(n) pire cas, O(log n) cas moyen)
     */
    BSTNode* searchNode(BSTNode* node, const std::string& rootKey) {
        if (node == nullptr) {
            return nullptr;
        }
        
        if (rootKey == node->root) {
            return node;
        } else if (rootKey < node->root) {
            return searchNode(node->left, rootKey);
        } else {
            return searchNode(node->right, rootKey);
        }
    }
    
    /**
     * Libère la mémoire d'une liste de dérivés
     * Complexité : O(m)
     */
    void deleteDerivedList(DerivedWord* list) {
        DerivedWord* current = list;
        while (current != nullptr) {
            DerivedWord* next = current->next;
            delete current;
            current = next;
        }
    }

    /**
     * Libère la mémoire d'un sous-arbre
     * Complexité : O(n)
     */
    void deleteTree(BSTNode* node) {
        if (node == nullptr) return;
        
        deleteTree(node->left);
        deleteTree(node->right);
        
        deleteDerivedList(node->derivedList);
        delete node;
    }

    /**
     * Retourne le nœud avec la plus petite clé (successeur)
     * Complexité : O(log n)
     */
    BSTNode* minValueNode(BSTNode* node) {
        BSTNode* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    /**
     * Supprime une racine du sous-arbre (ABR SIMPLE - SANS RÉÉQUILIBRAGE)
     * Complexité : O(h)
     */
    BSTNode* deleteNode(BSTNode* node, const std::string& rootKey) {
        if (node == nullptr) {
            return node;
        }

        if (rootKey < node->root) {
            node->left = deleteNode(node->left, rootKey);
        } else if (rootKey > node->root) {
            node->right = deleteNode(node->right, rootKey);
        } else {
            // Nœud trouvé

            // Cas 1: aucun enfant ou 1 seul enfant
            if (node->left == nullptr || node->right == nullptr) {
                BSTNode* child = (node->left != nullptr) ? node->left : node->right;

                if (child == nullptr) {
                    // Aucun enfant
                    deleteDerivedList(node->derivedList);
                    delete node;
                    node = nullptr;
                } else {
                    // Un seul enfant: remplacer le nœud par son enfant
                    BSTNode* old = node;
                    node = child;
                    deleteDerivedList(old->derivedList);
                    delete old;
                }
            } else {
                // Cas 2: deux enfants
                // On prend le successeur (min du sous-arbre droit)
                BSTNode* succ = minValueNode(node->right);

                // Copier la clé du successeur dans le nœud courant
                node->root = succ->root;

                // Échanger la liste dérivée (pour garder la cohérence)
                std::swap(node->derivedList, succ->derivedList);

                // Supprimer le successeur (IMPORTANT : supprimer succ->root)
                node->right = deleteNode(node->right, succ->root);
            }
        }

        if (node == nullptr) return node;

        // Mettre à jour la hauteur (pour information uniquement)
        int leftHeight = (node->left != nullptr) ? node->left->height : 0;
        int rightHeight = (node->right != nullptr) ? node->right->height : 0;
        node->height = 1 + std::max(leftHeight, rightHeight);

        // PAS DE RÉÉQUILIBRAGE - C'EST UN ABR SIMPLE !
        return node;
    }
    
    /**
     * Parcours en ordre (InOrder) : affiche les racines triées
     * Complexité : O(n)
     */
    void inorderTraversal(BSTNode* node) {
        if (node == nullptr) return;
        
        inorderTraversal(node->left);
        
        std::cout << "  Racine: " << node->root << " (Hauteur: " << node->height << ")" << std::endl;
        
        // Afficher les mots dérivés
        if (node->derivedList != nullptr) {
            std::cout << "    Dérivés : ";
            DerivedWord* current = node->derivedList;
            while (current != nullptr) {
                std::cout << current->word << " [" << current->pattern << "] ";
                current = current->next;
            }
            std::cout << std::endl;
        }
        
        inorderTraversal(node->right);
    }
    
public:
    // ========================================================================
    // CONSTRUCTEUR ET DESTRUCTEUR
    // ========================================================================
    
    BSTree() : root(nullptr) {}
    
    ~BSTree() {
        deleteTree(root);
    }
    
    // ========================================================================
    // OPÉRATIONS PUBLIQUES
    // ========================================================================
    
    /**
     * Insère une nouvelle racine dans l'arbre ABR (sans équilibrage)
     * Complexité : O(h) où h = hauteur
     */
    void insert(const std::string& rootStr) {
        root = insertNode(root, rootStr);
    }
    
    /**
     * Recherche une racine et retourne le nœud
     * Complexité : O(h)
     */
    BSTNode* search(const std::string& rootStr) {
        return searchNode(root, rootStr);
    }
    
    /**
     * Vérifie si une racine existe
     * Complexité : O(h)
     */
    bool contains(const std::string& rootStr) {
        return searchNode(root, rootStr) != nullptr;
    }
    
    /**
     * Retourne le nombre de nœuds (racines)
     * Complexité : O(n)
     */
    int countNodes(BSTNode* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }
    
    int getSize() {
        return countNodes(root);
    }
    
    /**
     * Affiche toutes les racines avec la structure de l'arbre
     * Complexité : O(n)
     */
    void displayAll() {
        if (root == nullptr) {
            std::cout << "Aucune racine dans l'arbre." << std::endl;
            return;
        }
        std::cout << "\n=== Affichage des Racines (En-Ordre) ===" << std::endl;
        inorderTraversal(root);
        std::cout << "\nTotal de racines : " << getSize() << std::endl;
    }
    
    /**
     * Retourne le nœud racine de l'arbre
     */
    BSTNode* getRoot() {
        return root;
    }

    /**
     * Supprime une racine de l'arbre ABR (sans rééquilibrage)
     * Complexité : O(h)
     */
    void remove(const std::string& rootStr) {
        root = deleteNode(root, rootStr);
    }
};

#endif // BST_TREE_H
