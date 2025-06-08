---
title: Math Module
description: Mathematical functions and constants
weight: 50
---

# Math Module

## Overview

The `std.math` module provides mathematical functions and constants for Chronovyan, including basic operations, trigonometry, statistics, and more.

## Constants

```chronovyan
IMPORT std.math as math;

PRINT(math.PI);      // 3.141592653589793
PRINT(math.E);       // 2.718281828459045
PRINT(math.TAU);     // 6.283185307179586
PRINT(math.SQRT_2);  // 1.4142135623730951
PRINT(math.LN_2);    // 0.6931471805599453
```

## Basic Operations

### `abs(x: NUMBER) -> NUMBER`
Returns the absolute value of a number.

### `min(a: NUMBER, b: NUMBER) -> NUMBER`
Returns the smaller of two numbers.

### `max(a: NUMBER, b: NUMBER) -> NUMBER`
Returns the larger of two numbers.

### `clamp(x: NUMBER, min: NUMBER, max: NUMBER) -> NUMBER`
Restricts a value to be within a range.

### `sign(x: NUMBER) -> INT`
Returns -1, 0, or 1 indicating the sign of the number.

## Rounding

```chronovyan
math.floor(3.7)   // 3.0
math.ceil(3.2)    // 4.0
math.round(3.5)   // 4.0
math.trunc(3.7)   // 3.0
```

## Exponents and Logarithms

### `pow(base: NUMBER, exp: NUMBER) -> NUMBER`
Raises a number to a power.

### `sqrt(x: NUMBER) -> NUMBER`
Square root function.

### `exp(x: NUMBER) -> NUMBER`
Returns e raised to the power of x.

### `log(x: NUMBER, base: NUMBER) -> NUMBER`
Logarithm with specified base.

### `log10(x: NUMBER) -> NUMBER`
Base-10 logarithm.

### `log2(x: NUMBER) -> NUMBER`
Base-2 logarithm.

## Trigonometry

### Basic Functions
```chronovyan
math.sin(math.PI/2)  // 1.0
math.cos(0)          // 1.0
math.tan(math.PI/4)  // 1.0
```

### Inverse Functions
```chronovyan
math.asin(1.0)  // π/2
math.acos(1.0)  // 0.0
math.atan(1.0)  // π/4
```

### Hyperbolic Functions
```chronovyan
math.sinh(1.0)
math.cosh(1.0)
math.tanh(1.0)
```

## Random Numbers

### `random() -> FLOAT`
Returns a random float in the range [0.0, 1.0).

### `randint(low: INT, high: INT) -> INT`
Returns a random integer in the range [low, high].

### `choice(seq: SEQUENCE<T>) -> T`
Returns a random element from a sequence.

### `shuffle(seq: MUTABLE_SEQUENCE<T>)`
Shuffles a sequence in-place.


## Statistics

### `mean(iter: ITERABLE<NUMBER>) -> FLOAT`
Calculates the arithmetic mean.

### `median(iter: ITERABLE<NUMBER>) -> FLOAT`
Calculates the median.

### `mode(iter: ITERABLE<T>) -> T`
Returns the most common value.

### `stdev(iter: ITERABLE<NUMBER>) -> FLOAT`
Calculates the sample standard deviation.

### `variance(iter: ITERABLE<NUMBER>) -> FLOAT`
Calculates the sample variance.

## Linear Algebra

### `Vector` Type
```chronovyan
DECLARE v1 = math.Vector([1.0, 2.0, 3.0]);
DECLARE v2 = math.Vector([4.0, 5.0, 6.0]);

// Vector operations
PRINT(v1 + v2);      // [5.0, 7.0, 9.0]
PRINT(v1.dot(v2));   // 32.0
PRINT(v1.cross(v2)); // [-3.0, 6.0, -3.0]
PRINT(v1.norm());    // 3.7416573867739413
```

### `Matrix` Type
```chronovyan
DECLARE m1 = math.Matrix([
    [1.0, 2.0],
    [3.0, 4.0]
]);

// Matrix operations
PRINT(m1.det());     // -2.0
PRINT(m1.transpose());
// [[1.0, 3.0],
//  [2.0, 4.0]]
```

## Complex Numbers

### `Complex` Type
```chronovyan
DECLARE c1 = math.Complex(1.0, 2.0);  // 1 + 2i
DECLARE c2 = math.Complex(3.0, 4.0);  // 3 + 4i

// Complex arithmetic
PRINT(c1 + c2);  // (4+6i)
PRINT(c1 * c2);  // (-5+10i)
PRINT(c1.abs()); // 2.23606797749979
```

## Example: Quadratic Equation Solver

```chronovyan
IMPORT std.math as math;

FUNC (FLOAT, FLOAT, BOOL) solve_quadratic(a: FLOAT, b: FLOAT, c: FLOAT) {
    DECLARE discr = b*b - 4*a*c;
    
    IF (discr < 0) {
        // No real roots
        RETURN (0.0, 0.0, FALSE);
    }
    
    DECLARE sqrt_discr = math.sqrt(discr);
    DECLARE x1 = (-b - sqrt_discr) / (2*a);
    DECLARE x2 = (-b + sqrt_discr) / (2*a);
    
    RETURN (x1, x2, TRUE);
}

// Example usage
LET (root1, root2, has_roots) = solve_quadratic(1.0, -3.0, 2.0);
IF (has_roots) {
    PRINT("Roots:", root1, root2);  // 1.0, 2.0
}
```

