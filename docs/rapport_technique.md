# RAPPORT TECHNIQUE
## Moteur Morphologique Arabe

**Date** : Février 2026  
**Structures** : ABR + Table de Hachage + Listes Chaînées

---

## 1. STRUCTURES DE DONNÉES

### 1.1 Arbre Binaire de Recherche (ABR)

**Objectif** : Gérer les racines arabes trilitérales.

**Structure** :
```cpp
struct BSTNode {
    std::string root;
    int height;
    DerivedWord* derivedList;
    BSTNode* left, *right;
};
```

**Opérations** :
- Insertion : O(h)
- Recherche : O(h)
- Parcours : O(n)

**h** = hauteur de l'arbre (≈ log n en cas moyen)

### 1.2 Table de Hachage

**Objectif** : Accès rapide aux schèmes morphologiques.

**Fonction de hachage** : djb2
```cpp
hash = ((hash << 5) + hash) + c
```

**Résolution de collisions** : Sondage linéaire

**Complexité** : O(1) en moyenne

### 1.3 Listes Chaînées

**Objectif** : Stocker les dérivés validés.

**Structure** :
```cpp
struct DerivedWord {
    std::string word;
    std::string pattern;
    int frequency;
    DerivedWord* next;
};
```

**Insertion** : O(1) en tête

---

## 2. ALGORITHMES

### 2.1 Génération de Mots

**Entrée** : Racine + Schème  
**Sortie** : Mot dérivé

**Exemple** : كتب + مفعول → مكتوب

**Étapes** :
1. Chercher racine dans ABR → O(h)
2. Chercher schème dans Hash → O(1)
3. Appliquer transformation → O(k)

**Complexité totale** : O(h + k)

### 2.2 Validation Morphologique

**Entrée** : Mot + Racine  
**Sortie** : Valide ou Non

**Algorithme** :
```
Pour chaque schème :
    Extraire racine du mot
    Si racine == racine attendue :
        Retourner VALIDE
Retourner NON VALIDE
```

**Complexité** : O(m × k) où m = nombre de schèmes

### 2.3 Extraction de Racine

**Entrée** : Mot  
**Sortie** : Racine + Schème

**Algorithme** :
```
Pour chaque schème :
    Extraire racine potentielle
    Chercher dans ABR
    Si trouvée :
        Retourner racine + schème
```

**Complexité** : O(m × h)

---

## 3. SCHÈMES MORPHOLOGIQUES

### Transformations Implémentées

Pour une racine R = [R₁, R₂, R₃] :

| Schème | Transformation |
|--------|----------------|
| فاعل | R₁ + ا + R₂ + R₃ |
| مفعول | م + R₁ + R₂ + و + R₃ |
| افتعل | ا + R₁ + ت + R₂ + R₃ |
| تفعيل | ت + R₁ + R₂ + ي + R₃ |

**Exemple** : كتب (k-t-b)
- فاعل → كاتب (agent)
- مفعول → مكتوب (patient)

---

## 4. ANALYSE DE COMPLEXITÉ

| Opération | Structure | Complexité |
|-----------|-----------|------------|
| Insérer racine | ABR | O(h) |
| Chercher racine | ABR | O(h) |
| Insérer schème | Hash | O(1) |
| Chercher schème | Hash | O(1) |
| Générer mot | ABR+Hash | O(h+k) |
| Valider mot | ABR+Hash | O(m×k) |
| Ajouter dérivé | Liste | O(1) |

**h** : hauteur ABR (≈ log n)  
**k** : longueur mot  
**m** : nombre de schèmes

---

## 5. EXEMPLES D'EXÉCUTION

### Exemple 1 : Génération
```
Input : كتب, مفعول
Étapes :
  1. Chercher كتب dans ABR → Trouvé
  2. Chercher مفعول dans Hash → Trouvé
  3. Appliquer : م + ك + ت + و + ب
Output : مكتوب
```

### Exemple 2 : Validation
```
Input : مكتوب, كتب
Étapes :
  1. Essayer schème مفعول
  2. Extraire : م[ك][ت]و[ب] → كتب
  3. Comparer كتب == كتب → OUI
Output : Valide ✓
```

---

## 6. CONCLUSION

Le système implémente efficacement un moteur morphologique en utilisant :

1. **ABR** - Organisation hiérarchique des racines
2. **Table de Hachage** - Accès rapide aux schèmes
3. **Listes Chaînées** - Gestion dynamique des dérivés

Les complexités sont optimales pour l'usage prévu (dataset moyen).
