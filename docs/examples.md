---
title: Examples
description: Practical examples of using Chronovyan
---

# Examples

## Basic Usage

### Creating a Simple Timeline

```python
from chronovyan import Timeline, Event

# Create a callback function that will be called when events are triggered
def log_event(event):
    print(f"[{timeline.current_time:.2f}s] {event.name}: {event.data}")

# Create a new timeline
timeline = Timeline()

# Create events with names and data
event1 = Event("first", "Hello, Chronovyan!", on_trigger=log_event)
event2 = Event("second", "This is a test event", on_trigger=log_event)

# Add events to the timeline with delays (in seconds)
timeline.add_event(event1, delay=1.0)  # Trigger after 1 second
timeline.add_event(event2, delay=3.0)  # Trigger after 3 seconds

# Run the timeline
timeline.run()

# Output:
# [1.00s] first: Hello, Chronovyan!
# [3.00s] second: This is a test event
```

### Using Event Data and Callbacks

```python
from chronovyan import Timeline, Event

# Create a timeline
timeline = Timeline()

# Define a more complex callback that uses event data
def process_event(event):
    print(f"[{timeline.current_time:.2f}s] Processing {event.name}")
    if event.data:
        print(f"  Data: {event.data}")
    if event.is_triggered:
        print(f"  This event was triggered at {event.timestamp}")

# Create events with different data and callbacks
start_event = Event("start", {"message": "Starting the process"}, on_trigger=process_event)
progress_event = Event("progress", {"percent": 50}, on_trigger=process_event)
complete_event = Event("complete", {"status": "success"}, on_trigger=process_event)

# Schedule the events
timeline.add_event(start_event, delay=0.5)      # After 0.5 seconds
timeline.add_event(progress_event, delay=2.0)   # After 2 seconds
# This event will be triggered immediately when the timeline starts
immediate_event = Event("immediate", "This runs right away", on_trigger=process_event)
timeline.add_event(immediate_event)

# Run the timeline for 3 seconds
timeline.run(max_time=3.0)

# Output:
# [0.00s] Processing immediate
#   Data: This runs right away
# [0.50s] Processing start
#   Data: {'message': 'Starting the process'}
# [2.00s] Processing progress
#   Data: {'percent': 50}
```
    # Event C depends on A and B
    def event_c_callback(_):
        print(f"[{timeline.current_time:.2f}s] Event C (depends on A & B)")
    
    event_c = Event(3.0, event_c_callback)
    
    timeline.add_event(event_a)
    timeline.add_event(event_b)
    timeline.add_event(event_c)
    
    return timeline

# Run the timeline with dependencies
timeline = setup_dependencies()
timeline.run()
```

## Advanced Usage

### Event Chaining

```python
from chronovyan import Timeline, Event

# Create a timeline
timeline = Timeline()

def create_chained_events():
    """Create a sequence of events where each triggers the next."""
    
    def create_callback(name, next_event=None):
        def callback(event):
            print(f"[{timeline.current_time:.2f}s] {event.name}: {event.data}")
            if next_event and not next_event.is_triggered:
                next_event.trigger()
        return callback
    
    # Create events in reverse order
    event_c = Event("event_c", "Third in sequence", on_trigger=create_callback("Third"))
    event_b = Event("event_b", "Second in sequence", on_trigger=create_callback("Second", event_c))
    event_a = Event("event_a", "First in sequence", on_trigger=create_callback("First", event_b))
    
    return event_a

# Get the first event in the chain
first_event = create_chained_events()

# Schedule the first event to run after 1 second
timeline.add_event(first_event, delay=1.0)

# Run the timeline
timeline.run()

# Output:
# [1.00s] First: First in sequence
# [1.00s] Second: Second in sequence
# [1.00s] Third: Third in sequence
```

### Error Handling in Events

```python
from chronovyan import Timeline, Event

# Create a timeline
timeline = Timeline()

def error_handler(event, exception):
    print(f"Error in {event.name}: {exception}")

def risky_operation(event):
    if "error" in str(event.data).lower():
        raise ValueError(f"Error processing {event.data}")
    print(f"Successfully processed: {event.data}")

