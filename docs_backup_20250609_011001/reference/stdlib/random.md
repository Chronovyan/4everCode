---
title: Random Module
description: Random number generation and sampling
weight: 80
---

# Random Module

## Overview

The `std.random` module provides facilities for generating random numbers and making random selections. It includes various distributions and utilities for common randomization tasks.

## Basic Random Numbers

### `random() -> FLOAT`
Returns a random float in the range [0.0, 1.0).

```chronovyan
IMPORT std.random as rng;

// Generate a random float between 0.0 (inclusive) and 1.0 (exclusive)
DECLARE x = rng.random();
```

### `seed(seed: INT)`
Seeds the random number generator for reproducible results.

```chronovyan
// Seed with a fixed value for reproducibility
rng.seed(42);
```

## Random Integers

### `randint(low: INT, high: INT) -> INT`
Returns a random integer in the range [low, high].

```chronovyan
// Roll a six-sided die
DECLARE roll = rng.randint(1, 6);

// Random index for an array
DECLARE items = ["apple", "banana", "cherry"];
DECLARE random_item = items[rng.randint(0, items.length - 1)];
```

### `randrange(stop: INT) -> INT`
Returns a random integer in the range [0, stop).

### `randrange(start: INT, stop: INT, step: INT = 1) -> INT`
Returns a random integer from `range(start, stop, step)`.

## Random Sequence Operations

### `choice(seq: SEQUENCE<T>) -> T`
Returns a random element from a non-empty sequence.

```chronovyan
DECLARE colors = ["red", "green", "blue"];
PRINT(rng.choice(colors));  // "red", "green", or "blue"
```

### `sample(population: SEQUENCE<T>, k: INT) -> VECTOR<T>`
Returns a k-length list of unique elements chosen from the population sequence.

```chronovyan
DECLARE numbers = 1..50;
DECLARE lottery = rng.sample(numbers, 6);  // 6 unique numbers
```

### `shuffle(seq: MUTABLE_SEQUENCE<T>)`
Shuffles a sequence in-place.

```chronovyan
DECLARE cards = ["A", "2", "3", "4", "5"];
rng.shuffle(cards);
PRINT(cards);  // e.g., ["3", "A", "5", "2", "4"]
```

## Random Distributions

### Uniform Distribution

#### `uniform(a: FLOAT, b: FLOAT) -> FLOAT`
Returns a random float in the range [a, b).

### Normal (Gaussian) Distribution

#### `normal(mean: FLOAT, stddev: FLOAT) -> FLOAT`
Returns a random float from a normal distribution.

```chronovyan
// Generate values with mean 0 and standard deviation 1
DECLARE value = rng.normal(0.0, 1.0);
```

### Exponential Distribution

#### `expovariate(lambda: FLOAT) -> FLOAT`
Returns a random float from an exponential distribution.

### Other Distributions

- `triangular(low: FLOAT, high: FLOAT, mode: FLOAT) -> FLOAT`
- `lognormal(mu: FLOAT, sigma: FLOAT) -> FLOAT`
- `weibull(alpha: FLOAT, beta: FLOAT) -> FLOAT`
- `gamma(alpha: FLOAT, beta: FLOAT) -> FLOAT`
- `beta(alpha: FLOAT, beta: FLOAT) -> FLOAT`
- `pareto(alpha: FLOAT) -> FLOAT`

## Random Bytes

### `randbytes(n: INT) -> BYTES`
Returns n random bytes.

```chronovyan
// Generate a random 16-byte token
DECLARE token = rng.randbytes(16).to_hex();
```

## Cryptographically Secure Randomness

### `secure_random() -> FLOAT`
Returns a cryptographically secure random float in [0.0, 1.0).

### `secure_randint(low: INT, high: INT) -> INT`
Returns a cryptographically secure random integer in [low, high].

### `secure_randbytes(n: INT) -> BYTES`
Returns n cryptographically secure random bytes.

## Example: Password Generator

```chronovyan
IMPORT std.random as rng;
IMPORT std.string as str;

FUNC STRING generate_password(length: INT = 16) {
    DECLARE chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" +
                   "abcdefghijklmnopqrstuvwxyz" +
                   "0123456789" +
                   "!@#$%^&*()";
    
    DECLARE password = "";
    FOR i IN 0..length {
        password += str.char_at(chars, rng.randint(0, chars.length - 1));
    }
    
    RETURN password;
}

// Generate a 12-character password
PRINT(generate_password(12));
```

## Example: Weighted Random Selection

