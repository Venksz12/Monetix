CREATE DATABASE IF NOT EXISTS monetix;

CREATE TABLE IF NOT EXISTS monetix.api_telemetry (
    event_time DateTime64(3),
    request_id UUID,
    api_id String,
    consumer_id String,
    endpoint String,
    method LowCardinality(String),
    status_code UInt16,
    latency_ms UInt32,
    input_tokens UInt64 DEFAULT 0,
    output_tokens UInt64 DEFAULT 0,
    cost_minor Int64 DEFAULT 0
) ENGINE = MergeTree
ORDER BY (api_id,event_time,request_id);

CREATE MATERIALIZED VIEW IF NOT EXISTS monetix.api_latency_rollup
ENGINE = SummingMergeTree
ORDER BY (api_id,toStartOfMinute(event_time),status_code)
AS SELECT api_id,toStartOfMinute(event_time) AS minute,status_code,
count() AS requests,sum(latency_ms) AS latency_sum
FROM monetix.api_telemetry GROUP BY api_id,minute,status_code;
