MONETIX
Distributed API Marketplace and Usage-Based Monetization Platform


1. INTRODUCTION

Monetix is an academic project developed to create a distributed API marketplace and usage-based monetization platform.

The main purpose of Monetix is to provide a platform where API providers can publish their APIs and consumers can discover, subscribe to, and use those APIs.

The system also manages API authentication, rate limiting, subscriptions, quotas, usage tracking, pricing, wallet balance, billing, and analytics.

The project demonstrates how different technologies such as C++23, PostgreSQL, Redis, NATS JetStream, ClickHouse, and Next.js can work together to build a modern distributed application.


2. PROJECT OBJECTIVES

The main objectives of Monetix are:

- Create a marketplace where providers can publish APIs.
- Allow consumers to discover and subscribe to APIs.
- Provide secure API-key authentication.
- Control API access using rate limits and quotas.
- Support different API pricing models.
- Track API usage automatically.
- Calculate API usage costs.
- Manage consumer wallet balances.
- Maintain billing and transaction records.
- Provide usage and performance analytics.
- Demonstrate distributed-system and microservice concepts.


3. SYSTEM ARCHITECTURE

The overall architecture of Monetix is shown below.

                    +----------------------+
                    |      CONSUMER        |
                    |   Web / API Client   |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |     NEXT.JS PORTAL    |
                    | Marketplace /        |
                    | Dashboard / Playground|
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |     API GATEWAY       |
                    |       C++23           |
                    | Boost.Asio / Beast    |
                    +----------+-----------+
                               |
              +----------------+----------------+
              |                |                |
              v                v                v
       +-------------+  +-------------+  +-------------+
       |   Identity  |  | Marketplace |  |   Pricing   |
       |   Service   |  |   Service   |  |   Service   |
       +------+------+  +------+------+  +------+------+
              |                |                |
              +----------------+----------------+
                               |
                               v
                       +---------------+
                       |  PostgreSQL   |
                       | Main Database |
                       +---------------+

                               |
                               v
                       +---------------+
                       |     Redis     |
                       | Cache / Rate  |
                       |    Limiting   |
                       +---------------+

                               |
                               v
                       +---------------+
                       | NATS JetStream|
                       | Event System  |
                       +-------+-------+
                               |
                 +-------------+-------------+
                 |                           |
                 v                           v
          +-------------+              +-------------+
          |   Metering  |              |    Audit    |
          |   Service   |              |   Events    |
          +------+------+              +-------------+
                 |
                 v
          +-------------+
          |    Wallet   |
          |  & Billing  |
          +------+------+
                 |
                 v
          +-------------+
          |  Financial  |
          |   Ledger    |
          +-------------+

                 |
                 v
          +-------------+
          | ClickHouse  |
          |  Analytics  |
          +-------------+


4. API GATEWAY ARCHITECTURE

The API Gateway is one of the most important parts of Monetix.

It is implemented using C++23 with Boost.Asio and Boost.Beast.

The gateway receives API requests and performs several checks before forwarding the request to the provider.

The request flow is:

       Client Request
             |
             v
      HTTP Request Parsing
             |
             v
       API Key Extraction
             |
             v
       SHA-256 Hashing
             |
             v
      API Key Cache Lookup
             |
             v
       Rate Limit Check
             |
             v
     Subscription Check
             |
             v
        Quota Check
             |
             v
       Provider API
             |
             v
       API Response
             |
             v
      Usage Event Created
             |
             v
       NATS JetStream


The gateway also provides the following headers:

X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset
X-Monetix-Request-Id


5. IDENTITY AND AUTHENTICATION

The Identity Service manages users and authentication.

Monetix supports three main user roles:

- Consumer
- Provider
- Admin

Consumers use APIs.

Providers publish and manage APIs.

Administrators manage the platform.

The authentication system supports:

- User registration
- User login
- JWT authentication
- API keys
- Role-based access control
- Password hashing

Passwords are protected using Argon2id hashing.

API keys are hashed using SHA-256 before being used for secure lookup.


6. MARKETPLACE SERVICE

The Marketplace Service manages the APIs available on the platform.

Providers can add:

- API name
- Description
- Category
- Base URL
- Documentation
- API version
- Endpoints
- SLA information
- Pricing plans

An API can have multiple versions.

For example:

/v1
/v2

Each API version can contain multiple endpoints.


7. PRICING ENGINE

Monetix supports multiple pricing models.

