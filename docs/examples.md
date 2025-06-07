---
title: Examples
description: Practical examples of using Chronovyan
---

# Examples

## Basic Usage

### Creating a Simple Timeline

```python
from chronovyan import Timeline, Event

def log_event(data):
    print(f"[{timeline.current_time:.2f}s] {data}")

# Create a new timeline
timeline = Timeline()

# Add some events
timeline.add_event(Event(1.0, log_event, "First event"))
timeline.add_event(Event(3.0, log_event, "Second event"))
timeline.add_event(Event(2.0, log_event, "Third event"))  # Will be reordered

# Run the timeline
timeline.run()

# Output:
# [1.00s] First event
# [2.00s] Third event
# [3.00s] Second event
```

### Event Dependencies

```python
from chronovyan import Timeline, Event

def setup_dependencies():
    timeline = Timeline()
    
    def create_printer(msg):
        def printer(_):
            print(f"[{timeline.current_time:.2f}s] {msg}")
        return printer
    
    # Create events with dependencies
    event_a = Event(1.0, create_printer("Event A"))
    event_b = Event(2.0, create_printer("Event B"))
    
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

```python
from chronovyan import Timeline, Event
import random

class Game:
    def __init__(self):
        self.timeline = Timeline()
        self.player_health = 100
        self.enemy_health = 100
        self.game_over = False
    
    def player_attack(self, _):
        if self.game_over:
            return
            
        damage = random.randint(5, 15)
        self.enemy_health -= damage
        print(f"Player attacks! Dealt {damage} damage. Enemy health: {max(0, self.enemy_health)}")
        
        if self.enemy_health <= 0:
            print("Player wins!")
            self.game_over = True
            self.timeline.stop()
        else:
            # Schedule enemy's turn
            self.timeline.add_event(Event(
                self.timeline.current_time + 1.0,
                self.enemy_attack
            ))
    
    def enemy_attack(self, _):
        if self.game_over:
            return
            
        damage = random.randint(3, 12)
        self.player_health -= damage
        print(f"Enemy attacks! Dealt {damage} damage. Your health: {max(0, self.player_health)}")
        
        if self.player_health <= 0:
            print("Game Over! You were defeated.")
            self.game_over = True
            self.timeline.stop()
        else:
            # Schedule player's turn
            print("\nYour turn! Press Enter to attack...")
            # In a real game, you'd wait for player input here
            # For this example, we'll automatically attack after a delay
            self.timeline.add_event(Event(
                self.timeline.current_time + 0.5,
                self.player_attack
            ))
    
    def start(self):
        print("Game started! It's your turn first.")
        self.timeline.add_event(Event(0.0, self.player_attack))
        self.timeline.run()

# Start the game
game = Game()
game.start()
```

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
