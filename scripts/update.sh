#!/bin/bash

set -e

PROJECT_ROOT="/home/ya.ryazancev/printer_hub"

echo "=== Print Agent Update ==="
date

cd "$PROJECT_ROOT"

echo "Fetching updates..."
git fetch origin main

LOCAL=$(git rev-parse HEAD)
REMOTE=$(git rev-parse origin/main)

if [ "$LOCAL" = "$REMOTE" ]; then
    echo "Already up to date."
    exit 0
fi

echo "New version detected:"
echo "  Local : $LOCAL"
echo "  Remote: $REMOTE"

echo "Updating repository..."
git pull --ff-only origin main

echo "Running installation..."
"$PROJECT_ROOT/scripts/install.sh"

echo "Update completed."