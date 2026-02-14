# RAPPORT TECHNIQUE
## Moteur Morphologique Arabe

**Date** : Février 2026  
**Langage** : C++11  
**Structures** : ABR + Table de Hachage + Listes Chaînées

---

## 1. STRUCTURES DE DONNÉES

### 1.1 Arbre Binaire de Recherche (ABR)

**Objectif** : Gérer les racines arabes trilitérales.

**Clé numérique** : Chaque racine est encodée en un entier unique :
```
clé = index(C₁) × 900 + index(C₂) × 30 + index(C₃)
```
Où `index()` retourne le rang alphabétique arabe du caractère (1 à 29).

**Structure** :
```cpp
struct BSTNode {
    std::string root;         // Racine arabe (ex: كتب)
    int key;                  // Clé numérique calculée
    int height;               // Hauteur du sous-arbre
    DerivedWord* derivedList; // Liste chaînée des mots dérivés
    BSTNode* left, *right;
};
```

**Construction équilibrée** : Les racines sont chargées depuis un fichier, encodées, triées par clé, puis insérées par la méthode médiane (insertion du milieu du tableau trié, puis récursion sur les deux moitiés). Cela produit un ABR équilibré sans rotations.

**Opérations** :
| Opération | Complexité |
|-----------|------------|
| Insertion | O(h) |
| Recherche | O(h) |
| Suppression | O(h) |
| Parcours en-ordre | O(n) |

**h** = hauteur de l'arbre (≈ log n après construction équilibrée)

### 1.2 Table de Hachage

**Objectif** : Accès rapide aux schèmes morphologiques.

**Fonction de hachage** : djb2
```cpp
hash = 5381;
for (chaque octet c de la clé) :
    hash = ((hash << 5) + hash) + c;
```

**Résolution de collisions** : Chaînage par listes chaînées (chaque case contient une liste de nœuds).

**Rehash dynamique** : Lorsque le facteur de charge dépasse 0.75, la table est redimensionnée (capacité × 2) et tous les éléments sont réinsérés.

**Complexité** : O(1) en moyenne pour insertion, recherche et suppression.

### 1.3 Listes Chaînées

**Objectif** : Stocker les mots dérivés associés à chaque racine.

**Structure** :
```cpp
struct DerivedWord {
    std::string word;       // Mot dérivé (ex: مكتوب)
    std::string pattern;    // Schème utilisé (ex: مفعول)
    int frequency;          // Nombre de fois généré
    DerivedWord* next;      // Pointeur vers le suivant
};
```

Chaque nœud de l'ABR possède un pointeur `derivedList` vers la tête de sa liste de dérivés.

**Insertion** : O(1) en tête de liste (avec vérification de doublon en O(k)).

---

## 2. ALGORITHMES

### 2.1 Génération de Mots (Méthode dynamique)

Le système utilise une méthode générique basée sur les lettres **ف** (C₁), **ع** (C₂), **ل** (C₃) présentes dans le nom du schème. Pour appliquer un schème à une racine :

1. Découper le nom du schème en caractères UTF-8
2. Pour chaque caractère :
   - Si **ف** → remplacer par la 1ère lettre de la racine
   - Si **ع** → remplacer par la 2ème lettre de la racine
   - Si **ل** → remplacer par la 3ème lettre de la racine
   - Sinon → copier le caractère tel quel

**Exemple** : Racine كتب + Schème مفعول
```
م → م (copié)
ف → ك (C₁)
ع → ت (C₂)
و → و (copié)
ل → ب (C₃)
Résultat : مكتوب
```

**Avantage** : Tout nouveau schème contenant ف/ع/ل fonctionne automatiquement, sans modification du code.

**Complexité** : O(h + k) — recherche dans l'ABR + parcours du schème.

### 2.2 Validation Morphologique

**Entrée** : Mot + Racine attendue  
**Sortie** : Valide ou Non + Schème trouvé

