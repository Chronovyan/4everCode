"""
Basic usage example for Chronovyan.

This script demonstrates how to create a timeline, add events, and run them.
"""
import time
from chronovyan import Timeline, Event

def main():
    print("=== Chronovyan Basic Example ===")
    
    # Create a new timeline
    timeline = Timeline()
    
    # Define a callback function for events
    def on_event(event):
        print(f"\nEvent triggered: {event.name}")
        print(f"  Timestamp: {event.timestamp:.2f}")
        if event.data:
            print(f"  Data: {event.data}")
    
    # Add some events with different delays
    print("\nAdding events to the timeline...")
    
    # Immediate event
    timeline.add_event(Event("First Event", on_trigger=on_event))
    
    # Event with a delay
    timeline.add_event(Event("Delayed Event", on_trigger=on_event, data={"key": "value"}), delay=1.5)
    
    # Multiple events with different delays
    for i in range(3):
        timeline.add_event(
            Event(f"Count {i+1}", on_trigger=on_event, data={"count": i+1}),
            delay=0.5 * (i + 1)
        )
    
    # Run the timeline
    print(f"\nRunning timeline with {timeline.event_count} events...")
    print("(Press Ctrl+C to stop early)")
    
    try:
        start_time = time.monotonic()
        timeline.run()
        elapsed = time.monotonic() - start_time
        print(f"\nTimeline completed in {elapsed:.2f} seconds")
    except KeyboardInterrupt:
        print("\nTimeline stopped by user")
    
    print("\nExample completed!")

if __name__ == "__main__":
    main()