# Create events with potential errors
event1 = Event("safe_event", "This will work", on_trigger=risky_operation)
event2 = Event("error_event", "This will cause an error", on_trigger=risky_operation)
event2.on_error = error_handler  # Attach error handler

# Schedule the events
timeline.add_event(event1, delay=0.5)
timeline.add_event(event2, delay=1.5)

# Run the timeline
try:
    timeline.run()
except Exception as e:
    print(f"Timeline stopped due to: {e}")

# Output:
# [0.50s] Successfully processed: This will work
# [1.50s] Error in error_event: Error processing This will cause an error
```

### Using max_time to Limit Execution

```python
from chronovyan import Timeline, Event

# Create a timeline
timeline = Timeline()

def periodic_event(event):
    print(f"[{timeline.current_time:.2f}s] Periodic event")
    # Reschedule this event
    timeline.add_event(Event("periodic", on_trigger=periodic_event), delay=1.0)

# Start periodic events
timeline.add_event(Event("start", on_trigger=periodic_event))

# Run for 3.5 seconds
print("Running for 3.5 seconds...")
timeline.run(max_time=3.5)

# Output:
# [0.00s] Periodic event
# [1.00s] Periodic event
# [2.00s] Periodic event
# [3.00s] Periodic event
# Timeline stopped at 3.50s
```

### Periodic Events

```python
from chronovyan import Timeline, Event

def create_periodic_printer(timeline, interval, count, prefix):
    def callback(_):
        nonlocal count
        if count > 0:
            print(f"[{timeline.current_time:.2f}s] {prefix} {count}")
            count -= 1
            # Schedule the next occurrence
            timeline.add_event(Event(timeline.current_time + interval, callback, None))
    
    return callback

# Create a timeline with periodic events
timeline = Timeline()
timeline.add_event(Event(0.0, create_periodic_printer(timeline, 1.0, 5, "Tick:")))
timeline.run()
```

### Timeout Pattern

```python
from chronovyan import Timeline, Event

def setup_timeout():
    timeline = Timeline()
    
    def on_timeout():
        print(f"[{timeline.current_time:.2f}s] Operation timed out!")
        timeline.stop()
    
    def long_running_operation():
        print(f"[{timeline.current_time:.2f}s] Starting long operation...")
        # Simulate work by scheduling completion after 3 seconds
        timeline.add_event(Event(timeline.current_time + 3.0, lambda _: print("Operation completed!")))
    
    # Set a timeout for 2 seconds
    timeline.add_event(Event(2.0, lambda _: on_timeout()))
    
    # Start the operation
    timeline.add_event(Event(0.1, lambda _: long_running_operation()))
    
    return timeline

# This will time out after 2 seconds
setup_timeout().run()
```

## Real-world Examples

### Network Request Simulation

```python
from chronovyan import Timeline, Event
import random

def simulate_network_request(timeline, url, callback):
    """Simulate a network request with random latency."""
    latency = random.uniform(0.1, 2.0)  # Random latency between 0.1 and 2.0 seconds
    
    def process_response():
        # Simulate success or failure
        success = random.random() > 0.3  # 70% success rate
        if success:
            response = f"Response from {url} (took {latency:.2f}s)"
        else:
            response = f"Error fetching {url} (took {latency:.2f}s)"
        callback(response, success)
    
    # Schedule the response
    timeline.add_event(Event(timeline.current_time + latency, 
                           lambda _: process_response()))

def run_requests():
    timeline = Timeline()
    urls = ["https://api.example.com/data/1",
            "https://api.example.com/data/2",
            "https://api.example.com/data/3"]
    
    def handle_response(response, success):
        status = "✓" if success else "✗"
        print(f"[{timeline.current_time:.2f}s] {status} {response}")
    
    # Start all requests at once
    for url in urls:
        simulate_network_request(timeline, url, handle_response)
    
    return timeline

# Run the simulation
run_requests().run()
```

### Game Loop Simulation

Here's a complete turn-based game example using Chronovyan's event system:

```python
from chronovyan import Timeline, Event
import random

