# Interface Graphique Qt

## 🎨 Lancement

Pour lancer l'interface graphique:

```bash
./bin/morphology_gui
```

Ou utiliser le script:
```bash
./run_gui.sh
```

## 🌟 Fonctionnalités

L'interface GUI simple offre:

1. **➕ Ajouter Racine** - Ajoute une nouvelle racine arabe à l'ABR
2. **🔍 Chercher** - Recherche une racine et affiche ses mots dérivés  
3. **📋 Tout Afficher** - Affiche toutes les racines (parcours inordre de l'ABR)
4. **⚙️ Générer Mot** - Génère un mot dérivé avec le pattern "فاعل"
5. **🗑️ Effacer** - Nettoie la zone de sortie

## 📝 Utilisation

### Ajouter une racine
1. Tapez une racine arabe (ex: `كتب`)
2. Cliquez sur **➕ Ajouter Racine**

### Chercher une racine
1. Tapez la racine à chercher
2. Cliquez sur **🔍 Chercher**

### Afficher toutes les racines
- Cliquez sur **📋 Tout Afficher**
- Les racines seront listées par ordre alphabétique (parcours ABR)

### Générer un mot dérivé
1. Tapez une racine existante
2. Cliquez sur **⚙️ Générer Mot (فاعل)**
3. Le mot sera affiché avec le pattern فاعل

## 🎯 Design

- Interface moderne avec couleurs différenciées
- Champ d'entrée aligné à droite (RTL pour l'arabe)
- Boutons colorés et ergonomiques
- Zone de sortie lisible

## 🔧 Compilation

```bash
make gui
```

Ou compiler tout:
```bash
make all
```
