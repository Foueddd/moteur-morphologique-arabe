# Moteur de Recherche Morphologique Arabe

## Description

Système de traitement morphologique pour la langue arabe.

**Structures de données :**
- Arbre Binaire de Recherche (ABR) - Racines arabes
- Table de Hachage (sondage linéaire) - Schèmes morphologiques  
- Listes Chaînées - Mots dérivés

**Fonctionnalités :**
- Générer des mots dérivés (كتب + مفعول → مكتوب)
- Valider l'appartenance d'un mot à une racine
- Extraire la racine d'un mot

---

## Structure du Projet

```
src/
  ├── structs.h              # Structures de données
  ├── bst_tree.h             # ABR
  ├── hash_table.h           # Table de hachage
  ├── morphology_engine.h    # Moteur principal
  ├── utils.h                # Utilitaires
  └── main.cpp               # Interface
data/
  └── roots.txt              # Racines (30+)
docs/
  └── rapport_technique.md   # Documentation
```

---

## Compilation et Exécution

```bash
make                    # Compiler
make run                # Lancer
make clean              # Nettoyer
```

---

## Structures de Données

### 1. Arbre Binaire de Recherche (ABR)
**Usage :** Stockage des racines arabes  
**Complexité :**
- Insertion : O(h) où h ≈ log(n)
- Recherche : O(h)

### 2. Table de Hachage
**Usage :** Stockage des schèmes  
**Méthode :** Sondage linéaire  
**Complexité :** O(1) en moyenne

### 3. Listes Chaînées
**Usage :** Mots dérivés  
**Complexité :** O(1) insertion

---

## Schèmes Morphologiques

| Schème | Type | Exemple |
|--------|------|---------|
| فاعل | Agent | كاتب |
| مفعول | Patient | مكتوب |
| افتعل | Réflexif | اكتتب |
| تفعيل | Causatif | تكتيب |

---

## Exemple d'Utilisation

### Génération
```
Racine : كتب
Schème : مفعول
→ Résultat : مكتوب
```

### Validation
```
Mot : مكتوب
Racine : كتب
→ Valide ✓ (schème: مفعول)
```

### Extraction
```
Mot : مكتوب
→ Racine : كتب
```

---

## Complexités

| Opération | Complexité |
|-----------|-----------|
| Ajouter racine | O(h) |
| Chercher racine | O(h) |
| Ajouter schème | O(1) |
| Générer mot | O(k) |
| Valider mot | O(m×k) |

---

## Auteur

Projet d'algorithmique - Structures de données  
Février 2026
