MONETIX
Distributed API Marketplace and Usage Based Monetization Platform

PROJECT DESCRIPTION

Monetix is an academic project developed to demonstrate how a modern distributed platform can be used to publish, discover, access, monitor, and monetize application programming interfaces (APIs). The main purpose of the project is to bring API management and usage-based billing together in one platform.

In Monetix, API providers can register their APIs, define versions and endpoints, configure service-level information, and create pricing plans. API consumers can browse available APIs, subscribe to plans, generate API keys, use APIs through the gateway, monitor their usage, and manage their wallet balance.

The system is designed using a distributed microservices architecture. The API Gateway handles incoming requests and performs authentication, rate limiting, subscription verification, quota checking, request routing, and request identification. Usage information is then published asynchronously through NATS JetStream so that metering and billing can be performed independently of the main request path.

The project uses C++23 for the backend, Boost.Asio and Boost.Beast for asynchronous gateway networking, Drogon for backend services, PostgreSQL for transactional data, Redis for caching and rate limiting, NATS JetStream for asynchronous event communication, ClickHouse for analytics, and Next.js with TypeScript for the web portal.


1. MAIN OBJECTIVES

The main objectives of Monetix are:

1. To create a centralized marketplace for APIs.
2. To provide secure authentication and authorization for API consumers and providers.
3. To implement API-key based access through a high-performance gateway.
4. To provide distributed rate limiting using Redis.
5. To verify subscriptions and usage quotas before forwarding requests.
6. To support multiple API pricing and monetization models.
7. To process API usage asynchronously using an event-driven architecture.
8. To prevent duplicate billing through idempotent event processing.
9. To maintain accurate financial information using integer minor-unit arithmetic.
10. To implement a double-entry financial ledger.
11. To provide API usage and performance analytics.
12. To provide separate dashboards for consumers and providers.
13. To demonstrate important distributed-systems concepts in a practical academic application.


2. SYSTEM ARCHITECTURE

Monetix follows a modular distributed architecture consisting of a frontend, API gateway, backend microservices, databases, caching infrastructure, messaging infrastructure, and analytics storage.

The major components are:

Frontend Portal
API Gateway
Identity and Authentication Service
Marketplace and Registry Service
Pricing Engine Service
Metering and Usage Worker
Wallet and Ledger Service
PostgreSQL
Redis
NATS JetStream
ClickHouse

The frontend communicates with the backend through HTTP APIs. The API Gateway is the main entry point for API consumption. It performs the security and policy checks before forwarding requests to registered upstream APIs.

The system separates synchronous request processing from asynchronous usage processing. This allows the gateway to remain focused on fast request handling while metering and billing are performed through events.


3. API GATEWAY

The API Gateway is implemented using C++, Boost.Asio, and Boost.Beast.

It is designed as a high performance asynchronous HTTP gateway and reverse proxy.

 The gateway request pipeline is:

Client Request
        |
        v
HTTP Request Parsing
        |
        v
API Key Extraction
        |
        v
SHA-256 API Key Hashing
        |
        v
API Key Cache / Validation
        |
        v
Redis Rate Limiting
        |
        v
Subscription Verification
        |
        v
Quota Verification
        |
        v
Upstream API Routing
        |
        v
Upstream Response
        |
        v
Usage Event Publishing

The gateway generates a unique request ID for every request. This identifier can be used to correlate gateway activity, usage events, and analytics records.

The gateway also returns the following headers:

X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset
X-Monetix-Request-Id

The gateway publishes asynchronous events such as api.requested and api.completed through NATS JetStream.


4. IDENTITY AND AUTHENTICATION SERVICE

The Identity Service is responsible for managing users and authentication.

The service supports three primary roles:

CONSUMER
PROVIDER
ADMIN

Consumers use the platform to discover and consume APIs.

Providers use the platform to publish APIs and manage their services.

Administrators can perform management and administrative operations.

The authentication design supports:

User registration
User login
JWT token issuance
JWT validation
Password hashing
Role based access control
Authorization

Passwords are designed to be protected using Argon2id hashing rather than storing plaintext passwords.

JWT tokens are used for authenticated application requests.


5. MARKETPLACE AND REGISTRY SERVICE

The Marketplace and Registry Service manages APIs available on the Monetix platform.

A provider can register an API with information such as:

API name
Description
Category
Provider
Base URL
Documentation
SLA information
Status

