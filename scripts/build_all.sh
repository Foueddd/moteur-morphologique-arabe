#!/bin/bash

# Script de compilation complète du projet
# Usage: ./scripts/build_all.sh

cd "$(dirname "$0")/.."

echo "🔧 Nettoyage des anciens fichiers de build..."
make clean

echo ""
echo "📦 Compilation du moteur CLI..."
make cli

echo ""
echo "🖼️  Compilation de l'interface graphique..."
make gui

echo ""
echo "✅ Compilation terminée avec succès!"
echo ""
echo "Pour exécuter:"
echo "  - CLI: ./build/morphology_engine"
echo "  - GUI: ./build/morphology_gui"
echo "  - ou: ./scripts/run_gui.sh"
