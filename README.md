# Monetix

Monetix is an academic reference implementation of a distributed API marketplace and usage-based monetization platform.

## Architecture

- **Gateway:** C++23 + Boost.Asio/Beast. Authenticates API keys, applies a token-bucket policy, forwards HTTP requests, emits request/completion events.
- **Services:** C++23 + Drogon: identity, marketplace, pricing, metering, wallet.
- **Persistence:** PostgreSQL 16 for transactional state, Redis 7 for hot-path rate/quota state, ClickHouse for telemetry, NATS JetStream for durable events.
- **Portal:** Next.js 14 App Router + TypeScript + Tailwind.
- **Tests:** C++ unit tests, Python integration smoke test, k6 load test.

This is intentionally production-shaped but keeps external-provider integrations replaceable. Payment providers, JWT signing keys, Redis/NATS clients, and upstream APIs are configured by environment variables.

## Quick start

Prerequisites: Docker, Docker Compose, curl.

```bash
./setup.sh
docker compose -f infrastructure/docker-compose.yml up --build
```

Portal: http://localhost:3000  
Gateway: http://localhost:8080  
Identity: http://localhost:8081  
Marketplace: http://localhost:8082  
Pricing: http://localhost:8083  
Wallet: http://localhost:8084  
Metering health: http://localhost:8085/health

Demo API key: `demo_monetix_key`

Example:

```bash
curl -H 'X-API-Key: demo_monetix_key' http://localhost:8080/demo/hello
```

## Academic focus

The repository demonstrates:
1. asynchronous HTTP I/O and reverse proxying;
2. deterministic money arithmetic in integer minor units;
3. immutable double-entry ledger modeling;
4. idempotent usage metering;
5. event-driven decoupling with JetStream;
6. horizontally-safe rate-limit/quota concepts;
7. API/version/plan/subscription domain modeling;
8. observability-ready telemetry.

The included Docker Compose stack is a local reference environment, not a hardened production deployment. Before production, add TLS termination, secret management, HA PostgreSQL/Redis/NATS, provider-specific payment verification, persistent JWT key rotation, and a dedicated service mesh/ingress policy.

## Useful references

- Boost.Beast HTTP is built on Boost.Asio and supports asynchronous HTTP operations.
- NATS provides JetStream for durable messaging.
- PostgreSQL 16 provides the relational transaction/constraint layer.
- Next.js documents App Router as the newer routing model.

See the implementation notes in `docs/ARCHITECTURE.md`.
