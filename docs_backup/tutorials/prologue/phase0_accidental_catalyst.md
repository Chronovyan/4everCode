---
title: 'Phase 0: The Accidental Catalyst'
description: Documentation for tutorials\prologue\phase0_accidental_catalyst.md
weight: 140
draft: true
---

# Phase 0: The Accidental Catalyst
*Setting: Professor Thorne's Lab - Prologue*

## The Discovery

*A dimly lit, cluttered university research lab. Dust motes dance in the few shafts of light penetrating the gloom. Old equipment, draped in sheets, lines the walls. Alex, an intern, looks around with a mix of curiosity and trepidation, having just found a way into this forgotten section.*

Alex pushes open a heavy, unmarked door, revealing a section of the university's advanced research wing that hasn't seen use in months, maybe longer. This was Professor Alistair Thorne's domain before his abrupt, unexplained "sabbatical." The air is stale, carrying the scent of old electronics and ozone. Equipment of strange design, a bizarre fusion of modern components and what look like ancient artifacts, are haphazardly arranged.

Towards the back, one workstation stands out. It's a custom-built terminal, wires snaking from it to various enigmatic devices. Its large monitor is surprisingly active, displaying lines of an elegant, unfamiliar script that glows with a soft, amber light. Beside the terminal, slumped against a rack of components, is a small, metallic drone, about the size of a house cat. It's clearly one of Thorne's prototypes – dented, wires exposed in places, and utterly lifeless. This must be "CORA," a "Cognitive Operations & Rifting Assistant" mentioned cryptically in some of Thorne's requisition forms Alex had seen earlier.

## First Interaction

*(Player, as Alex, can interact with the terminal. Upon interaction, the screen focuses on a section of the Chronovyan code.)*

**CORA (Voice, initially weak and distorted, coming from the drone as a result of the interaction below):** "...power... detected..."

