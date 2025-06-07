.PHONY: help install test lint format check check-typing docs serve-docs clean

# Define help message
help:
	@echo "Chronovyan Development Commands:"
	@echo "  install     Install package in development mode with all dependencies"
	@echo "  test        Run tests with coverage"
	@echo "  lint        Run linters (black, isort, flake8)"
	@echo "  format      Format code with black and isort"
	@echo "  check       Run all checks (lint, type-check, test)"
	@echo "  check-typing  Run type checking with mypy"
	@echo "  docs        Build documentation"
	@echo "  serve-docs  Serve documentation locally"
	@echo "  clean       Clean build artifacts"

# Install package in development mode with all dependencies
install:
	pip install -e ".[dev]"
	pre-commit install

# Run tests with coverage
test:
	python -m pytest tests/ --cov=chronovyan --cov-report=term-missing --cov-report=xml

# Run linters (black, isort, flake8)
lint:
	black --check --diff .
	isort --check-only --diff .
	flake8 .

# Format code with black and isort
format:
	black .
	isort .

# Run all checks (lint, type-check, test)
check: lint check-typing test

# Run type checking with mypy
check-typing:
	mypy .

# Build documentation
docs:
	mkdocs build --clean

# Serve documentation locally
serve-docs:
	mkdocs serve

# Clean build artifacts
clean:
	rm -rf build/ dist/ *.egg-info/ .pytest_cache/ .coverage htmlcov/ .mypy_cache/
