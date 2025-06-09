# Your First Forever Code Program

In this guide, we'll create a simple Forever Code program to help you understand the basics of the language and its features.

## Prerequisites

- Forever Code installed (see [Installation](./installation.md))
- A text editor or IDE of your choice

## Creating a New Project

1. Create a new directory for your project:

   ```bash
   mkdir my-forever-app
   cd my-forever-app
   ```

2. Initialize a new Forever Code project:

   ```bash
   forever init
   ```

   This will create a basic project structure with the following files:
   ```
   my-forever-app/
   ├── .forever/           # Forever Code internal directory
   ├── main.fc            # Main program file
   └── forever.toml       # Project configuration
   ```

## Writing Your First Program

Open `main.fc` in your text editor and add the following code:

```python
# main.fc
from datetime import datetime
from forever import Entity, field

# Define a simple entity
class User(Entity):
    name: str = field(indexed=True)
    email: str = field(unique=True)
    created_at: datetime = field(default_factory=datetime.utcnow)
    is_active: bool = True

# Create a new user
def create_user(name: str, email: str) -> User:
    """Create and return a new user."""
    user = User(name=name, email=email)
    user_id = user.save()
    print(f"Created user with ID: {user_id}")
    return user

# Query users
def find_user_by_email(email: str) -> User | None:
    """Find a user by their email address."""
    return User.query().filter(User.email == email).first()

# Main program
if __name__ == "__main__":
    # Create some users
    alice = create_user("Alice", "alice@example.com")
    bob = create_user("Bob", "bob@example.com")

    # Query a user
    user = find_user_by_email("alice@example.com")
    if user:
        print(f"Found user: {user.name} ({user.email})")
    else:
        print("User not found")
```

## Running Your Program

To run your Forever Code program:

```bash
forever run main.fc
```

You should see output similar to:

```
Created user with ID: user_abc123
Created user with ID: user_def456
Found user: Alice (alice@example.com)
```

## Understanding the Code

1. **Entity Definition**: We defined a `User` entity with fields for name, email, creation time, and active status.
2. **Persistence**: The `save()` method persists the entity to the database.
3. **Querying**: We can query entities using a fluent API.
4. **Type Safety**: The code is fully typed using Python's type hints.

## Next Steps

- Learn about [Core Concepts](../concepts/) in Forever Code
- Explore the [API Reference](../reference/)
- Check out [Example Projects](../guides/examples/)