Each API can have multiple versions.

Each version can contain multiple endpoints.

The registry is responsible for maintaining the relationship between:

Provider
API
API Version
Endpoint
Pricing Plan


6. PRICING ENGINE

The Pricing Engine calculates the cost of API consumption.

Monetix supports several pricing models.

MODEL 1: PAY-AS-YOU-GO

In this model, the consumer pays for individual API requests.

For example:

Price per request = 5 paise

If the consumer makes 100 requests:

100 x 5 paise = 500 paise

Therefore, the total cost is 5 rupees.

MODEL 2: MONTHLY SUBSCRIPTION

In this model, the consumer pays a fixed monthly amount and receives a predefined request allowance.

Example:

Monthly price = Rs. 999
Included requests = 50,000

The subscription can define what happens when the included quota is exhausted.

MODEL 3: TIERED VOLUME PRICING

In this model, the price depends on the total request volume.

Example:

0 to 10,000 requests = Free
10,001 to 100,000 requests = Standard rate
Above 100,000 requests = Discounted rate

This model encourages higher-volume usage while providing lower effective pricing at larger scales.

MODEL 4: PREPAID CREDITS

Consumers can purchase credits in advance.

The wallet stores the available balance and each API request deducts the required amount.

MODEL 5: AI TOKEN PRICING

AI-based APIs can be priced according to input and output tokens.

For example:

Input tokens x input-token price
+
Output tokens x output-token price

This model is suitable for AI inference and language-model APIs.


7. USAGE METERING

The Monetix Gateway publishes an api.completed event after processing an API request.

The event can contain information such as:

Request ID
Consumer ID
Provider ID
API ID
API version
Endpoint
HTTP status code
Latency
Timestamp
Input token count
Output token count

The Metering Service consumes these events from NATS JetStream.

The metering process calculates the usage associated with each API request and provides the information required for pricing and billing.

This design prevents the main API request path from being tightly coupled to billing operations.


8. IDEMPOTENT EVENT PROCESSING

Distributed messaging systems can deliver an event more than once. If the same API completion event were billed twice, the consumer could be incorrectly charged.

Monetix addresses this using an event de-duplication mechanism.

The processed_events table stores identifiers of events that have already been processed.

The processing flow is:

Event received
        |
        v
Check processed_events
        |
        +---- Event already exists
        |             |
        |             v
        |          Ignore
        |
        v
Process usage
        |
        v
Calculate cost
        |
        v
Update financial records
        |
        v
Store event identifier

This provides idempotent processing and helps prevent duplicate billing.


9. WALLET AND FINANCIAL SYSTEM

The Wallet Service manages consumer balances and financial transactions.

Monetary values are represented using integer minor units instead of floating-point values.

For example:

Rs. 10.50

is represented as:

1050 paise

This approach avoids common floating-point precision problems in financial calculations.

The primary representation is int64_t.

The wallet system supports:

Credits
Debits
Refunds
Adjustments

Wallet operations are associated with ledger transactions so that financial activity can be audited.


10. DOUBLE-ENTRY LEDGER

Monetix uses a double-entry style financial ledger.

The main transaction types are:

CREDIT
DEBIT
REFUND
ADJUSTMENT

The ledger is intended to remain immutable after a financial transaction has been recorded.

A simplified example is:

Consumer Wallet
       |
       | DEBIT
       v
Financial Ledger
       |
       | CREDIT
       v
Provider Revenue

This provides a traceable financial history and makes it possible to reconstruct account balances from transaction records.

Integer arithmetic and transactional database operations are used to improve financial correctness.


11. PAYMENT WEBHOOKS

The Wallet Service provides a design for receiving payment notifications from payment providers such as Razorpay and Stripe.

A webhook request is not accepted simply because it comes from an external service.

The signature is verified using HMAC-SHA256.

The basic flow is:

Payment Provider
       |
       v
Webhook Request
       |
       v
HMAC Signature Verification
       |
       +---- Invalid
       |       |
       |       v
       |     Reject
       |
       v
Validate Event
       |
       v
Credit Wallet
       |
       v
Create Ledger Entry

In a production deployment, webhook secrets should be stored securely and never committed to source control.


12. POSTGRESQL DATABASE

PostgreSQL is used for transactional and relational data.

The project includes tables for:

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

