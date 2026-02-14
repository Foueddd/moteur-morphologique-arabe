#ifndef BST_TREE_H
#define BST_TREE_H

#include "structs.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <vector>

class BSTree {
private:
    BSTNode* root;

    // Pair racine + clé numérique pour tri et construction équilibrée
    struct RootEntry {
        std::string root;
        int key;
    };
    // Construit un ABR équilibré à partir d’un tableau trié par clé
    BSTNode* buildBalancedRecursive(std::vector<RootEntry>& entries, int start, int end) {
        if (start > end) return nullptr;
        
        int mid = (start + end) / 2;
        BSTNode* node = new BSTNode(entries[mid].root, entries[mid].key);
        node->left = buildBalancedRecursive(entries, start, mid - 1);
        node->right = buildBalancedRecursive(entries, mid + 1, end);
        int lh = (node->left) ? node->left->height : 0;
        int rh = (node->right) ? node->right->height : 0;
        node->height = 1 + std::max(lh, rh);
        
        return node;
    }
    // Insertion par clé numérique
    BSTNode* insertNode(BSTNode* node, const std::string& rootStr, int key) {
        if (node == nullptr) {
            return new BSTNode(rootStr, key);
        }
        if (key < node->key) {
            node->left = insertNode(node->left, rootStr, key);
        } else if (key > node->key) {
            node->right = insertNode(node->right, rootStr, key);
        } else {
            return node;
        }
        int leftHeight = (node->left != nullptr) ? node->left->height : 0;
        int rightHeight = (node->right != nullptr) ? node->right->height : 0;
        node->height = 1 + std::max(leftHeight, rightHeight);
        
        return node;
    }
    // Recherche par clé numérique
    BSTNode* searchNode(BSTNode* node, int key) {
        if (node == nullptr) {
            return nullptr;
        }
        if (key == node->key) {
            return node;
        } else if (key < node->key) {
            return searchNode(node->left, key);
        } else {
            return searchNode(node->right, key);
        }
    }
    
    // Libère une liste de dérivés
    void deleteDerivedList(DerivedWord* list) {
        DerivedWord* current = list;
        while (current != nullptr) {
            DerivedWord* next = current->next;
            delete current;
            current = next;
        }
    }

    // Libère tout l’arbre
    void deleteTree(BSTNode* node) {
        if (node == nullptr) return;
        
        deleteTree(node->left);
        deleteTree(node->right);
        
        deleteDerivedList(node->derivedList);
        delete node;
    }

    BSTNode* minValueNode(BSTNode* node) {
        BSTNode* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    // Suppression par clé numérique
    BSTNode* deleteNode(BSTNode* node, int key) {
        if (node == nullptr) {
            return node;
        }

        if (key < node->key) {
            node->left = deleteNode(node->left, key);
        } else if (key > node->key) {
            node->right = deleteNode(node->right, key);
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                BSTNode* child = (node->left != nullptr) ? node->left : node->right;

                if (child == nullptr) {
                    deleteDerivedList(node->derivedList);
                    delete node;
                    node = nullptr;
                } else {
                    BSTNode* old = node;
                    node = child;
                    deleteDerivedList(old->derivedList);
                    delete old;
                }
            } else {
                BSTNode* succ = minValueNode(node->right);
                node->root = succ->root;
                node->key = succ->key;
                std::swap(node->derivedList, succ->derivedList);
                node->right = deleteNode(node->right, succ->key);
            }
        }

        if (node == nullptr) return node;

        int leftHeight = (node->left != nullptr) ? node->left->height : 0;
        int rightHeight = (node->right != nullptr) ? node->right->height : 0;
        node->height = 1 + std::max(leftHeight, rightHeight);

        return node;
    }
    
    // Parcours en ordre (racines triées)
    void inorderTraversal(BSTNode* node) {
        if (node == nullptr) return;
        
        inorderTraversal(node->left);
        
        std::cout << "  Racine: " << node->root << " (Clé: " << node->key << ", Hauteur: " << node->height << ")" << std::endl;
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
    BSTree() : root(nullptr) {}
    
    ~BSTree() {
        deleteTree(root);
    }
    // Calcule la clé puis insère
    void insert(const std::string& rootStr) {
        int key = Utils::computeRootKey(rootStr);
        if (key <= 0) return;
        root = insertNode(root, rootStr, key);
    }
    // Recherche par racine (clé calculée)
    BSTNode* search(const std::string& rootStr) {
        int key = Utils::computeRootKey(rootStr);
        if (key <= 0) return nullptr;
        return searchNode(root, key);
    }
    // Vérifie l’existence d’une racine
    bool contains(const std::string& rootStr) {
        int key = Utils::computeRootKey(rootStr);
        if (key <= 0) return false;
        return searchNode(root, key) != nullptr;
    }
    int countNodes(BSTNode* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }
    
    int getSize() {
        return countNodes(root);
    }
    
    // Affiche toutes les racines
    void displayAll() {
        if (root == nullptr) {
            std::cout << "Aucune racine dans l'arbre." << std::endl;
            return;
        }
        std::cout << "\n=== Affichage des Racines (En-Ordre) ===" << std::endl;
        inorderTraversal(root);
        std::cout << "\nTotal de racines : " << getSize() << std::endl;
    }
    
    BSTNode* getRoot() {
        return root;
    }

    // Supprime une racine
    void remove(const std::string& rootStr) {
        int key = Utils::computeRootKey(rootStr);
        if (key <= 0) return;
        root = deleteNode(root, key);
    }
    // Construction équilibrée : encodage → tri → médiane
    void buildBalanced(std::vector<std::string>& roots) {
        std::vector<RootEntry> entries;
        for (size_t i = 0; i < roots.size(); i++) {
            RootEntry e;
            e.root = roots[i];
            e.key = Utils::computeRootKey(roots[i]);
            if (e.key > 0) {
                entries.push_back(e);
            }
        }
        std::sort(entries.begin(), entries.end(),
                  [](const RootEntry& a, const RootEntry& b) {
                      return a.key < b.key;
                  });
        entries.erase(
            std::unique(entries.begin(), entries.end(),
                [](const RootEntry& a, const RootEntry& b) {
                    return a.key == b.key;
                }),
            entries.end());
        deleteTree(root);
        root = nullptr;
        if (!entries.empty()) {
            root = buildBalancedRecursive(entries, 0, (int)entries.size() - 1);
        }
    }
};

#endif // BST_TREE_H