**On-Screen Text (Thorne's Terminal - Initial View):**

```chronovyan
// --- Thorne's Dimensional Resonance Project - Log 7.3 ---
// System Status: Standby. Rift Calibrator Array awaiting alignment.
// Current Parameters:

DECLARE CONF rift_focus_intensity : INT = 35; // Default. Range: 0-100. Critical for stable aperture.
DECLARE CONF target_dimension_vector : VECTOR = [4.7, 8.1, -2.3, 0.99]; // Do NOT alter without full recalibration!
DECLARE CONF temporal_phase_angle : FLOAT = 12.7; // Degrees. Sensitive.

// --- Primary Calibration Sequence ---
// To initiate, ensure rift_focus_intensity is optimal (target: 70-80).
// Then, execute: 'initiate_calibration_pulse(rift_focus_intensity)'.
// Monitor harmonic resonator feedback.
// CAUTION: Unstable intensity may lead to unpredictable field effects.
```

**(Gameplay):** Alex reads Thorne's comments. The line `DECLARE CONF rift_focus_intensity : INT = 35;` is highlighted or selectable. A small, inert "Harmonic Calibrator" device connected to the terminal has a dim, unlit indicator light labeled "Optimal Intensity."

**CORA (Voice, still weak):** "Intensity... low..."

## Modifying a Variable

**(Gameplay):** Alex decides to change the value.
* Player interacts with `DECLARE CONF rift_focus_intensity : INT = 35;`.
* An input field appears, or Alex can directly type to modify the value.
* Alex changes it to `DECLARE CONF rift_focus_intensity : INT = 75;`.

**On-Screen Text (Thorne's Terminal - After Modification):**

```chronovyan
// --- Thorne's Dimensional Resonance Project - Log 7.3 ---
// System Status: Standby. Rift Calibrator Array awaiting alignment.
// Current Parameters:

DECLARE CONF rift_focus_intensity : INT = 75; // Default. Range: 0-100. Critical for stable aperture.
DECLARE CONF target_dimension_vector : VECTOR = [4.7, 8.1, -2.3, 0.99]; // Do NOT alter without full recalibration!
DECLARE CONF temporal_phase_angle : FLOAT = 12.7; // Degrees. Sensitive.

// --- Primary Calibration Sequence ---
// To initiate, ensure rift_focus_intensity is optimal (target: 70-80).
// Then, execute: 'initiate_calibration_pulse(rift_focus_intensity)'.
// Monitor harmonic resonator feedback.
// CAUTION: Unstable intensity may lead to unpredictable field effects.
```

**(Feedback):** The "Optimal Intensity" indicator light on the Harmonic Calibrator device flickers and then glows a steady green. A faint hum emanates from it. The dormant CORA drone beside the terminal twitches, its optical sensor flickering weakly with a blue light.

**CORA (Voice, slightly clearer):** "Energy... flow... detected. System... partially... active. I am CORA. Identity: Cognitive... Operations... Rifting... Assistant. Status: Damaged. Power: Minimal."

## Executing a Command

**(Gameplay):** Alex, surprised but intrigued, now focuses on Thorne's next instruction: `initiate_calibration_pulse(rift_focus_intensity)`. There's a command input line at the bottom of the terminal screen.

**CORA:** "Professor Thorne's sequence... calibration pulse... required."

**(Gameplay):** Alex types the command into the input line.
* Player types: `initiate_calibration_pulse(rift_focus_intensity)`
* Player hits "Execute" or Enter.

**On-Screen Text (Thorne's Terminal - Command Output Log):**

```
> initiate_calibration_pulse(75)
Initializing calibration pulse with intensity 75...
Pulse sequence engaged.
Harmonic Resonator Feedback: [][][][][][][] Resonance stable.
...
WARNING: Unexpected energy surge detected in primary conduit!
WARNING: Containment field fluctuating!
CRITICAL: Dimensional aperture forming! Evacuate! Evacuate!
```

## The Rift

**(Feedback & Scene):** As the command executes, the Harmonic Calibrator emits a series of bright blue pulses. The hum from various connected devices intensifies rapidly. CORA's optical sensor brightens considerably.

**CORA:** "Alex! Unforeseen energy cascade! The rift parameters are... unstable! We must—"

Suddenly, the room is filled with an intense, blinding light emanating from a point just above the main experimental platform. A shimmering, tearing distortion appears in mid-air – a swirling vortex of colors Alex has never seen. The pull is immense. Loose papers, small tools, and dust are sucked towards it. Alex stumbles, trying to grab onto something. CORA, now fully lit but still mostly immobile, slides across the floor.

**Alex (Sound of surprise/effort):** "Whoa!"

**(Scene):** Alex and CORA are drawn inexorably into the swirling portal. With a final, violent wrench and a sound like tearing fabric, they are pulled through. The portal snaps shut, plunging Thorne's lab back into dusty silence, save for the faint, residual crackle of energy.

## Chronovyan Code Snippets

Below are the key code snippets that appeared on Thorne's Terminal during this prologue:

### Snippet 1: Variable Declaration & Modification (Focus)

```chronovyan
// System Status: Standby. Rift Calibrator Array awaiting alignment.
// Current Parameters:

DECLARE CONF rift_focus_intensity : INT = 35; // <<< ALEX MODIFIES THIS VALUE
                               // Comments from Thorne:
                               // Default. Range: 0-100.
                               // Critical for stable aperture.
                               // Optimal for calibration: 70-80.

DECLARE CONF target_dimension_vector : VECTOR = [4.7, 8.1, -2.3, 0.99];
DECLARE CONF temporal_phase_angle : FLOAT = 12.7;
```

### Snippet 2: Command / Function Call (Focus)

```chronovyan
// --- Primary Calibration Sequence ---
// To initiate, ensure rift_focus_intensity is optimal (target: 70-80).
// Then, execute: 'initiate_calibration_pulse(rift_focus_intensity)'.  <<< ALEX TYPES THIS COMMAND
// Monitor harmonic resonator feedback.
// CAUTION: Unstable intensity may lead to unpredictable field effects.

// (Hidden function definition, not initially visible or editable by Alex, but executed by the system)
func initiate_calibration_pulse(intensity_value : NUMBER) {
    // [Internal Chronovyan logic for the pulse based on intensity_value]
    // ...
    // This is where the unintended rift activation logic would be triggered by Thorne's setup.
    // ...
    IF (intensity_value > 70 && intensity_value < 85) {
        // Trigger unintended higher-level process if conditions are "just right"
        activate_emergency_rift_protocol_UNSTABLE(intensity_value, target_dimension_vector);
    }
    // ...
}
```