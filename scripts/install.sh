#!/bin/bash
set -e
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
echo "Project root: $PROJECT_ROOT"
echo "Installing dependencies..."

dnf install -y \
    systemd-devel \
    gcc-c++ \
    cmake \
    curl-devel

echo "Building..."

mkdir -p build
cd build

cmake ..
make -j$(nproc)

echo "Installing binary..."

if [ -f /usr/local/bin/print-agent ]; then
    rm -f /usr/local/bin/print-agent
fi

install -Dm755 print-agent \
    /usr/local/bin/print-agent

install -d /etc/print-agent
install -d /var/lib/print-agent

echo "Installing config..."

install -m 644 ${PROJECT_ROOT}/conf/.env \
    /etc/print-agent/.env

echo "Installing state.json..."
if [ -f ${PROJECT_ROOT}/state.json ]; then
    install -m 644 ${PROJECT_ROOT}/state.json \
        /var/lib/print-agent/state.json
fi
echo "Installing data.json..."
if [ -f ${PROJECT_ROOT}/print_jobs.json ]; then
    install -m 644 ${PROJECT_ROOT}/print_jobs.json \
        /var/lib/print-agent/print_jobs.json
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

systemctl daemon-reload

systemctl enable print-agent-init.service
systemctl enable print-agent-update.timer

systemctl start print-agent-init.service
systemctl start print-agent-update.timer

echo "Done."