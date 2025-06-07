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
        if self._running:
            raise RuntimeError("Timeline is already running")
            
        self._running = True
        self._start_time = time.monotonic()
        
        try:
            while self._events and (max_time is None or self._current_time < max_time):
                now = time.monotonic() - self._start_time
                
                # Process all events that should have happened by now
                while self._events and self._events[0].time <= now:
                    scheduled_event = heapq.heappop(self._events)
                    self._current_time = max(self._current_time, scheduled_event.time)
                    scheduled_event()
                
                # If we have more events to process but haven't reached max_time,
                # wait until the next event is due
                if self._events and (max_time is None or self._events[0].time < max_time):
                    time_to_wait = self._events[0].time - now
                    if time_to_wait > 0:
                        time.sleep(time_to_wait)
                else:
                    break
                    
                # Update current time after waiting
                now = time.monotonic() - self._start_time
                self._current_time = now
                
        finally:
            self._running = False
    
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