PAY-AS-YOU-GO

The consumer pays for every API request.

Example:

100 requests x 5 paise
= 500 paise
= Rs. 5


MONTHLY SUBSCRIPTION

The consumer pays a fixed monthly amount and receives a predefined request allowance.

Example:

Monthly Price: Rs. 999
Included Requests: 50,000


TIERED PRICING

The price changes based on usage.

Example:

0 - 10,000 requests       Free
10,001 - 100,000          Normal Price
100,001+                  Discounted Price


PREPAID CREDITS

The consumer adds money to the wallet before using APIs.

Every API request deducts the required amount from the wallet.


AI TOKEN PRICING

AI APIs can be charged based on input and output token usage.

Example:

Input Tokens  x Input Price
+
Output Tokens x Output Price

= Total API Cost


8. USAGE METERING

After an API request is completed, the gateway creates an api.completed event.

The event can contain:

- Request ID
- Consumer ID
- Provider ID
- API ID
- API version
- Endpoint
- HTTP status
- Request latency
- Timestamp
- Input tokens
- Output tokens

The event is sent to NATS JetStream.

The Metering Service receives the event and calculates the usage.

The basic flow is:

        API Request
             |
             v
        API Gateway
             |
             v
       API Completed
           Event
             |
             v
       NATS JetStream
             |
             v
        Metering
             |
             v
        Pricing
             |
             v
         Billing
             |
             v
          Wallet


9. EVENT IDEMPOTENCY

In a distributed system, the same event may sometimes be received more than once.

If the same API usage event was processed twice, the consumer could be charged twice.

To avoid this problem, Monetix maintains a processed_events table.

The process is:

       Event Received
             |
             v
     Check Event ID
             |
        +----+----+
        |         |
        v         v
     Exists     New Event
        |         |
        v         v
      Ignore    Process
                  |
                  v
            Store Event ID

This prevents duplicate usage processing and duplicate billing.


10. WALLET SYSTEM

The Wallet Service manages consumer balances.

For financial calculations, Monetix uses integer minor units instead of floating-point values.

For example:

Rs. 10.50

is stored as:

1050 paise

This helps prevent floating-point precision problems.

The wallet supports:

- Credit
- Debit
- Refund
- Adjustment


11. FINANCIAL LEDGER

Monetix maintains financial transaction records using a double-entry style ledger.

The main transaction types are:

CREDIT
DEBIT
REFUND
ADJUSTMENT

A simple example is:

Consumer Wallet
       |
       | Debit
       v
Financial Ledger
       |
       | Credit
       v
Provider Revenue

Ledger records provide a history of financial activity.

The ledger is designed to keep financial records consistent and traceable.


12. PAYMENT WEBHOOKS

Monetix provides support for payment webhook verification.

The planned payment providers include:

- Razorpay
- Stripe

The webhook flow is:

       Payment Provider
              |
              v
        Webhook Request
              |
              v
     HMAC-SHA256 Check
              |
        +-----+-----+
        |           |
        v           v
     Invalid       Valid
        |           |
        v           v
      Reject      Process
                    |
                    v
               Add Wallet
                    |
                    v
               Add Ledger
                    |
                    v
               Update Status


13. REDIS

Redis is used for fast temporary and shared information.

Main uses include:

- API key caching
- Rate limiting
- Token bucket information
- Quota counters
- Temporary cached data

Multiple API Gateway instances can share Redis.

Example:

Gateway 1 --------+
                  |
Gateway 2 --------+------ Redis
                  |
Gateway 3 --------+

This allows the system to maintain shared rate-limit information.


14. RATE LIMITING

Rate limiting controls how many requests a consumer can send within a given period.

For example:

100 requests per minute

If the consumer exceeds the limit, the gateway returns:

HTTP 429 Too Many Requests

The gateway also provides:

X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset

This protects provider APIs from excessive traffic.


15. NATS JETSTREAM

NATS JetStream is used as the event messaging system.

Important events include:

api.requested
api.completed

The gateway publishes events and other services consume them.

The basic architecture is:

              API Gateway
                   |
                   v
             NATS JetStream
                   |
        +----------+----------+
        |          |          |
        v          v          v
     Metering    Billing    Audit
        |
        v
     Analytics


This makes the system event-driven and reduces direct dependency between the API Gateway and background services.


16. POSTGRESQL

PostgreSQL is used as the main relational database.

Important tables include:

