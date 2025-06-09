"""Pytest configuration and fixtures for Chronovyan tests."""
import sys
from pathlib import Path

# Add the project root to the Python path
project_root = str(Path(__file__).parent.resolve())
sys.path.insert(0, project_root)

# Common test fixtures can be defined here

# You can add common fixtures that will be available to all tests
# For example:
# @pytest.fixture
def example_fixture():
    """Example fixture that returns a value."""
    return "example"
