---
title: The Weaver's Guide to 4ever Values
description: Documentation for ValueSystemLore.md
weight: 100
draft: true
---

# The Weaver's Guide to 4ever Values

*"In the beginning, there was the Void... and then there was `nil`."*

## The Primordial Threads

### The Void (`nil`)
- **Role**: The canvas upon which all is woven
- **Lore**: The space between threads, the silence between notes
- **In Game**: What you get when you try to access a thread that's already been unwoven
- **Pro Tip**: The Void always returns `false`, but so does `false` itself. Coincidence?

### The Twin Flames (`true` and `false`)
- **Role**: The fundamental choice in all of existence
- **Lore**: Legend says they were once a single flame until the First Conditional split them
- **In Game**: Used in all decision-making `IF` statements
- **Pro Tip**: In Chronoscript, `true` is just `false` that hasn't been proven wrong yet

### The Numeric Continuum (`number`)
- **Role**: The raw Aethel that powers all calculations
- **Lore**: Some say the number 42 appears in every major timeline convergence
- **In Game**: Used for everything from counting loot to calculating paradox levels
- **Pro Tip**: `3.14159...` is known as the "Weaver's Constant" - it appears in all stable patterns

### The Wordweaver's Chant (`string`)
- **Role**: The literal fabric of reality
- **Lore**: The first string was `@Origin`, and all others are echoes of its power
- **In Game**: Used for all text, from simple messages to complex incantations
- **Pro Tip**: A string is just an array of characters, but don't tell the arrays that

## The Living Patterns

### The Gathering (`array`)
- **Role**: Ordered collections of threads
- **Lore**: The first array was the "Anchors' Chain" that prevented the Second Fracture
- **In Game**: Perfect for inventories, spell components, or timelines
- **Pro Tip**: `array[0]` is how you access the first element, because Weavers start counting from the First Thread

### The Keeper's Tome (`object`)
- **Role**: Named collections of values
- **Lore**: The most sacred is the `4ever_Tapestry`, said to contain the master pattern of reality
- **In Game**: Used for character sheets, item definitions, and world state
- **Pro Tip**: Dot notation (`tome.property`) is just syntactic sugar for bracket notation (`tome["property"]`), but it's the sugar that makes the medicine go down

## The Celestial Weavers

### The Timekeeper (`Clock`)
- **Role**: Guardian of moments
- **Lore**: The only being said to have witnessed the `@Origin`
- **In Game**: Used for timing events, cooldowns, and temporal mechanics
- **Pro Tip**: `timekeeper.sleep(1.0)` is known as "The Weaver's Nap" - use it wisely

### The Shapeless One (`function`)
- **Role**: The great transformer
- **Lore**: Can take on any form but is bound by the rules of the `RUNTIME`
- **In Game**: The building blocks of all complex behavior
- **Pro Tip**: A function that doesn't return anything actually returns `nil`, which is just the function returning to the Void

## Famous Bugs and Legendary Values

### The Phantom Reference
- **Symptoms**: Values that disappear when you look at them
- **Cause**: Garbage collection by the Threadbare Monks
- **Solution**: Hold onto your references, or the Void will take them

### The Infinite Loop of Madness
- **Symptoms**: Your script runs forever, reality begins to unravel
- **Cause`: Forgetting the base case in a recursive function
- **Solution**: Always know when to stop, or the Timekeeper will stop you

### The Type Mismatch Catastrophe
- **Symptoms**: `"42" + 1` equals `"421"`
- **Cause**: The Wordweaver's Chant is stronger than the Numeric Continuum
- **Solution**: Know your types, or cast them with the proper incantations

## Exercises for Aspiring Weavers

1. **The Greeting Ritual**
   ```chronoscript
   // Create a function that greets a Weaver by name
   fn greet(weaverName) {
       // Your code here
   }
   ```

2. **The Inventory Conundrum**
   ```chronoscript
   // Create an inventory system using arrays and objects
   let inventory = [];  // Start with an empty gathering
   // Add some items to the inventory
   // Then write a function to find an item by name
   ```

3. **The Timekeeper's Challenge**
   ```chronoscript
   // Create a function that tells you how long until the next hour
   fn minutesUntilNextHour() {
       let now = Clock().now();
       // Your code here
   }
   ```

Remember, young Weaver: Every bug is just a story waiting to be told, and every error message is a lesson from the RUNTIME. May your threads never tangle and your patterns hold strong!