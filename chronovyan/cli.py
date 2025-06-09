""
Command-line interface for Forever.
"""
import argparse
import sys
import json
from typing import List, Optional

from . import __version__, Timeline, Event

def create_timeline():
    """Create and return a new timeline."""
    return Timeline()

def add_event(timeline: Timeline, name: str, delay: float = 0.0, data: Optional[dict] = None):
    """Add an event to the timeline."""
    def on_trigger(event):
        print(f"\nEvent triggered: {event.name}")
        if event.data:
            print(f"Event data: {json.dumps(event.data, indent=2)}")
    
    event = Event(name=name, data=data, on_trigger=on_trigger, delay=delay)
    timeline.add_event(event, delay=delay)
    return event

def run_timeline(timeline: Timeline, duration: Optional[float] = None):
    """Run the timeline for the specified duration or until all events are processed."""
    print(f"\nStarting timeline with {timeline.event_count} events...")
    try:
        timeline.run(max_time=duration)
    except KeyboardInterrupt:
        print("\nTimeline stopped by user")
    finally:
        print(f"Timeline completed. Current time: {timeline.current_time:.2f}s")

def parse_args(args: List[str]) -> argparse.Namespace:
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description="Forever - A temporal programming language")
    
    # Global arguments
    parser.add_argument(
        "-v", "--version", 
        action="version",
        version=f"Forever {__version__}",
        help="Show version and exit"
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest="command", help="Command to run")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run a timeline with events")
    run_parser.add_argument(
        "-d", "--duration", 
        type=float, 
        help="Maximum duration to run the timeline (in seconds)"
    )
    
    # Add event command
    event_parser = subparsers.add_parser("add", help="Add an event to the timeline")
    event_parser.add_argument("name", help="Name of the event")
    event_parser.add_argument(
        "--delay", 
        type=float, 
        default=0.0, 
        help="Delay before the event occurs (in seconds)"
    )
    event_parser.add_argument(
        "--data", 
        type=json.loads, 
        default={}, 
        help="JSON data to associate with the event"
    )
    
    # List events command
    list_parser = subparsers.add_parser("list", help="List events in the timeline")
    
    return parser.parse_args(args)

def main(args: Optional[List[str]] = None) -> int:
    """Main entry point for the CLI."""
    if args is None:
        args = sys.argv[1:]
    
    # If no arguments, show help
    if not args:
        args = ["--help"]
    
    parsed_args = parse_args(args)
    
    # Create a default timeline
    timeline = create_timeline()
    
    # Handle commands
    if parsed_args.command == "run":
        run_timeline(timeline, parsed_args.duration)
    elif parsed_args.command == "add":
        add_event(
            timeline, 
            parsed_args.name, 
            delay=parsed_args.delay, 
            data=parsed_args.data
        )
        print(f"Added event: {parsed_args.name} (delay: {parsed_args.delay}s)")
    elif parsed_args.command == "list":
        print(f"Timeline has {timeline.event_count} events")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
