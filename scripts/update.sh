#!/bin/bash

set -e

CUPS_CONF="/etc/cups/cupsd.conf"
AGENT_CONF="/etc/print-agent/print-agent.conf"
PROJECT_ROOT=""

echo "=== Print Agent Update ==="
date

if [ -f "$AGENT_CONF" ]; then
    PROJECT_ROOT=$(grep -E '^PROJECT_ROOT=' "$AGENT_CONF" | tail -n1 | cut -d= -f2- | tr -d '\r')
fi

if [ -z "$PROJECT_ROOT" ]; then
    echo "PROJECT_ROOT is not configured in $AGENT_CONF"
    exit 1
fi

AGENT_UPDATE_VALUE=""
if [ -f "$AGENT_CONF" ]; then
    AGENT_UPDATE_VALUE=$(grep -E '^AGENT_UPDATE=' "$AGENT_CONF" | tail -n1 | cut -d= -f2- | tr -d '\r')
fi
if [ -z "$AGENT_UPDATE_VALUE" ]; then
    AGENT_UPDATE_VALUE="yes"
fi

if [ "$AGENT_UPDATE_VALUE" = "yes" ]; then
    echo "AGENT_UPDATE is enabled."
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
else
    echo "AGENT_UPDATE is disabled. Skipping update."
fi

