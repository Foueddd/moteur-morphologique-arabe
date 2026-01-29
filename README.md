# 🌿 Moteur de Recherche Morphologique Arabe

## Description du Projet

Ce projet implémente un **moteur morphologique complet** pour la langue arabe, basé sur les concepts algorithmiques avancés :
- **Arbre AVL** pour la gestion des racines arabes
- **Table de Hachage** pour les schèmes morphologiques
- **Listes Chaînées** pour les mots dérivés

Le système permet de :
1. ✅ Générer des mots dérivés à partir d'une racine et d'un schème
2. ✅ Valider si un mot appartient morphologiquement à une racine
3. ✅ Extraire la racine d'un mot existant

---

## 📁 Structure du Projet

```
moteur-morphologique-arabe/
├── src/
│   ├── structs.h              # Structures de données
│   ├── avl_tree.h             # Implémentation de l'arbre AVL
│   ├── hash_table.h           # Table de hachage (sondage linéaire)
│   ├── morphology_engine.h    # Moteur principal
│   ├── utils.h                # Fonctions utilitaires
│   └── main.cpp               # Interface CLI
├── data/
│   └── roots.txt              # Racines arabes par défaut
├── docs/
│   └── rapport_technique.md   # Rapport 2-3 pages
├── Makefile                   # Script de compilation
└── README.md                  # Ce fichier
```

---

## 🚀 Compilation et Exécution

### Prérequis
- **Compilateur** : g++ ou clang++
- **Standard C++** : C++11 ou supérieur
- **Système d'exploitation** : Linux, macOS, ou Windows (avec MinGW)

### Compilation
```bash
cd /tmp/moteur-morphologique-arabe
make                    # Compiler le projet
make run               # Compiler et exécuter
make clean             # Nettoyer les fichiers objets
make distclean         # Nettoyer tout
```

### Interface Graphique (Qt)
> Nécessite Qt Widgets (Qt5)

```bash
make gui               # Compiler l'interface graphique
./bin/morphology_gui   # Lancer la GUI
```

**UI** : thème sombre, disposition RTL, étiquettes bilingues (arabe/français).

### Exécution directe
```bash
./bin/morphology_engine
```

---

## 💡 Fonctionnalités Principales

### 1️⃣ Gestion des Racines (Arbre AVL)
- **Insertion** : O(log n) - Ajouter une nouvelle racine
- **Recherche** : O(log n) - Trouver une racine
- **Affichage** : O(n) - Lister toutes les racines en ordre

**Exemple** :
```
Racine : كتب (k-t-b) = Écrire
Racine : درس (d-r-s) = Étudier
Racine : قرأ (q-r-a) = Lire
```

### 2️⃣ Gestion des Schèmes (Table de Hachage)
Schèmes implémentés :
- **فاعل** (faʿil) : Agent/Participant actif
- **مفعول** (mafʿūl) : Patient/Sujet passif
- **افتعل** (iftaʿal) : Forme réflexive
- **تفعيل** (tafʿīl) : Causatif/Transitif
- **مفعال** (mafʿāl) : Intensif
- **فعال** (faʿāl) : Pluriel/Adjectif

**Complexité Hash** :
- Insertion : O(1) en moyenne
- Recherche : O(1) en moyenne
- Collision : Résolution par sondage linéaire

### 3️⃣ Génération Morphologique

**Exemple : Racine كتب + Schème مفعول = مكتوب**

Formule appliquée pour **مفعول** :
```
Racine = [R₁, R₂, R₃] = [ك, ت, ب]
Schème = م + R₁ + R₂ + و + R₃
Résultat = م + ك + ت + و + ب = مكتوب
```

Complexité : **O(k)** où k = longueur du mot généré

### 4️⃣ Validation Morphologique

**Exemple : Valider que مكتوب appartient à كتب**

Le système :
1. Essaie chaque schème
2. Extrait la racine du mot
3. Compare avec la racine attendue

Résultat : **OUI** ✓ (Schème: مفعول)

