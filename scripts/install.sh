#!/bin/bash
set -e

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

install -Dm755 print-agent \
    /usr/local/bin/print-agent/print-agent
install -Dm755 .env \
    /usr/local/bin/print-agent/.env
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