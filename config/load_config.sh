#!/bin/bash

# Script pour charger et valider la configuration

CONFIG_DIR="$(dirname "$0")"

echo "Chargement de la configuration Smart City Emergency..."
echo "=================================================="

# Vérifier l'existence des fichiers
required_files=("config.json" "scenarios.json" "controls.json")
for file in "${required_files[@]}"; do
    if [ -f "$CONFIG_DIR/$file" ]; then
        echo "✓ $file trouvé"
    else
        echo "✗ $file manquant"
    fi
done

echo ""
echo "Validation de config.json..."

if command -v python3 &> /dev/null; then
    python3 -m json.tool "$CONFIG_DIR/config.json" > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo "✓ config.json a une syntaxe JSON valide"
    else
        echo "✗ config.json a une syntaxe JSON invalide"
    fi
else
    echo "⚠ Python3 non installé, impossible de valider la syntaxe JSON"
fi

echo ""
echo "Configuration chargée avec succès !"
