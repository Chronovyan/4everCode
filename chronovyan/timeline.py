"""
The Timeline class is the core of Chronovyan, managing a sequence of events over time.
"""
import time
import heapq
from typing import List, Optional, Callable, Any
from dataclasses import dataclass, field
from datetime import datetime, timedelta

@dataclass(order=True)
class ScheduledEvent:
    """A container for events scheduled on a timeline."""
    time: float
    event: 'Event' = field(compare=False)
    
    def __call__(self):
        """Trigger the event."""
        self.event.trigger()

class Timeline:
    """
    A sequence of events that occur over time.
    
    The Timeline class allows you to schedule events to occur at specific times
    or after specific delays, and then run them in the correct order.
    """
    
    def __init__(self):
        """Initialize a new, empty timeline."""
        self._events: List[ScheduledEvent] = []
        self._running = False
        self._start_time: Optional[float] = None
        self._current_time: float = 0.0
    
    def add_event(self, event: 'Event', delay: float = 0.0) -> None:
        """
        Add an event to the timeline.
        
        Args:
            event: The event to add
            delay: Number of seconds to wait before the event occurs
        """
        scheduled_time = self._current_time + delay
        heapq.heappush(self._events, ScheduledEvent(scheduled_time, event))
    
    def run(self, max_time: Optional[float] = None) -> None:
        """
        Run the timeline, executing events in order.
        
        Args:
            max_time: Maximum time to run the timeline (in seconds).
                     If None, runs until all events are processed.
        """
        self._running = True
        self._start_time = time.monotonic()
        self._current_time = 0.0
        
        try:
            while self._events and self._running:
                # Get the current time since start
                current_time = time.monotonic() - self._start_time
                
                # Check if we've reached max_time
                if max_time is not None and current_time >= max_time:
                    break
                
                # Get the next event
                if not self._events:
                    break
                    
                next_event = self._events[0]
                
                # If next event is in the future beyond max_time, stop
                if max_time is not None and next_event.time > max_time:
                    break
                
                # If the next event is in the future, sleep until it's time
                if next_event.time > current_time:
                    time_to_wait = next_event.time - current_time
                    time.sleep(time_to_wait)
                    current_time = time.monotonic() - self._start_time
                
                # Process all events that are due
                while self._events and self._events[0].time <= current_time:
                    scheduled_event = heapq.heappop(self._events)
                    self._current_time = scheduled_event.time
                    scheduled_event()
                
        finally:
            self._running = False
            self._last_elapsed_time = time.monotonic() - self._start_time if hasattr(self, '_start_time') and self._start_time is not None else 0.0
            self._start_time = None
    
    @property
    def elapsed_time(self) -> float:
        """
        Get the time elapsed since the timeline was started.
        
        Returns:
            The time in seconds since the timeline was started, or 0 if not running.
        """
        if not hasattr(self, '_start_time') or self._start_time is None:
            return 0.0
        
        if self._running:
            return time.monotonic() - self._start_time
        else:
            # If not running, return the last recorded elapsed time
            return getattr(self, '_last_elapsed_time', 0.0)
    
    def clear(self) -> None:
        """
        Clear all events from the timeline.
        """
        self._events = []
    
    def get_next_event_time(self) -> Optional[float]:
        """
        Get the time of the next scheduled event.
        
        Returns:
            The time of the next event, or None if there are no events.
        """
        if not self._events:
            return None
        return self._events[0].time
    
    def pause(self) -> None:
        """Pause the timeline."""
        # Implementation for pausing the timeline
        pass
    
    def resume(self) -> None:
        """Resume a paused timeline."""
        # Implementation for resuming the timeline
        pass
    
    def stop(self) -> None:
        """Stop the timeline immediately."""
        self._running = False
    
    @property
    def is_running(self) -> bool:
        """Check if the timeline is currently running."""
        return self._running
    
    @property
    def current_time(self) -> float:
        """Get the current time in the timeline."""
        if self._running and self._start_time is not None:
            return time.monotonic() - self._start_time
        return self._current_time
    
    @property
    def event_count(self) -> int:
        """Get the number of events currently scheduled."""
        return len(self._events)
