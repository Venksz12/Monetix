#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
command -v docker >/dev/null || { echo "Docker is required"; exit 1; }
command -v docker compose >/dev/null || { echo "Docker Compose is required"; exit 1; }
mkdir -p infrastructure/postgres infrastructure/clickhouse infrastructure/nats
echo "Monetix project is ready."
echo "Run: docker compose -f infrastructure/docker-compose.yml up --build"