Complexité : **O(m × k)** où m = nombre de schèmes

### 5️⃣ Extraction de Racine

**Exemple : Extraire la racine de مكتوب**

Le système identifie le schème et extrait les consonnes radicales.

Résultat : **كتب**

---

## 📊 Complexité Algorithmique

| Opération | Structure | Complexité | Notes |
|-----------|-----------|-----------|-------|
| Insérer racine | AVL | O(log n) | Équilibrage automatique |
| Chercher racine | AVL | O(log n) | Recherche binaire |
| Insérer schème | Hash | O(1) avg | Sondage linéaire |
| Chercher schème | Hash | O(1) avg | Accès direct |
| Générer mot | Hash + Calcul | O(k) | k = longueur |
| Valider mot | Hash + Calcul | O(m × k) | m = schèmes |
| Parcours racines | AVL | O(n) | En-ordre |
| Afficher schèmes | Hash | O(n) | Toute la table |

---

## 📚 Exemples d'Utilisation

### Exemple 1 : Génération Simple
```
Entrée :
  Racine: كتب
  Schème: فاعل
Sortie:
  Mot généré: كاتب (celui qui écrit)
```

### Exemple 2 : Validation Morphologique
```
Entrée :
  Mot: مكتوب
  Racine attendue: كتب
Sortie:
  Valide: ✓ OUI
  Schème trouvé: مفعول
```

### Exemple 3 : Extraction de Racine
```
Entrée :
  Mot: مدرسة
Sortie:
  Racine trouvée: درس
  Schème: مفعلة
```

---

## 🔧 Architecture Technique

### Structures de Données

#### AVLNode (Arbre)
```cpp
struct AVLNode {
    std::string root;           // Racine (كتب)
    int height;                 // Hauteur pour équilibre
    DerivedWord* derivedList;   // Liste chaînée de dérivés
    AVLNode* left, *right;      // Fils gauche/droit
};
```

#### DerivedWord (Liste Chaînée)
```cpp
struct DerivedWord {
    std::string word;           // مكتوب
    std::string pattern;        // مفعول
    int frequency;              // Fréquence
    DerivedWord* next;          // Suivant
};
```

#### Pattern (Schème)
```cpp
struct Pattern {
    std::string name;           // فاعل
    std::string structure;      // VCCCVC
    std::string description;    // "Participe actif"
};
```

#### HashTable
```cpp
struct HashTable {
    Pattern table[50];          // Tableau de schèmes
    bool occupied[50];          // Indicateurs
    int size;                   // Nombre d'éléments
};
```

### Fonctions Clés

**AVL Operations** :
- `insert()` - Insertion avec équilibrage
- `rotateLeft()` / `rotateRight()` - Équilibrage AVL
- `search()` - Recherche binaire
- `getBalanceFactor()` - Vérification d'équilibre

**Hash Operations** :
- `hashFunction()` - Fonction de hachage djb2
- `insert()` - Insertion avec sondage linéaire
- `search()` - Recherche dans la table
- `reorganizeAfterDeletion()` - Cohérence après suppression

**Morphology Operations** :
- `generateDerivedWord()` - Génération
- `validateWord()` - Validation
- `applyPatternTransformation()` - Application du schème
- `extractRootFromWord()` - Extraction inverse

---

## 🧠 Concepts Algorithmiques Appliqués

### 1. Arbre Binaire de Recherche Équilibré (AVL)
- **Concepts** : Rotation, facteur d'équilibre, hauteur
- **Cas d'utilisation** : Gestion triée et efficace des racines
- **Avantage** : Garantit O(log n) même dans le pire cas

### 2. Table de Hachage
- **Concepts** : Fonction de hachage, résolution de collision, facteur de charge
- **Cas d'utilisation** : Accès rapide aux schèmes
- **Avantage** : Accès O(1) en moyenne

### 3. Listes Chaînées
- **Concepts** : Allocation dynamique, pointeurs
- **Cas d'utilisation** : Stock des mots dérivés par racine
- **Avantage** : Flexibilité, insertion O(1)

