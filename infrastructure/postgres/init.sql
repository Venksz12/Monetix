CREATE EXTENSION IF NOT EXISTS pgcrypto;

CREATE TABLE IF NOT EXISTS users (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 email TEXT UNIQUE NOT NULL,
 password_hash TEXT NOT NULL,
 role TEXT NOT NULL CHECK (role IN ('Consumer','Provider','Admin')),
 created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS providers (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 user_id UUID NOT NULL REFERENCES users(id),
 name TEXT NOT NULL,
 created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS apis (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 provider_id UUID REFERENCES providers(id),
 slug TEXT UNIQUE NOT NULL,
 name TEXT NOT NULL,
 category TEXT,
 description TEXT,
 created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS api_versions (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 api_id UUID NOT NULL REFERENCES apis(id) ON DELETE CASCADE,
 version TEXT NOT NULL,
 upstream_url TEXT NOT NULL,
 documentation TEXT,
 UNIQUE(api_id,version)
);
CREATE TABLE IF NOT EXISTS endpoints (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 api_version_id UUID NOT NULL REFERENCES api_versions(id) ON DELETE CASCADE,
 method TEXT NOT NULL,
 path TEXT NOT NULL,
 sla_p95_ms INTEGER NOT NULL DEFAULT 500,
 UNIQUE(api_version_id,method,path)
);
CREATE TABLE IF NOT EXISTS plans (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 api_id UUID NOT NULL REFERENCES apis(id),
 name TEXT NOT NULL,
 model TEXT NOT NULL CHECK (model IN ('payg','subscription','tiered','prepaid','ai_tokens')),
 currency CHAR(3) NOT NULL DEFAULT 'INR',
 unit_minor BIGINT NOT NULL DEFAULT 0,
 included_requests BIGINT NOT NULL DEFAULT 0,
 monthly_minor BIGINT NOT NULL DEFAULT 0,
 metadata JSONB NOT NULL DEFAULT '{}'
);
CREATE TABLE IF NOT EXISTS subscriptions (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 user_id UUID NOT NULL REFERENCES users(id),
 plan_id UUID NOT NULL REFERENCES plans(id),
 status TEXT NOT NULL DEFAULT 'active',
 started_at TIMESTAMPTZ NOT NULL DEFAULT now(),
 renews_at TIMESTAMPTZ
);
CREATE TABLE IF NOT EXISTS api_keys (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 user_id UUID NOT NULL REFERENCES users(id),
 key_hash CHAR(64) UNIQUE NOT NULL,
 environment TEXT NOT NULL CHECK (environment IN ('test','prod')),
 scopes TEXT[] NOT NULL DEFAULT '{}',
 revoked_at TIMESTAMPTZ
);
CREATE TABLE IF NOT EXISTS wallets (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 user_id UUID UNIQUE NOT NULL REFERENCES users(id),
 currency CHAR(3) NOT NULL DEFAULT 'INR',
 balance_minor BIGINT NOT NULL DEFAULT 0 CHECK (balance_minor >= 0)
);
CREATE TABLE IF NOT EXISTS ledger_transactions (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 external_ref TEXT,
 type TEXT NOT NULL CHECK (type IN ('CREDIT','DEBIT','REFUND','ADJUSTMENT')),
 account TEXT NOT NULL,
 amount_minor BIGINT NOT NULL CHECK (amount_minor >= 0),
 currency CHAR(3) NOT NULL,
 created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
 metadata JSONB NOT NULL DEFAULT '{}'
);
CREATE INDEX IF NOT EXISTS ledger_external_ref_idx ON ledger_transactions(external_ref);
CREATE TABLE IF NOT EXISTS processed_events (
 event_id TEXT PRIMARY KEY,
 processed_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS invoices (
 id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
 user_id UUID NOT NULL REFERENCES users(id),
 period_start DATE NOT NULL,
 period_end DATE NOT NULL,
 subtotal_minor BIGINT NOT NULL DEFAULT 0,
 tax_minor BIGINT NOT NULL DEFAULT 0,
 total_minor BIGINT NOT NULL DEFAULT 0,
 currency CHAR(3) NOT NULL DEFAULT 'INR',
 status TEXT NOT NULL DEFAULT 'open',
 created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE TABLE IF NOT EXISTS audit_logs (
 id BIGSERIAL PRIMARY KEY,
 actor_user_id UUID REFERENCES users(id),
 action TEXT NOT NULL,
 resource_type TEXT NOT NULL,
 resource_id TEXT,
 metadata JSONB NOT NULL DEFAULT '{}',
 created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE OR REPLACE FUNCTION prevent_ledger_update() RETURNS trigger LANGUAGE plpgsql AS $$
BEGIN RAISE EXCEPTION 'ledger_transactions is immutable'; END $$;
DROP TRIGGER IF EXISTS ledger_immutable_u ON ledger_transactions;
CREATE TRIGGER ledger_immutable_u BEFORE UPDATE OR DELETE ON ledger_transactions FOR EACH ROW EXECUTE FUNCTION prevent_ledger_update();

INSERT INTO users(email,password_hash,role) VALUES ('demo@monetix.local','demo','Consumer')
ON CONFLICT (email) DO NOTHING;