## Performance Considerations

### Statistical Analysis

```chronovyan
IMPORT std.math as math;

STRUCT Statistics {
    mean: FLOAT,
    median: FLOAT,
    std_dev: FLOAT,
    min: FLOAT,
    max: FLOAT,
}

FUNC Statistics analyze_data(data: VECTOR<FLOAT>) -> Statistics {
    IF (data.is_empty()) {
        THROW ValueError("Cannot analyze empty dataset");
    }
    
    DECLARE sorted_data = data.sorted();
    DECLARE n = sorted_data.len() as FLOAT;
    
    // Calculate mean
    DECLARE sum = 0.0;
    FOR x IN &sorted_data {
        sum += x;
    }
    DECLARE mean = sum / n;
    
    // Calculate standard deviation
    DECLARE variance = 0.0;
    FOR x IN &sorted_data {
        variance += (x - mean).powi(2);
    }
    variance /= n;
    DECLARE std_dev = math.sqrt(variance);
    
    // Calculate median
    DECLARE median = IF (n as INT % 2 == 1) {
        sorted_data[n as INT / 2]
    } ELSE {
        (sorted_data[(n/2.0 - 1.0) as INT] + sorted_data[(n/2.0) as INT]) / 2.0
    };
    
    RETURN Statistics {
        mean: mean,
        median: median,
        std_dev: std_dev,
        min: sorted_data[0],
        max: sorted_data[sorted_data.len() - 1]
    };
}
```

### Linear Regression

```chronovyan
IMPORT std.math as math;

STRUCT Point {
    x: FLOAT,
    y: FLOAT
}

STRUCT Line {
    slope: FLOAT,
    intercept: FLOAT,
    
    FUNC predict(&self, x: FLOAT) -> FLOAT {
        RETURN self.slope * x + self.intercept;
    }
}

FUNC Line linear_regression(points: VECTOR<Point>) -> Line {
    DECLARE n = points.len() as FLOAT;
    DECLARE sum_x = 0.0;
    DECLARE sum_y = 0.0;
    DECLARE sum_xy = 0.0;
    DECLARE sum_xx = 0.0;
    
    FOR point IN &points {
        sum_x += point.x;
        sum_y += point.y;
        sum_xy += point.x * point.y;
        sum_xx += point.x * point.x;
    }
    
    DECLARE slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
    DECLARE intercept = (sum_y - slope * sum_x) / n;
    
    RETURN Line { slope, intercept };
}
```

## Performance Optimization

### Vectorized Operations

```chronovyan
// Slow: Element-wise operations in a loop
FUNC VECTOR<FLOAT> scale_vector(s: FLOAT, v: VECTOR<FLOAT>) -> VECTOR<FLOAT> {
    DECLARE result = VECTOR::with_capacity(v.len());
    FOR x IN &v {
        result.push(s * x);
    }
    RETURN result;
}

// Fast: Use vectorized operations
FUNC VECTOR<FLOAT> scale_vector_fast(s: FLOAT, v: VECTOR<FLOAT>) -> VECTOR<FLOAT> {
    RETURN v * s;  // Uses SIMD instructions when available
}
```

### Pre-computation

```chronovyan
// Pre-compute values for better performance
STRUCT TrigTable {
    sin_values: VECTOR<FLOAT>,
    cos_values: VECTOR<FLOAT>,
    step: FLOAT,
    
    FUNC NEW(steps: INT) -> Self {
        DECLARE sin_vals = VECTOR::with_capacity(steps);
        DECLARE cos_vals = VECTOR::with_capacity(steps);
        DECLARE step = 2.0 * math.PI / steps as FLOAT;
        
        FOR i IN 0..steps {
            LET angle = i as FLOAT * step;
            sin_vals.push(math.sin(angle));
            cos_vals.push(math.cos(angle));
        }
        
        RETURN Self {
            sin_values: sin_vals,
            cos_values: cos_vals,
            step: step
        };
    }
    
    FUNC sin(&self, x: FLOAT) -> FLOAT {
        LET idx = ((x / self.step) as INT) % self.sin_values.len();
        RETURN self.sin_values[idx];
    }
    
    FUNC cos(&self, x: FLOAT) -> FLOAT {
        LET idx = ((x / self.step) as INT) % self.cos_values.len();
        RETURN self.cos_values[idx];
    }
}
```

## Related

- [Random](./random.md) - For generating random numbers and statistical distributions
- [Collections](./collections.md) - For working with numerical data structures
- [Testing](./testing.md) - For testing numerical algorithms
- [Time](./time.md) - For time-based calculations

## Best Practices

1. **Use appropriate precision** - Choose between `FLOAT` and `DOUBLE` based on your needs
2. **Beware of floating-point errors** - Use approximate equality for comparisons
3. **Use built-in functions** - They're optimized and tested
4. **Consider numerical stability** - Avoid subtracting nearly equal numbers
5. **Profile performance-critical code** - Optimize hotspots

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For built-in mathematical operators-related mathematical operations
- [Random](./random.md) - For advanced random number generation
