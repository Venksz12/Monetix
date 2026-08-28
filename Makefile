SHELL := /bin/bash
COMPOSE := docker compose -f infrastructure/docker-compose.yml

.PHONY: up down build test integration load clean

up:
	$(COMPOSE) up --build

down:
	$(COMPOSE) down -v

build:
	$(COMPOSE) build

test:
	cmake -S backend -B backend/build -DCMAKE_BUILD_TYPE=Release
	cmake --build backend/build -j
	backend/build/monetix_tests

integration:
	python3 tests/integration_test.py

load:
	k6 run tests/k6-load-test.js

clean:
	rm -rf backend/build frontend/.next