```chronovyan
IMPORT std.random as rng;

FUNC T weighted_choice<T>(items: VECTOR<T>, weights: VECTOR<FLOAT>) -> T {
    // Calculate cumulative weights
    DECLARE total = 0.0;
    DECLARE cumulative_weights = VECTOR<FLOAT>();
    
    FOR w IN weights {
        total += w;
        cumulative_weights.push(total);
    }
    
    // Select a random point in the total weight
    DECLARE r = rng.random() * total;
    
    // Find the corresponding item
    FOR i IN 0..cumulative_weights.length {
        IF (r < cumulative_weights[i]) {
            RETURN items[i];
        }
    }
    
    // Should never reach here if weights are positive
    RETURN items[0];
}

// Example usage
DECLARE loot = ["Common", "Uncommon", "Rare", "Epic", "Legendary"];
DECLARE weights = [50.0, 30.0, 12.0, 6.0, 2.0];

// Get a random loot item with the specified weights
PRINT(weighted_choice(loot, weights));
```

## Performance Considerations

- The default random number generator is fast but not suitable for cryptographic purposes
- For cryptographic operations, use the `secure_*` functions
- Creating many random number generators can be expensive; reuse them when possible
- For large sequences, consider using a generator expression with `sample` instead of creating large intermediate sequences

## Thread Safety

- The global random number generator is thread-local, so it's safe to use in concurrent contexts
- Each thread gets its own random state
- For parallel applications, consider using separate `Random` instances per thread

## Common Patterns

### Procedural Generation

```chronovyan
IMPORT std.random as rng;

// Generate a random dungeon layout
FUNC generate_dungeon(width: INT, height: INT) -> VEC<VEC<CHAR>> {
    DECLARE grid = VEC::with_capacity(height);
    
    // Initialize grid with walls
    FOR _ IN 0..height {
        grid.push(VEC::from_elem(width, '#'));
    }
    
    // Carve out rooms and corridors
    LET rooms = rng.gen_range(5..10);
    FOR _ IN 0..rooms {
        LET room_width = rng.gen_range(3..8);
        LET room_height = rng.gen_range(3..6);
        LET x = rng.gen_range(1..width - room_width - 1);
        LET y = rng.gen_range(1..height - room_height - 1);
        
        // Carve out the room
        FOR dy IN 0..room_height {
            FOR dx IN 0..room_width {
                grid[y + dy][x + dx] = '.';
            }
        }
    }
    
    // Add some random doors
    LET doors = rng.gen_range(3..7);
    FOR _ IN 0..doors {
        LET x = rng.gen_range(1..width-1);
        LET y = rng.gen_range(1..height-1);
        IF grid[y][x] == '#' {
            grid[y][x] = '+';
        }
    }
    
    RETURN grid;
}
```

### Monte Carlo Simulation

```chronovyan
IMPORT std.random as rng;
IMPORT std.math as math;

// Estimate π using Monte Carlo method
FUNC estimate_pi(samples: UINT) -> FLOAT {
    DECLARE inside = 0;
    
    FOR _ IN 0..samples {
        LET x = rng.gen::<FLOAT>() * 2.0 - 1.0;
        LET y = rng.gen::<FLOAT>() * 2.0 - 1.0;
        
        IF x*x + y*y <= 1.0 {
            inside += 1;
        }
    }
    
    RETURN 4.0 * (inside as FLOAT) / (samples as FLOAT);
}
```

## Performance Considerations

### Seeding Strategies

```chronovyan
// For reproducible results (e.g., in tests)
LET seed = 42;
LET mut rng = rng.StdRng::seed_from_u64(seed);

// For cryptographic security
LET mut csprng = rng.thread_rng();  // Automatically seeded from OS

// For parallel RNG (each thread gets independent, deterministic stream)
LET mut rng = rng.StdRng::from_entropy();
```

### Sampling Without Replacement

```chronovyan
// Efficiently sample k unique elements from a collection
FUNC sample_without_replacement<T>(items: &[T], k: UINT) -> VEC<&T> {
    IF k >= items.len() {
        RETURN items.iter().collect();
    }
    
    DECLARE result = VEC::with_capacity(k);
    DECLARE mut remaining = items.len();
    
    FOR item IN items {
        LET prob = (k - result.len()) as FLOAT / remaining as FLOAT;
        IF rng.gen_bool(prob) {
            result.push(item);
            IF result.len() == k {
                BREAK;
            }
        }
        remaining -= 1;
    }
    
    RETURN result;
}
```

## Related

- [Math](./math.md) - For mathematical functions and statistical distributions
- [Collections](./collections.md) - For shuffling and sampling from collections
- [Testing](./testing.md) - For property-based testing
- [Crypto](./crypto.md) - For cryptographically secure random number generation

## Best Practices

1. **Choose the right RNG** for your needs:
   - `thread_rng()` for general use
   - `StdRng` for reproducibility
   - `OsRng` for cryptographic operations

2. **Be mindful of bias** when mapping random numbers to ranges

3. **Reseed carefully** - prefer system entropy when possible

4. **Use appropriate distributions** for non-uniform random numbers

5. **Consider thread safety** in concurrent contexts

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For built-in random number generation
- [String](./string.md) - For string manipulation with random data
