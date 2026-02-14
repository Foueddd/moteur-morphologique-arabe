# Moteur de Recherche Morphologique Arabe

Projet universitaire — Algorithmique et Structures de Données  
**Langage** : C++11 | **Interface** : CLI + GUI (Qt5)

---

## Description

Ce projet implémente un moteur de traitement morphologique pour la langue arabe.  
Il permet de **générer**, **valider** et **analyser** des mots dérivés à partir de racines trilitérales et de schèmes morphologiques.

### Fonctionnalités

- **Génération** : Produire un mot dérivé à partir d'une racine et d'un schème (كتب + مفعول → مكتوب)
- **Validation** : Vérifier si un mot appartient morphologiquement à une racine donnée
- **Extraction** : Retrouver la racine et le schème d'un mot inconnu
- **Gestion** : Ajouter, supprimer, rechercher des racines et des schèmes

---

## Structures de Données

| Structure | Rôle | Détails |
|-----------|------|---------|
| **Arbre Binaire de Recherche (ABR)** | Stockage des racines | Clé numérique (index × 900 + index × 30 + index), construction équilibrée par médiane |
| **Table de Hachage** | Stockage des schèmes | Fonction djb2, chaînage par listes chaînées, rehash dynamique (seuil 0.75) |
| **Listes Chaînées** | Mots dérivés par racine | Insertion en tête, chaque nœud ABR possède sa propre liste |

### Méthode de Transformation (Racine → Mot)

Le système utilise une méthode dynamique : les lettres **ف** (C₁), **ع** (C₂), **ل** (C₃) dans le nom du schème indiquent où placer les consonnes de la racine. Tout autre caractère est copié tel quel.

> Exemple : كتب + فاعل → ك‌ا‌ت‌ب = **كاتب** (ف→ك, ا copié, ع→ت, ل→ب)

Cette méthode est extensible : tout nouveau schème contenant ف/ع/ل fonctionne automatiquement.

---

## Structure du Projet

```
moteur-morphologique-arabe/
├── core/                        # Noyau algorithmique
│   ├── structs.h                # Structures : DerivedWord, BSTNode, Pattern, ValidationResult
│   ├── bst_tree.h               # ABR : insertion, recherche, suppression, construction équilibrée
│   ├── hash_table.h             # Table de hachage : chaînage, rehash, djb2
│   └── morphology_engine.h     # Moteur : génération, validation, extraction
├── cli/                         # Interface ligne de commande
│   └── main.cpp                 # Menus interactifs (racines, schèmes, génération, validation)
├── gui/                         # Interface graphique Qt5
│   ├── gui_main.cpp
│   ├── gui_window.h
│   └── gui_window.cpp
├── utils/                       # Utilitaires
│   └── utils.h                  # UTF-8, calcul de clé, E/S fichier, affichage
├── data/
│   └── roots.txt                # 30+ racines arabes trilitérales
├── docs/
│   └── rapport_technique.md     # Rapport technique détaillé
├── Makefile                     # Compilation (cli, gui, clean)
└── README.md
```

---

## Compilation et Exécution

### Prérequis

- **g++** avec support C++11
- **Qt5** (pour l'interface graphique uniquement) : `qt5-default` ou `qtbase5-dev`

### Compiler

```bash
# Interface CLI uniquement
make cli

# Interface graphique Qt5
make gui

# Les deux
make all
```

### Exécuter

```bash
# CLI
make run-cli

# GUI
make run-gui
```

### Nettoyer

```bash
make clean
```

---

## Schèmes Morphologiques

| Schème | Type | Exemple (racine كتب) |
|--------|------|----------------------|
| فاعل | Participe actif | كاتب |
| مفعول | Participe passif | مكتوب |
| افتعل | Forme réflexive | اكتتب |
| تفعيل | Causatif | تكتيب |
| مفعال | Intensif | مكتاب |
| فعال | Pluriel / adjectif | كتاب |

Des schèmes supplémentaires peuvent être ajoutés dynamiquement via le menu.

---

## Complexités

| Opération | Complexité |
|-----------|------------|
| Ajouter / chercher / supprimer une racine | O(h) ≈ O(log n) |
| Ajouter / chercher / supprimer un schème | O(1) amorti |
| Générer un mot dérivé | O(h + k) |
| Valider un mot | O(m × k) |
| Extraire la racine d'un mot | O(m × h) |

*h* = hauteur ABR, *k* = longueur du mot, *m* = nombre de schèmes, *n* = nombre de racines

---

## Exemple d'Utilisation (CLI)

```
=== GÉNÉRATION ===
Racine : كتب
Schème : مفعول
→ Mot généré : مكتوب

=== VALIDATION ===
Mot : مكتوب   Racine : كتب
→ Valide ✓ (schème : مفعول)

=== EXTRACTION ===
Mot : كاتب
→ Racine : كتب  (schème : فاعل)
```

---

Projet d'algorithmique — Structures de données  
Février 2026
