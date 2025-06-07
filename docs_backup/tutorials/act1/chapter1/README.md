---
title: 'Act I, Chapter 1: First Echoes in Chronovya'
description: Documentation for tutorials\act1\chapter1\README.md
weight: 150
draft: true
---

# Act I, Chapter 1: First Echoes in Chronovya

## Introduction

After the crash landing on the alien world of Chronovya, Alex and CORA find themselves in an unfamiliar landscape filled with strange technology and mysterious structures. Their immediate goals are clear: ensure survival, understand their surroundings, and take the first steps to assess this alien environment.

This chapter focuses on the most fundamental interactions with Chronovyan technology and serves as an introduction to the Chronovyan programming language. Players will learn to communicate with the alien world through basic programming concepts, allowing them to gather essential information about their surroundings and establish a foothold in this strange new place.

## Learning Objectives

By completing this chapter, players will learn and practice the following programming concepts:

- Basic variable assignment and manipulation
- Reading data from sensor-like objects
- Calling simple methods on objects
- Understanding and using return values
- Using `print_to_cora()` for output and feedback
- Working with basic data types (numbers, strings, booleans)
- Type conversion between different data formats
- Basic conditional logic (if/else statements)
- String concatenation and formatting
- Simple environmental data analysis

## Quests in this Chapter

### Arrival Narrative

*Note: The arrival narrative provides context and background for this chapter's events. It introduces Alex and CORA to Chronovya and sets up the initial challenges they face.*

- Link to narrative: [Arrival Narrative](./00_arrival_narrative.md)

### Side Quest: "The Whispering Stone"

Alex discovers a strange, humming stone that seems to be gathering and processing environmental data. With CORA's help, they must learn to communicate with this alien technology to extract valuable atmospheric information.

- **Description:** Learn to interact with a Chronovyan sensor (the Whispering Stone) to gather atmospheric pressure readings
- **Key concepts:** Object interaction (`object.method()`), storing return values, basic output
- **Link to side quest:** [The Whispering Stone](../../examples/side_quests/01_the_whispering_stone.md)
- **Link to example code:** [Sensor Reading Example](../../examples/01_beginner/act1_ch1_ex1_sensor_reading.cvy)

### Side Quest: "Crystal Calibration"

A cluster of colored crystals holds crucial environmental data, but their formats are corrupted. Alex must write code to properly interpret and convert the data stored within them.

- **Description:** Calibrate Chronovyan data crystals by understanding and converting between different data types
- **Key concepts:** Data types, type conversion functions (e.g., `convert_to_number()`, `convert_to_boolean()`), working with variables
- **Link to side quest:** [Crystal Calibration](../../examples/side_quests/02_crystal_calibration.md)
- **Link to example code:** [Environmental Data Translation](../../examples/01_beginner/act1_ch1_ex2_environmental_data_translation.cvy)

### Main Quest: "Mapping the Safe Zone"

Three mysterious monoliths stand silent in the landscape. Alex must activate them using the knowledge gained from the side quests to generate a map of the surrounding area, identifying safe zones and available resources.

- **Description:** Activate three Wayfinder Monoliths to generate a map of the local area
- **Key concepts applied:** Synthesizes skills from the prerequisite side quests, including sensor reading, data type handling, variable manipulation, and string formatting for activation pings
- **Link to main quest:** [Mapping the Safe Zone](../../examples/main_quests/act1_ch1_mq1_mapping_the_safe_zone.md)

## Expected Outcome

By the end of this chapter, Alex will have:

- A basic understanding of how to interact with simple Chronovyan systems
- The ability to read and interpret data from alien technology
- Experience handling and converting between different data types
- Created a small, mapped safe zone to serve as a base of operations
- The foundation of programming knowledge needed to face more complex challenges

Having established this initial foothold, Alex and CORA will be ready to venture further into Chronovya, exploring more advanced systems and uncovering the secrets of this mysterious world in the chapters to come.