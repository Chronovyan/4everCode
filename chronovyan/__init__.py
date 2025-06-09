"""
Forever - A temporal programming language for weaving time.

Forever provides a framework for working with time as a first-class construct,
allowing you to create, manage, and synchronize events across different points in time.
"""

__version__ = "0.1.0"

from .timeline import Timeline
from .event import Event

__all__ = ["Timeline", "Event"]
