"""
The Event class represents an event that can be scheduled on a timeline.
"""
from typing import Any, Callable, Optional
from dataclasses import dataclass, field
import time

@dataclass
class Event:
    """
    An event that can be scheduled on a timeline.
    
    Args:
        name: A name for the event (for debugging and logging)
        data: Optional data to associate with the event
        on_trigger: Optional callback function to call when the event is triggered
        delay: Optional delay in seconds before the event occurs (can also be set when adding to timeline)
    """
    name: str
    data: Any = None
    on_trigger: Optional[Callable[['Event'], None]] = None
    delay: float = 0.0
    
    # Internal state
    _trigger_time: Optional[float] = field(init=False, default=None)
    _is_triggered: bool = field(init=False, default=False)
    
    def __post_init__(self):
        """Initialize the event with the current time as creation time."""
        self._creation_time = time.monotonic()
    
    def trigger(self) -> None:
        """
        Trigger the event.
        
        This calls the on_trigger callback if one was provided.
        """
        self._trigger_time = time.monotonic()
        self._is_triggered = True
        
        if self.on_trigger is not None:
            self.on_trigger(self)
    
    @property
    def is_triggered(self) -> bool:
        """Check if the event has been triggered."""
        return self._is_triggered
    
    @property
    def timestamp(self) -> Optional[float]:
        """
        Get the time when the event was triggered.
        
        Returns:
            The timestamp when the event was triggered, or None if it hasn't been triggered yet.
        """
        return self._trigger_time
    
    @property
    def age(self) -> float:
        """
        Get the age of the event in seconds.
        
        Returns:
            The number of seconds since the event was created.
        """
        return time.monotonic() - self._creation_time
    
    def __str__(self) -> str:
        """Return a string representation of the event."""
        status = "triggered" if self.is_triggered else "pending"
        return f"Event(name='{self.name}', status='{status}', data={self.data})"
