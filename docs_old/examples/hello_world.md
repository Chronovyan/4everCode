# Hello World Example

This is a simple "Hello World" example to get you started with Chronovyan.

## The Code

```python
import chronovyan as cv

def say_hello(event):
    print(f"{event.name} at {event.timestamp:.2f} seconds")

def main():
    print("Starting Chronovyan Hello World example...")
    
    # Create a new timeline
    timeline = cv.Timeline()
    
    # Add some events
    timeline.add_event(cv.Event("Hello", on_trigger=say_hello))
    timeline.add_event(cv.Event("World", delay=1.0, on_trigger=say_hello))
    
    # Run the timeline
    print("Running timeline...")
    timeline.run()
    print("Timeline complete!")

if __name__ == "__main__":
    main()
```

## Explanation

1. **Import the library**: We start by importing the Chronovyan library.

2. **Create a callback function**: The `say_hello` function will be called when each event is triggered.

3. **Create a timeline**: The `Timeline` object will manage our events.

4. **Add events**: We add two events to the timeline:
   - The first event says "Hello" immediately
   - The second event says "World" after a 1-second delay

5. **Run the timeline**: The `run()` method starts the timeline and processes all events.

## Expected Output

When you run this code, you should see output similar to:

```
Starting Chronovyan Hello World example...
Running timeline...
Hello at 0.00 seconds
World at 1.00 seconds
Timeline complete!
```

## Next Steps

- Try modifying the delays between events
- Add more events to the timeline
- Experiment with different callback functions
- For more examples, see the [Temporal Examples](temporal_examples.md) page.