The database maintains relationships between users, providers, APIs, subscriptions, wallets, and financial transactions.

PostgreSQL is particularly important for operations that require transactional consistency, such as wallet updates and ledger operations.


13. REDIS

Redis is used for high-speed distributed state.

The main Redis responsibilities are:

API-key cache
Rate limiting
Token bucket state
Quota counters
Short-lived cached information

Redis allows multiple gateway instances to share rate-limit state.

For example:

Gateway Instance 1 ----+
                       |
Gateway Instance 2 ----+---- Redis
                       |
Gateway Instance 3 ----+

This is important because an individual in-memory counter on each gateway would not provide a globally consistent rate limit.


14. RATE LIMITING

Monetix uses a token-bucket style rate-limiting approach.

For example, an API plan may allow:

100 requests per minute.

The gateway checks the available tokens before forwarding a request.

If sufficient tokens are available, the request continues.

If the limit has been exceeded, the gateway returns:

HTTP 429 Too Many Requests

The response also provides rate-limit information through:

X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset

This mechanism helps protect upstream APIs from excessive traffic and allows providers to enforce subscription policies.


15. NATS JETSTREAM

NATS JetStream is used as the asynchronous messaging layer.

Important events include:

api.requested
api.completed

The purpose of using asynchronous messaging is to separate request processing from background operations such as:

Usage metering
Billing
Audit processing
Analytics processing

This reduces direct coupling between services.

The architecture can therefore be represented as:

Gateway
   |
   v
NATS JetStream
   |
   +---- Metering
   |
   +---- Billing
   |
   +---- Audit
   |
   +---- Analytics


16. CLICKHOUSE ANALYTICS

ClickHouse is used for high-volume request telemetry and analytics.

The system can store information such as:

Request timestamp
API
Endpoint
API version
Consumer
Provider
HTTP status
Latency
Request ID
Token usage
Cost

The analytics layer can be used to calculate:

Request count
Success rate
Error rate
P50 latency
P95 latency
P99 latency
Traffic by endpoint
Traffic by API version
Traffic by HTTP status
Revenue trends

ClickHouse is separated from PostgreSQL because analytical workloads can be very different from transactional workloads.


17. FRONTEND PORTAL

The frontend is implemented using:

Next.js 14
TypeScript
React
Tailwind CSS

The portal provides interfaces for both API consumers and API providers.


18. MARKETPLACE EXPLORER

The Marketplace Explorer allows consumers to:

Search APIs
Browse API categories
View API descriptions
View pricing plans
View versions
Inspect provider information
Subscribe to APIs

The goal is to provide an API marketplace experience similar to a service catalog.


19. API PLAYGROUND

The Interactive API Playground allows consumers to test APIs directly from the browser.

The interface can display:

HTTP status code
Request latency
Estimated request cost
Remaining quota
Response data
Monetix request ID

This helps users understand the behavior and cost of an API before integrating it into their own applications.


20. CONSUMER DASHBOARD

The Consumer Dashboard provides information about API consumption.

Typical sections include:

Active subscriptions
API keys
Test keys
Production keys
Permission scopes
Wallet balance
Usage statistics
Request history
Quota information

The dashboard gives consumers a centralized view of their API activity.


21. PROVIDER DASHBOARD

The Provider Dashboard provides API owners with information about their services.

Typical information includes:

Registered APIs
API versions
Endpoints
Consumer activity
Request volume
Revenue
Endpoint latency
Status-code distribution
Performance metrics

This allows providers to understand how their APIs are being used.


22. SECURITY

Security is an important part of the Monetix design.

The project includes or provides design support for:

SHA-256 API-key hashing
Argon2id password hashing
JWT authentication
Role-based access control
API-key scopes
HMAC-SHA256 webhook verification
Rate limiting
Audit logging
Request identifiers
Immutable financial records

Production deployments should additionally use HTTPS/TLS, secure secret management, database encryption where appropriate, strict network policies, and proper service-to-service authentication.


23. DISTRIBUTED SYSTEM CONCEPTS

Monetix demonstrates several important distributed-systems concepts.

ASYNCHRONOUS COMMUNICATION

NATS JetStream allows services to communicate through persistent events rather than requiring synchronous communication for every operation.

IDEMPOTENCY

The processed_events table prevents duplicate processing of the same usage event.

HORIZONTAL SCALABILITY

Multiple gateway instances can share Redis state and process requests concurrently.

