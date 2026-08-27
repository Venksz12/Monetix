-- Atomic Redis token bucket (EVALSHA in production).
-- KEYS[1] = bucket key
-- ARGV[1] = capacity, ARGV[2] = refill_per_second, ARGV[3] = now_ms
local cap=tonumber(ARGV[1]); local rate=tonumber(ARGV[2]); local now=tonumber(ARGV[3])
local v=redis.call('HMGET',KEYS[1],'tokens','ts')
local tokens=tonumber(v[1]) or cap
local ts=tonumber(v[2]) or now
tokens=math.min(cap,tokens+((now-ts)/1000.0)*rate)
if tokens < 1 then
  redis.call('HSET',KEYS[1],'tokens',tokens,'ts',now); redis.call('PEXPIRE',KEYS[1],60000)
  return {0,math.floor(tokens)}
end
tokens=tokens-1
redis.call('HSET',KEYS[1],'tokens',tokens,'ts',now); redis.call('PEXPIRE',KEYS[1],60000)
return {1,math.floor(tokens)}
