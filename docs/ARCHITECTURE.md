# Monetix Architecture Notes

## Request lifecycle

1. Gateway parses HTTP.
2. `X-API-Key` is hashed with SHA-256; only the hash is used as the cache/database lookup key.
3. A token bucket is checked. Redis is the intended distributed implementation; an in-process bucket is used as a safe development fallback when Redis is unavailable.
4. Subscription/quota policy is resolved by the marketplace service.
5. Gateway forwards to the configured upstream.
6. `api.requested` and `api.completed` events are published to NATS.
7. Metering consumes `api.completed`, de-duplicates by event id, and records usage.
8. Pricing calculates charge in integer minor units.
9. Wallet service posts an immutable debit/credit pair to the ledger.

## Money

All monetary values are `int64` minor units. For INR, `100` means ₹1.00; for USD, `100` means $1.00. Never use floating-point values in ledger calculations.

## Ledger invariant

Every financial transaction has at least one debit and one credit entry. The sum of signed entries in a transaction is zero. Rows are append-only; corrections are represented by compensating entries.

## Idempotency

`processed_events(event_id primary key, processed_at)` is the authoritative de-duplication gate for metering. The event is acknowledged only after the transaction that records the event and usage has committed.

## Failure modes

- Redis unavailable: gateway uses bounded local token bucket and exposes a degraded health signal.
- NATS unavailable: gateway continues the synchronous request path but logs the event-publish failure; deployments that require billing correctness should instead fail closed or use an outbox.
- Upstream timeout: gateway returns 504 and still emits a completion event.
- Duplicate completion event: metering's primary key prevents double billing.