FAULT ISOLATION

The architecture separates gateway, identity, marketplace, pricing, metering, and wallet responsibilities.

CONSISTENCY

PostgreSQL transactions are used for operations that require reliable state changes.

FINANCIAL CORRECTNESS

Integer minor-unit arithmetic prevents floating-point rounding errors in monetary calculations.

OBSERVABILITY

Request IDs and analytics telemetry help identify and analyze individual API requests.

EVENT-DRIVEN ARCHITECTURE

Usage and billing workflows are decoupled from the synchronous API request path.


24. PROJECT DIRECTORY STRUCTURE

monetix/

backend/
    CMakeLists.txt

    gateway/
        CMakeLists.txt
        include/
        src/
            main.cpp
            http_gateway.cpp
            auth_interceptor.cpp
            rate_limiter.cpp
            nats_publisher.cpp

    services/
        CMakeLists.txt
        identity/
        marketplace/
        pricing/
        metering/
        wallet/

    tests/
        unit_tests.cpp

frontend/
    package.json
    tsconfig.json
    tailwind.config.js
    app/
        layout.tsx
        page.tsx
        marketplace/
            page.tsx
        playground/
            page.tsx
        dashboard/
            consumer/
                page.tsx
            provider/
                page.tsx
    components/

infrastructure/
    docker-compose.yml
    postgres/
        init.sql
    clickhouse/
        init.sql
    nats/
        nats-server.conf

tests/
    k6-load-test.js
    integration_test.py

Makefile
README.md
setup.sh


25. REQUIREMENTS

The recommended development environment includes:

Linux, macOS, or Windows with WSL2
Docker
Docker Compose
Git
CMake
A C++23 compatible compiler
Node.js 20 or later
Python 3
k6 for load testing

Docker is recommended because the project depends on several infrastructure components.


26. RUNNING THE PROJECT

After extracting the project, enter the Monetix directory:

cd monetix

Make the setup script executable:

chmod +x setup.sh

Run:

./setup.sh

The infrastructure can then be started with:

docker compose -f infrastructure/docker-compose.yml up --build

The exact service availability depends on the local environment and build configuration.


27. TESTING

The project includes unit, integration, and load-testing components.

C++ UNIT TESTS

The backend includes unit tests for important core functionality.

Example:

make test

INTEGRATION TESTS

Python integration tests are available at:

tests/integration_test.py

Example:

python3 tests/integration_test.py

LOAD TESTING

A k6 load-testing script is available at:

tests/k6-load-test.js

Example:

k6 run tests/k6-load-test.js

The load test can be used to measure throughput, latency, error rates, and gateway behavior under concurrent traffic.


28. SAMPLE API REQUEST

A simple gateway request can be tested using:

curl -H "X-API-Key: demo_monetix_key" http://localhost:8080/demo/hello

The gateway should process the request according to the configured authentication, rate-limit, subscription, and routing rules.

A successful response may include:

X-Monetix-Request-Id
X-RateLimit-Limit
X-RateLimit-Remaining
X-RateLimit-Reset


29. ACADEMIC EXPERIMENTS

Monetix can be used to conduct several experiments for academic evaluation.

EXPERIMENT 1: RATE LIMITING

Configure an API with a fixed request limit.

Generate requests above the configured limit.

Observe that requests within the limit are accepted while excess requests receive HTTP 429.

EXPERIMENT 2: DUPLICATE EVENT PROCESSING

Publish the same api.completed event multiple times.

Verify that the metering system processes the event only once.

EXPERIMENT 3: PRICING MODELS

Generate different request volumes and verify that the pricing engine applies the correct pricing tier.

EXPERIMENT 4: WALLET TRANSACTIONS

Perform credits, debits, refunds, and adjustments.

Verify that the wallet balance matches the corresponding ledger transactions.

EXPERIMENT 5: PERFORMANCE

Use k6 to increase the number of concurrent clients.

Measure:

Requests per second
Average latency
P50 latency
P95 latency
P99 latency
Error rate

EXPERIMENT 6: HORIZONTAL SCALING

Run multiple gateway instances and compare the throughput and latency with a single gateway instance.


30. FAILURE SCENARIOS

The system can also be evaluated under infrastructure failures.

REDIS FAILURE

Investigate how rate-limiting behavior changes when Redis becomes unavailable.

NATS FAILURE

