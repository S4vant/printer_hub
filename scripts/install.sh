#!/bin/bash
set -e
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CUPS_CONF="/etc/cups/cupsd.conf"
AGENT_CONF="/etc/print-agent/print-agent.conf"
LOCAL_CONF="${PROJECT_ROOT}/conf/print-agent.conf"

if [ "$EUID" -ne 0 ]; then
    echo "Please run as root."
    exit 1
fi

echo "Project root: $PROJECT_ROOT"
echo "Installing dependencies..."

dnf install -y \
    systemd-devel \
    gcc-c++ \
    cmake \
    curl-devel \
    cups

echo "Building..."

if ! grep -q "^LogLevel debug$" "$CUPS_CONF"; then
    echo "Setting CUPS LogLevel to debug..."

    if grep -q "^LogLevel" "$CUPS_CONF"; then
        sed -i 's/^LogLevel.*/LogLevel debug/' "$CUPS_CONF"
    else
        echo "LogLevel debug" | tee -a "$CUPS_CONF" >/dev/null
    fi

    systemctl restart cups
fi

rm -rf build
if [ -f ${PROJECT_ROOT}/CMakeCache.txt ]; then
    rm -f CMakeCache.txt
fi
mkdir -p build
cd build

cmake ..
make -j$(nproc)

echo "Installing binary..."

rm /usr/local/bin/print-agent

install -Dm755 print-agent \
    /usr/local/bin/print-agent

install -d /etc/print-agent
install -d /var/lib/print-agent

echo "Installing config..."

CONF_UPDATE_VALUE=""
if [ -f "$LOCAL_CONF" ]; then
    CONF_UPDATE_VALUE=$(grep -E '^CONF_UPDATE=' "$LOCAL_CONF" | tail -n1 | cut -d= -f2- | tr -d '\r')
fi
if [ -z "$CONF_UPDATE_VALUE" ] && [ -f "$AGENT_CONF" ]; then
    CONF_UPDATE_VALUE=$(grep -E '^CONF_UPDATE=' "$AGENT_CONF" | tail -n1 | cut -d= -f2- | tr -d '\r')
fi
if [ -z "$CONF_UPDATE_VALUE" ]; then
    CONF_UPDATE_VALUE="yes"
fi

if [ ! -f "$AGENT_CONF" ] || [ "$CONF_UPDATE_VALUE" = "yes" ]; then
    echo "Installing config from ${LOCAL_CONF}..."
    install -m 644 "$LOCAL_CONF" "$AGENT_CONF"
else
    echo "CONF_UPDATE is disabled; keeping existing config at $AGENT_CONF"
fi

echo "Set project root in config..."
if [ -f "$AGENT_CONF" ]; then
    if grep -q '^PROJECT_ROOT=' "$AGENT_CONF"; then
        sed -i "s|^PROJECT_ROOT=.*|PROJECT_ROOT=${PROJECT_ROOT}|" "$AGENT_CONF"
    else
        echo "\nPROJECT_ROOT=${PROJECT_ROOT}" >> "$AGENT_CONF"
    fi
else
    echo "Warning: agent config not found at $AGENT_CONF"
fi

echo "Installing state.json..."
if [ -f ${PROJECT_ROOT}/state.json ]; then
    install -m 644 ${PROJECT_ROOT}/state.json \
        /var/lib/print-agent/state.json
fi
echo "Installing data.json..."
if [ -f ${PROJECT_ROOT}/print_jobs.json ]; then
    install -m 644 ${PROJECT_ROOT}/print_jobs.json \
        /var/lib/print-agent/print_jobs.json
fi
echo "Installing systemd units..."

install -Dm644 \
    ../systemd/print-agent-init.service \
    /etc/systemd/system/print-agent-init.service

install -Dm644 \
    ../systemd/print-agent-update.service \
    /etc/systemd/system/print-agent-update.service

install -Dm644 \
    ../systemd/print-agent-update.timer \
    /etc/systemd/system/print-agent-update.timer

install -Dm755 \
    ../scripts/update.sh \
    /usr/local/sbin/print-agent-update.sh

echo "Reloading systemd..."

systemctl daemon-reload

systemctl enable print-agent-init.service
systemctl enable print-agent-update.timer

systemctl start print-agent-init.service
systemctl start print-agent-update.timer

echo "installing completed"