users
providers
apis
api_versions
endpoints
plans
subscriptions
api_keys
wallets
ledger_transactions
processed_events
invoices
audit_logs

PostgreSQL is mainly responsible for important application and financial information.


17. CLICKHOUSE

ClickHouse is used for storing and analyzing large amounts of API usage information.

It can be used to calculate:

- Total requests
- Successful requests
- Failed requests
- API latency
- P50 latency
- P95 latency
- P99 latency
- Traffic by endpoint
- Traffic by API
- Traffic by status code
- Usage trends


18. FRONTEND

The Monetix frontend is developed using:

- Next.js 14
- TypeScript
- React
- Tailwind CSS

The frontend contains different sections for consumers and providers.


19. MARKETPLACE EXPLORER

The Marketplace allows consumers to:

- Search APIs
- Browse API categories
- View API information
- View pricing plans
- View API versions
- View documentation
- Subscribe to APIs


20. API PLAYGROUND

The API Playground allows consumers to test APIs directly from the web portal.

It can display:

- Request status
- Response data
- Request latency
- Estimated cost
- Remaining quota
- Request ID

This makes it easier for consumers to test an API before integrating it into their applications.


21. CONSUMER DASHBOARD

The Consumer Dashboard provides information about API usage.

It includes:

- Active subscriptions
- API keys
- Test keys
- Production keys
- Permission scopes
- Wallet balance
- Usage information
- Quota information


22. PROVIDER DASHBOARD

The Provider Dashboard allows API providers to manage and monitor their APIs.

It includes:

- API information
- API versions
- Endpoints
- Consumer information
- Request statistics
- Revenue information
- Latency information
- Performance information


23. SECURITY

Security is an important part of Monetix.

The project includes:

- API key authentication
- SHA-256 API-key hashing
- JWT authentication
- Argon2id password hashing
- Role-based access control
- Rate limiting
- API scopes
- HMAC-SHA256 webhook verification
- Audit logging

In a real production deployment, additional security measures such as HTTPS, secure secret storage, and stronger service-to-service authentication would be required.


24. COMPLETE REQUEST FLOW

The complete Monetix request flow can be summarized as:

                 Consumer
                    |
                    v
             Next.js Portal
                    |
                    v
              API Gateway
                    |
        +-----------+-----------+
        |           |           |
        v           v           v
   Authentication Rate Limit  Quota
        |           |           |
        +-----------+-----------+
                    |
                    v
              Provider API
                    |
                    v
               Response
                    |
                    v
             Usage Event
                    |
                    v
             NATS JetStream
                    |
                    v
               Metering
                    |
                    v
               Pricing
                    |
                    v
                Wallet
                    |
                    v
                Ledger
                    |
                    v
              Analytics


25. PROJECT TECHNOLOGIES

Backend:

C++23
Boost.Asio
Boost.Beast
Drogon
OpenSSL

Database:

PostgreSQL 16

Caching and Rate Limiting:

Redis 7

Messaging:

NATS JetStream

Analytics:

ClickHouse

Frontend:

Next.js 14
TypeScript
React
Tailwind CSS

Infrastructure:

Docker
Docker Compose

Testing:

C++ Unit Tests
Python Integration Tests
k6 Load Testing


26. PROJECT STRUCTURE

```text
monetix/
|
+-- backend/
|   |
|   +-- CMakeLists.txt
|   |
|   +-- gateway/
|   |   +-- CMakeLists.txt
|   |   +-- include/
|   |   +-- src/
|   |       +-- main.cpp
|   |       +-- http_gateway.cpp
|   |       +-- auth_interceptor.cpp
|   |       +-- rate_limiter.cpp
|   |       +-- nats_publisher.cpp
|   |
|   +-- services/
|   |   +-- CMakeLists.txt
|   |   +-- identity/
|   |   +-- marketplace/
|   |   +-- pricing/
|   |   +-- metering/
|   |   +-- wallet/
|   |
|   +-- tests/
|       +-- unit_tests.cpp
|
+-- frontend/
|   +-- package.json
|   +-- tsconfig.json
|   +-- tailwind.config.js
|   +-- app/
|   +-- components/
|
+-- infrastructure/
|   +-- docker-compose.yml
|   +-- postgres/
|   |   +-- init.sql
|   +-- clickhouse/
|   |   +-- init.sql
|   +-- nats/
|       +-- nats-server.conf
|
+-- tests/
|   +-- k6-load-test.js
|   +-- integration_test.py
|
+-- Makefile
+-- README.md
+-- setup.sh
```