Investigate event publication and recovery behavior when NATS becomes temporarily unavailable.

DATABASE FAILURE

Investigate how transactional operations behave when PostgreSQL becomes unavailable.

UPSTREAM FAILURE

Simulate an unavailable API provider and observe gateway error handling.

DUPLICATE EVENTS

Send repeated usage events and verify that billing is not duplicated.


31. PERFORMANCE CONSIDERATIONS

The gateway is designed around asynchronous I/O using Boost.Asio and Boost.Beast.

The main design goal is to avoid unnecessary blocking operations in the request path.

Redis provides low-latency shared state for rate limiting and caching.

NATS JetStream moves non-critical background processing away from the synchronous request path.

ClickHouse is used for analytical workloads instead of placing high-volume telemetry queries directly on the transactional PostgreSQL database.

These decisions allow the architecture to be evaluated for throughput and scalability.


32. DATA CONSISTENCY

Different components of Monetix have different consistency requirements.

Financial operations require strong transactional consistency.

API telemetry can tolerate asynchronous processing.

Caching can tolerate temporary staleness depending on the use case.

Usage events require durable delivery and idempotent processing.

This separation allows the system to balance performance and consistency rather than treating every operation identically.


33. LIMITATIONS

This project is primarily intended for academic learning, demonstration, experimentation, and evaluation.

These include:

Production-grade Kubernetes deployment
High-availability database configuration
Redis clustering
NATS clustering
TLS and mTLS
OpenTelemetry tracing
Prometheus and Grafana monitoring
Advanced circuit breakers
Dead-letter queues
Production secret management
Comprehensive API contract testing
Full payment-provider production integration
Fraud detection
Tax handling
Provider settlement
Multi-region deployment
Advanced disaster recovery
Security auditing
Compliance controls

These limitations provide opportunities for future research and development.


34. FUTURE ENHANCEMENTS

Possible future enhancements include:

Kubernetes deployment
Automatic API health checking
Provider failover
Multi-region gateway deployment
OpenTelemetry integration
Prometheus metrics
Grafana dashboards
Advanced fraud detection
Automated invoice generation
Tax calculation
Provider revenue settlement
Additional payment providers
API quality scoring
Advanced SLA enforcement
Machine-learning based usage prediction
Dynamic pricing
API recommendation systems
WebSocket support
GraphQL support


35. ACADEMIC SIGNIFICANCE

Monetix is relevant to multiple areas of computer science and software engineering.

The project demonstrates practical concepts from:

Distributed Systems
Cloud Computing
Microservices Architecture
Computer Networks
Database Management Systems
Software Architecture
Financial Technology
API Management
Performance Engineering
Event-Driven Systems
Security Engineering

The project is especially useful for studying how synchronous and asynchronous processing can be combined in a distributed application.

The gateway provides the low-latency synchronous path, while NATS JetStream enables asynchronous metering and billing workflows.


36. CONCLUSION

Monetix is a distributed API marketplace and usage based monetization platform designed as an academic implementation of a modern cloud oriented system.

The platform combines API discovery, authentication, rate limiting, subscriptions, quotas, pricing, usage metering, wallets, financial ledgers, payment verification, and analytics.

The most important architectural principle is the separation between fast API request processing and asynchronous usage processing. The gateway focuses on handling requests efficiently, while NATS JetStream allows metering and billing services to process usage independently.

The use of PostgreSQL provides transactional data management, Redis provides fast shared state, ClickHouse provides scalable analytics, and the Next.js portal provides a user-facing interface for consumers and providers.

Overall, Monetix demonstrates how distributed systems, asynchronous messaging, microservices, financial correctness, and high-performance networking can be combined to create a realistic API economy platform.


37. PROJECT PURPOSE

The primary purpose of this project is academic.

It is intended to demonstrate the design and implementation of a distributed system rather than represent a fully certified commercial payment or API management product.

The project can therefore be extended, benchmarked, tested, and modified to study different distributed system strategies and performance characteristics.


38. PROJECT INFORMATION

Primary Backend Language:
C++

Frontend:
Next.js and TypeScript

Database:
PostgreSQL

Cache and Rate Limiting:
Redis

Messaging:
NATS JetStream

Analytics:
ClickHouse

Deployment:
Docker / Docker Compose

Testing:
C++ Unit Tests, Python Integration Tests, k6 Load Tests
