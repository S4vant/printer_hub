#!/bin/bash
set -e

systemctl stop print-agent-update.timer || true
systemctl disable print-agent-update.timer || true

systemctl stop print-agent-init.service || true
systemctl disable print-agent-init.service || true

rm -f /etc/systemd/system/print-agent-init.service
rm -f /etc/systemd/system/print-agent-update.service
rm -f /etc/systemd/system/print-agent-update.timer

rm -f /usr/local/bin/print-agent

systemctl daemon-reload