27. REQUIREMENTS

The recommended environment is:

- Linux, macOS, or Windows with WSL2
- Docker
- Docker Compose
- Git
- CMake
- C++23 compiler
- Node.js 20 or later
- Python 3
- k6 for load testing

Docker is recommended because Monetix uses multiple infrastructure services.


28. RUNNING THE PROJECT

Open the Monetix project directory:

cd monetix

Make the setup script executable:

chmod +x setup.sh

Run the setup script:

./setup.sh

Start the infrastructure:

docker compose -f infrastructure/docker-compose.yml up --build


29. TESTING

The project contains unit tests, integration tests, and load tests.
C++ Unit Tests:
make test

Python Integration Test:
python3 tests/integration_test.py

k6 Load Test:
k6 run tests/k6-load-test.js


30. SAMPLE API REQUEST

A basic gateway request can be tested using:
curl -H "X-API-Key: demo_monetix_key" http://localhost:8080/demo/hello
The gateway can return information such as:

X-Monetix-Request-Id
X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset


31. ACADEMIC EXPERIMENTS

Monetix can be used for different academic experiments.

Rate Limiting:
Configure a request limit and send more requests than the allowed amount. Check how the gateway responds with HTTP 429.

Duplicate Events:
Send the same api.completed event multiple times and verify that it is processed only once.

Pricing:
Generate different amounts of API usage and check whether the correct pricing tier is applied.

Wallet:
Perform credits, debits, refunds, and adjustments and verify the final wallet balance.

Performance:
Use k6 to generate multiple concurrent requests and measure throughput and latency.

Scaling:
Run multiple gateway instances and compare the performance with a single gateway instance.


32. DISTRIBUTED SYSTEM CONCEPTS

Monetix demonstrates several important distributed-system concepts.

MICROSERVICES
Different responsibilities are divided into separate services.

EVENT-DRIVEN PROCESSING
NATS JetStream is used to send usage events between components.

ASYNC PROCESSING
Usage and billing can be processed separately from the main API request.

CACHING
Redis is used to reduce the need for repeated database lookups.

IDEMPOTENCY
Processed events are recorded so that duplicate events do not cause duplicate billing.

SCALABILITY
The gateway can be deployed as multiple instances.

CONSISTENCY
PostgreSQL transactions are used for important application and financial operations.

OBSERVABILITY
Request IDs and analytics data help in tracking API requests and performance.

33. LIMITATIONS
Monetix is mainly an academic project.
A complete production system would require additional features such as:

- Kubernetes
- High-availability databases
- Redis clustering
- NATS clustering
- HTTPS and TLS
- Distributed tracing
- Advanced monitoring
- Production payment integration
- Advanced security
- Fraud detection
- Disaster recovery
- Multi-region deployment

These features can be added in future versions.

34. FUTURE ENHANCEMENTS
Future improvements may include:

- Kubernetes deployment
- API health monitoring
- Automatic provider failover
- Advanced analytics
- OpenTelemetry tracing
- Prometheus and Grafana monitoring
- Additional payment gateways
- Automated invoice generation
- Fraud detection
- Multi-region support
- Dynamic pricing
- API recommendation system
- Mobile application


35. ACADEMIC SIGNIFICANCE
Monetix is useful for studying several areas of computer science.
The project demonstrates concepts related to:

- Distributed Systems
- Microservices
- Cloud Computing
- Computer Networks
- Database Systems
- API Management
- Software Architecture
- Cybersecurity
- Financial Technology
- Event-Driven Systems
- Performance Testing

The project shows how multiple independent services can work together to provide a single platform.


36. CONCLUSION

Monetix is a distributed API marketplace and usage-based monetization platform developed for academic purposes.
It provides a complete flow from API discovery and authentication to API consumption, usage tracking, pricing, wallet management, billing, and analytics.
The API Gateway handles the main request processing, while NATS JetStream allows usage information to be processed asynchronously.
PostgreSQL stores important application data, Redis provides fast caching and rate limiting, ClickHouse handles analytics, and the Next.js frontend provides an interface for consumers and providers.
Overall, Monetix demonstrates how modern distributed-system technologies can be combined to build a scalable, secure, and modular API marketplace.
The project can also be extended in the future with additional security, monitoring, scalability, payment, and cloud-deployment features.