class SimpleGame:
    def __init__(self):
        self.timeline = Timeline()
        self.player_health = 100
        self.enemy_health = 100
        self.game_over = False
    
    def log_status(self):
        """Print the current game status."""
        print(f"\n--- Status ---")
        print(f"Player Health: {self.player_health}")
        print(f"Enemy Health: {self.enemy_health}")
    
    def player_turn(self, event):
        """Handle the player's turn."""
        if self.game_over:
            return
            
        self.log_status()
        print("\n--- Player's Turn ---")
        
        # Player's attack
        damage = random.randint(5, 15)
        self.enemy_health -= damage
        print(f"You hit the enemy for {damage} damage!")
        
        # Check for victory
        if self.enemy_health <= 0:
            self.enemy_health = 0
            self.log_status()
            print("\n🎉 You defeated the enemy!")
            self.game_over = True
            self.timeline.stop()
            return
        
        # Schedule enemy's turn after a delay
        self.timeline.add_event(
            Event("enemy_turn", on_trigger=self.enemy_turn),
            delay=1.5
        )
    
    def enemy_turn(self, event):
        """Handle the enemy's turn."""
        if self.game_over:
            return
            
        self.log_status()
        print("\n--- Enemy's Turn ---")
        
        # Enemy's attack
        damage = random.randint(3, 12)
        self.player_health -= damage
        print(f"The enemy hits you for {damage} damage!")
        
        # Check for defeat
        if self.player_health <= 0:
            self.player_health = 0
            self.log_status()
            print("\n💀 You were defeated!")
            self.game_over = True
            self.timeline.stop()
            return
        
        # Schedule player's next turn after a delay
        self.timeline.add_event(
            Event("player_turn", on_trigger=self.player_turn),
            delay=1.5
        )
    
    def start(self):
        """Start the game."""
        print("=== Simple Combat Game ===")
        print("Defeat the enemy before they defeat you!")
        
        # Start with player's turn
        self.timeline.add_event(
            Event("start_game", on_trigger=self.player_turn),
            delay=1.0
        )
        
        # Run the game
        try:
            self.timeline.run()
        except KeyboardInterrupt:
            print("\nGame stopped by user.")
        finally:
            print("\nThanks for playing!")

# Start the game
if __name__ == "__main__":
    game = SimpleGame()
    game.start()
```

This example demonstrates a complete turn-based combat game where:
1. The player and enemy take turns attacking each other
2. Each turn is scheduled with a delay for better readability
3. The game ends when either the player or enemy's health reaches zero
4. Status is displayed after each turn
5. The game can be safely interrupted with Ctrl+C

## Performance Considerations

### Efficient Event Creation

When creating many events, consider these optimizations:

```python
# Less efficient: Creating many small callbacks
def create_events_slow(timeline, count):
    for i in range(count):
        timeline.add_event(Event(
            i * 0.1,
            lambda _, i=i: print(f"Event {i}")  # Note the i=i to capture the loop variable
        ))

# More efficient: Batch similar events
def create_events_fast(timeline, count):
    def batch_handler(event_data):
        for data in event_data:
            print(f"Event {data}")
    
    # Group events by time and process in batches
    event_data = [(i * 0.1, i) for i in range(count)]
    timeline.add_event(Event(0.0, lambda _: batch_handler([d[1] for d in event_data])))
```

## Integration Examples

### Using with asyncio

```python
import asyncio
from chronovyan import Timeline, Event

async def run_timeline_with_asyncio(timeline):
    loop = asyncio.get_event_loop()
    
    def run_timeline():
        timeline.run()
    
    # Run the timeline in a thread pool
    await loop.run_in_executor(None, run_timeline)

# Example usage
async def main():
    timeline = Timeline()
    timeline.add_event(Event(1.0, lambda _: print("Async event!")))
    
    # Run other asyncio tasks concurrently
    await asyncio.gather(
        run_timeline_with_asyncio(timeline),
        asyncio.sleep(2.0)
    )

# Run the asyncio event loop
asyncio.run(main())
```
