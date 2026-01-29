#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "structs.h"
#include <algorithm>
#include <iostream>

/**
 * ============================================================================
 * IMPLÉMENTATION DE L'ARBRE AVL POUR LA GESTION DES RACINES
 * Moteur de Recherche Morphologique Arabe
 * ============================================================================
 * 
 * Complexité:
 * - Insertion : O(log n)
 * - Recherche : O(log n)
 * - Suppression : O(log n)
 * - Parcours : O(n)
 */

class AVLTree {
private:
    AVLNode* root;
    
    // ========================================================================
    // FONCTIONS UTILITAIRES PRIVÉES
    // ========================================================================
    
    /**
     * Retourne la hauteur d'un nœud
     * Complexité : O(1)
     */
    int getHeight(AVLNode* node) {
        return (node == nullptr) ? 0 : node->height;
    }
    
    /**
     * Calcule le facteur d'équilibre (différence de hauteur)
     * Complexité : O(1)
     */
    int getBalanceFactor(AVLNode* node) {
        return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
    }
    
    /**
     * Met à jour la hauteur d'un nœud
     * Complexité : O(1)
     */
    void updateHeight(AVLNode* node) {
        if (node != nullptr) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }
    
    /**
     * ROTATION DROITE
     *          y                           x
     *         / \                         / \
     *        x   C      ====>             A   y
     *       / \                             / \
     *      A   B                           B   C
     * 
     * Cas LL : Nouveau nœud inséré dans l'enfant gauche du fils gauche
     * Complexité : O(1)
     */
    AVLNode* rotateRight(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* B = x->right;
        
        // Effectuer la rotation
        x->right = y;
        y->left = B;
        
        // Mettre à jour les hauteurs
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    /**
     * ROTATION GAUCHE
     *        x                           y
     *       / \                         / \
     *      A   y      ====>             x   C
     *         / \                     / \
     *        B   C                   A   B
     * 
     * Cas RR : Nouveau nœud inséré dans l'enfant droit du fils droit
     * Complexité : O(1)
     */
    AVLNode* rotateLeft(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* B = y->left;
        
        // Effectuer la rotation
        y->left = x;
        x->right = B;
        
        // Mettre à jour les hauteurs
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    /**
     * Insère une racine dans le sous-arbre enraciné au nœud
     * Effectue automatiquement l'équilibrage AVL
     * Complexité : O(log n)
     */
    AVLNode* insertNode(AVLNode* node, const std::string& root) {
        // 1. Insertion standard de BST
        if (node == nullptr) {
            return new AVLNode(root);
        }
        
        if (root < node->root) {
            node->left = insertNode(node->left, root);
        } else if (root > node->root) {
            node->right = insertNode(node->right, root);
        } else {
            // Racine existe déjà
            return node;
        }
        
        // 2. Mettre à jour la hauteur du nœud courant
        updateHeight(node);
        
        // 3. Obtenir le facteur d'équilibre
        int balance = getBalanceFactor(node);
        
        // 4. Effectuer les rotations si nécessaire
        
        // Cas LL : Déséquilibre à gauche-gauche
        if (balance > 1 && root < node->left->root) {
            return rotateRight(node);
        }
        
        // Cas RR : Déséquilibre à droite-droite
        if (balance < -1 && root > node->right->root) {
            return rotateLeft(node);
        }
        
        // Cas LR : Déséquilibre à gauche-droite
        if (balance > 1 && root > node->left->root) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Cas RL : Déséquilibre à droite-gauche
        if (balance < -1 && root < node->right->root) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    /**
     * Recherche une racine dans le sous-arbre
     * Complexité : O(log n)
     */
    AVLNode* searchNode(AVLNode* node, const std::string& root) {
        if (node == nullptr) {
            return nullptr;
        }
        
        if (root == node->root) {
            return node;
        } else if (root < node->root) {
            return searchNode(node->left, root);
        } else {
            return searchNode(node->right, root);
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
    void deleteTree(AVLNode* node) {
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
    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    /**
     * Supprime une racine du sous-arbre et rééquilibre
     * Complexité : O(log n)
     */
    AVLNode* deleteNode(AVLNode* node, const std::string& rootKey) {
        if (node == nullptr) {
            return node;
        }

        if (rootKey < node->root) {
            node->left = deleteNode(node->left, rootKey);
        } else if (rootKey > node->root) {
            node->right = deleteNode(node->right, rootKey);
        } else {
            // Nœud à supprimer trouvé
            if (node->left == nullptr || node->right == nullptr) {
                AVLNode* temp = node->left ? node->left : node->right;

                if (temp == nullptr) {
                    // Aucun enfant
                    deleteDerivedList(node->derivedList);
                    delete node;
                    node = nullptr;
                } else {
                    // Un seul enfant: remplacer le nœud par son enfant
                    AVLNode* old = node;
                    node = temp;
                    deleteDerivedList(old->derivedList);
                    delete old;
                }
            } else {
                // Deux enfants: utiliser le successeur
                AVLNode* temp = minValueNode(node->right);
                std::swap(node->root, temp->root);
                std::swap(node->derivedList, temp->derivedList);
                node->right = deleteNode(node->right, rootKey);
            }
        }

        if (node == nullptr) return node;

        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Cas LL
        if (balance > 1 && getBalanceFactor(node->left) >= 0) {
            return rotateRight(node);
        }

        // Cas LR
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Cas RR
        if (balance < -1 && getBalanceFactor(node->right) <= 0) {
            return rotateLeft(node);
        }

        // Cas RL
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }
    
    /**
     * Parcours en ordre (InOrder) : affiche les racines triées
     * Complexité : O(n)
     */
    void inorderTraversal(AVLNode* node) {
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
    
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        deleteTree(root);
    }
    
    // ========================================================================
    // OPÉRATIONS PUBLIQUES
    // ========================================================================
    
    /**
     * Insère une nouvelle racine dans l'arbre AVL
     * Complexité : O(log n)
     */
    void insert(const std::string& rootStr) {
        this->root = insertNode(this->root, rootStr);
    }
    
    /**
     * Recherche une racine et retourne le nœud
     * Complexité : O(log n)
     */
    AVLNode* search(const std::string& rootStr) {
        return searchNode(this->root, rootStr);
    }
    
    /**
     * Vérifie si une racine existe
     * Complexité : O(log n)
     */
    bool contains(const std::string& rootStr) {
        return searchNode(this->root, rootStr) != nullptr;
    }
    
    /**
     * Retourne le nombre de nœuds (racines)
     * Complexité : O(n)
     */
    int countNodes(AVLNode* node) {
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
    AVLNode* getRoot() {
        return root;
    }
    
    /**
     * Définit le nœud racine (utile après insertion)
     */
    void setRoot(AVLNode* newRoot) {
        root = newRoot;
    }

    /**
     * Supprime une racine de l'arbre AVL
     * Complexité : O(log n)
     */
    void remove(const std::string& rootStr) {
        root = deleteNode(root, rootStr);
    }
};

#endif // AVL_TREE_H