**Algorithme** :
```
Pour chaque schème dans la table de hachage :
    Extraire la racine potentielle du mot (positions de ف/ع/ل)
    Si racine extraite == racine attendue :
        Retourner VALIDE + schème
Retourner NON VALIDE
```

**Complexité** : O(m × k) où m = nombre de schèmes, k = longueur du mot.

### 2.3 Extraction de Racine

**Entrée** : Mot  
**Sortie** : Racine + Schème

**Algorithme** :
```
Pour chaque schème :
    Extraire racine potentielle (positions ف/ع/ل)
    Chercher cette racine dans l'ABR
    Si trouvée :
        Retourner racine + schème
Retourner NON TROUVÉ
```

**Complexité** : O(m × h)

---

## 3. SCHÈMES MORPHOLOGIQUES

### Schèmes par défaut

| Schème | Description | Exemple (كتب) |
|--------|-------------|----------------|
| فاعل | Participe actif (agent) | كاتب |
| مفعول | Participe passif (patient) | مكتوب |
| افتعل | Forme VIII (réflexive) | اكتتب |
| تفعيل | Forme II (causatif) | تكتيب |
| مفعال | Forme intensive | مكتاب |
| فعال | Pluriel / adjectif | كتاب |

**Extensible** : L'utilisateur peut ajouter de nouveaux schèmes à tout moment via le menu. La transformation est automatique tant que le schème contient les lettres ف, ع et ل.

---

## 4. ANALYSE DE COMPLEXITÉ

| Opération | Structure | Complexité |
|-----------|-----------|------------|
| Insérer racine | ABR | O(h) |
| Chercher racine | ABR | O(h) |
| Supprimer racine | ABR | O(h) |
| Insérer schème | Table Hash | O(1) amorti |
| Chercher schème | Table Hash | O(1) |
| Supprimer schème | Table Hash | O(1) |
| Générer mot | ABR + Hash | O(h + k) |
| Valider mot | ABR + Hash | O(m × k) |
| Extraire racine | ABR + Hash | O(m × h) |
| Ajouter dérivé | Liste chaînée | O(1) |

**h** : hauteur ABR (≈ log n après construction équilibrée)  
**k** : longueur du mot en caractères  
**m** : nombre de schèmes  
**n** : nombre de racines

---

## 5. EXEMPLES D'EXÉCUTION

### Exemple 1 : Génération
```
Entrée : racine = كتب, schème = مفعول
Étapes :
  1. Chercher كتب dans ABR → Trouvé
  2. Chercher مفعول dans la table de hachage → Trouvé
  3. Appliquer transformation dynamique :
     م(copié) + ك(ف→C₁) + ت(ع→C₂) + و(copié) + ب(ل→C₃)
Sortie : مكتوب
```

### Exemple 2 : Validation
```
Entrée : mot = مكتوب, racine attendue = كتب
Étapes :
  1. Essayer schème مفعول (positions : م_ف_ع_و_ل)
  2. Extraire : position ف→ك, position ع→ت, position ل→ب → كتب
  3. Comparer كتب == كتب → OUI
Sortie : Valide ✓ (schème : مفعول)
```

### Exemple 3 : Extraction de racine
```
Entrée : mot = كاتب
Étapes :
  1. Essayer schème فاعل (positions : ف_ا_ع_ل)
  2. Extraire : position ف→ك, position ع→ت, position ل→ب → كتب
  3. Chercher كتب dans ABR → Trouvé
Sortie : Racine = كتب, Schème = فاعل
```

---

## 6. CONCLUSION

Le système implémente un moteur morphologique arabe complet en utilisant trois structures de données complémentaires :

1. **ABR** — Organisation hiérarchique des racines avec clé numérique et construction équilibrée par médiane
2. **Table de Hachage** — Accès O(1) aux schèmes avec chaînage et rehash dynamique (djb2)
3. **Listes Chaînées** — Gestion dynamique des mots dérivés par racine

La méthode de transformation dynamique (positions ف/ع/ل) permet d'ajouter de nouveaux schèmes sans modifier le code source, ce qui rend le système extensible.