### 4. Analyse de Complexité
- **Temps** : Optimisation des requêtes
- **Espace** : Gestion de la mémoire

---

## 📖 Fichiers du Projet

### En-têtes (Headers)
- **structs.h** (108 lignes)
  - Structures : AVLNode, DerivedWord, Pattern, HashTable, ValidationResult

- **avl_tree.h** (289 lignes)
  - Classe AVLTree avec opérations d'équilibrage
  - Fonctions : insert(), search(), rotate(), rebalance()

- **hash_table.h** (326 lignes)
  - Classe PatternHashTable avec sondage linéaire
  - Fonction de hachage djb2
  - Gestion des collisions

- **morphology_engine.h** (431 lignes)
  - Cœur du moteur
  - Génération, validation, extraction de racines

- **utils.h** (308 lignes)
  - Utilitaires : E/S, formatage, menus
  - Chargement/sauvegarde de fichiers

### Implémentation
- **main.cpp** (480 lignes)
  - Interface CLI interactive
  - Menus principaux et sous-menus
  - Gestion des entrées utilisateur

### Données
- **roots.txt**
  - Racines arabes de démonstration

### Build
- **Makefile**
  - Compilation avec g++
  - Cibles : all, run, clean, debug

---

## 🧪 Test du Projet

### Scénario de Test Complet

1. **Ajouter des racines**
   ```
   كتب, درس, قرأ, ذهب, شرب
   ```

2. **Initialiser les schèmes** par défaut (6 schèmes)

3. **Générer des mots** :
   - كتب + فاعل = كاتب
   - كتب + مفعول = مكتوب

4. **Valider des mots** :
   - مكتوب ✓ من كتب
   - مدرسة ✓ من درس

5. **Extraire des racines** :
   - من مكتوب → كتب

---

## 📝 Notes Importantes

### Gestion des Caractères Arabes
- Le projet utilise UTF-8 pour les caractères arabes
- Compatibilité avec tous les systèmes modernes
- Aucune bibliothèque externe requise

### Limitations Connues
1. Les transformations morphologiques sont simplifiées
2. Les caractères diacritiques ne sont pas gérés
3. Les racines quadrilitérales ne sont pas supportées
4. Pas de gestion complète des exceptions de la langue

### Extensions Possibles
1. Ajouter plus de schèmes morphologiques
2. Implémenter les racines quadrilitérales
3. Ajouter la gestion des affixes (préfixes/suffixes)
4. Créer une base de données complète
5. Interface graphique

---

## 🎓 Concepts Pédagogiques

Ce projet illustre :
1. ✅ Structure de données avancées (AVL, Hash)
2. ✅ Algorithmes d'équilibrage d'arbres
3. ✅ Résolution de collisions en hachage
4. ✅ Manipulation de listes chaînées
5. ✅ Analyse de complexité algorithmique
6. ✅ Gestion de la mémoire en C++
7. ✅ Interface CLI interactive
8. ✅ Programmation modulaire

---

## 👨‍💻 Auteur

Mini-projet d'algorithmique avancée - Année universitaire 2025-2026

---

## 📄 Licence

Projet académique - Utilisation libre pour fins éducatives

---

## 🔗 Ressources

### Concepts AVL
- [AVL Tree Wikipedia](https://en.wikipedia.org/wiki/AVL_tree)
- [AVL Rotations](https://www.geeksforgeeks.org/avl-tree-set-1-insertion/)

### Hachage
- [Hash Table Wikipedia](https://en.wikipedia.org/wiki/Hash_table)
- [Collision Resolution](https://www.geeksforgeeks.org/hashing-set-2-separate-chaining/)

### Morphologie Arabe
- [Arabic Morphology](https://en.wikipedia.org/wiki/Semitic_root)
- [Root-Pattern System](https://en.wikipedia.org/wiki/Semitic_root)

---

**✓ Prêt pour la compilation et l'exécution!**
