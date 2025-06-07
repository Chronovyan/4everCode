"""
Event synchronization example for Chronovyan.

This script demonstrates how to synchronize events and create dependencies between them.
"""
import time
from dataclasses import dataclass
from typing import List, Optional
from chronovyan import Timeline, Event

@dataclass
class Task:
    """A simple task with a name and duration."""
    name: str
    duration: float  # in seconds
    dependencies: List[str] = None
    
    def __post_init__(self):
        if self.dependencies is None:
            self.dependencies = []

def main():
    print("=== Chronovyan Event Synchronization Example ===\n")
    
    # Define some tasks with dependencies
    tasks = [
        Task("Task A", 1.0),
        Task("Task B", 2.0, ["Task A"]),  # Depends on Task A
        Task("Task C", 1.5, ["Task A"]),  # Depends on Task A
        Task("Task D", 1.0, ["Task B", "Task C"]),  # Depends on B and C
        Task("Task E", 0.5, ["Task D"]),  # Depends on D
    ]
    
    # Create a timeline and a dictionary to track task events
    timeline = Timeline()
    task_events = {}
    
    # Create events for each task
    for task in tasks:
        def create_task_handler(task):
            def on_start():
                print(f"🚀 [{timeline.current_time:.1f}s] Starting {task.name} (takes {task.duration}s)")
                
                # Schedule the completion of this task
                def on_complete():
                    print(f"✅ [{timeline.current_time:.1f}s] Completed {task.name}")
                    
                    # Trigger dependent tasks if all their dependencies are complete
                    for dependent in tasks:
                        if task.name in dependent.dependencies:
                            all_deps_complete = all(
                                dep in task_events and task_events[dep].is_triggered
                                for dep in dependent.dependencies
                            )
                            
                            if all_deps_complete and dependent.name not in task_events:
                                task_events[dependent.name] = Event(
                                    f"Start {dependent.name}",
                                    on_trigger=lambda e, d=dependent: on_task_start(d)
                                )
                                timeline.add_event(task_events[dependent.name])
                
                # Schedule the completion
                completion_event = Event(
                    f"Complete {task.name}",
                    on_trigger=lambda e, t=task: on_complete()
                )
                timeline.add_event(completion_event, delay=task.duration)
                
            return on_start
        
        # Create the start event for this task
        on_task_start = create_task_handler(task)
        
        if not task.dependencies:
            # No dependencies, can start immediately
            event = Event(
                f"Start {task.name}",
                on_trigger=lambda e, t=task: on_task_start()
            )
            task_events[task.name] = event
            timeline.add_event(event)
    
    # Print the task dependencies
    print("Task Dependencies:")
    for task in tasks:
        deps = ", ".join(task.dependencies) if task.dependencies else "None"
        print(f"- {task.name} (takes {task.duration}s) | Depends on: {deps}")
    
    # Run the timeline
    print("\n=== Task Execution ===")
    start_time = time.monotonic()
    timeline.run()
    elapsed = time.monotonic() - start_time
    
    print(f"\nAll tasks completed in {elapsed:.2f} seconds")
    print("\nExample completed!")

if __name__ == "__main__":
    main()
