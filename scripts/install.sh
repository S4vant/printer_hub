#!/bin/bash
set -e
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CUPS_CONF="/etc/cups/cupsd.conf"

if ! grep -q "^LogLevel debug$" "$CUPS_CONF"; then
    echo "Setting CUPS LogLevel to debug..."

    if grep -q "^LogLevel" "$CUPS_CONF"; then
        sudo sed -i 's/^LogLevel.*/LogLevel debug/' "$CUPS_CONF"
    else
        echo "LogLevel debug" | sudo tee -a "$CUPS_CONF" >/dev/null
    fi

    sudo systemctl restart cups
fi
echo "Project root: $PROJECT_ROOT"
echo "Installing dependencies..."

dnf install -y \
    systemd-devel \
    gcc-c++ \
    cmake \
    curl-devel

echo "Building..."

rm -rf build
if [ -f ${PROJECT_ROOT}/CMakeChache.txt ]; then
    rm -f CMakeChache.txt
fi
mkdir -p build
cd build

cmake ..
make -j$(nproc)

echo "Installing binary..."

if [ -f /usr/local/bin/print-agent ]; then
    rm -r /usr/local/bin/print-agent
fi

install -Dm755 print-agent \
    /usr/local/bin/print-agent

install -d /etc/print-agent
install -d /var/lib/print-agent

echo "Installing config..."

install -m 644 ${PROJECT_ROOT}/conf/print-agent.conf \
    /etc/print-agent/print-agent.conf

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

install -Dm644 \
    ../scripts/update.sh \
    /usr/local/sbin/print-agent-update.sh

echo "Reloading systemd..."

systemctl daemon-reload

systemctl enable print-agent-init.service
systemctl enable print-agent-update.timer

systemctl start print-agent-init.service
systemctl start print-agent-update.timer

echo "Done."