---
title: Chronovyan Documentation Style Guide
description: Documentation for contributing\style-guide.md
weight: 120
draft: true
---

# Chronovyan Documentation Style Guide

## Purpose
This guide ensures consistency, clarity, and professionalism across all Chronovyan documentation.

## Voice and Tone
- **Professional but approachable** - Be clear and precise while remaining friendly.
- **Active voice** - Prefer active voice over passive voice.
- **Second person** - Address the reader as "you" when giving instructions.
- **Inclusive language** - Use gender-neutral terms and be mindful of cultural differences.

## Formatting

### Headers
- Use sentence case for headers (only capitalize the first word and proper nouns)
- Use `#` for main title, `##` for main sections, `###` for subsections, etc.
- Include exactly one blank line before and after headers

### Text
- Use line breaks at 100 characters for better diff views
- Use one sentence per line for better version control
- Use **bold** for UI elements and `code` for code elements
- Use *italics* for emphasis (sparingly)

### Code Examples
- Include comments explaining non-obvious code
- Show both input and expected output when relevant
- Use syntax highlighting (specify language after the opening ```)
- Keep examples focused and minimal

### Lists
- Use ordered lists for sequential steps
- Use bullet points for non-sequential items
- Start each item with a capital letter
- Don't use punctuation at the end of list items unless they're complete sentences

### Links
- Use descriptive link text (not "click here")
- Place links at the end of sentences when possible
- Use relative paths for internal links

### Images
- Include alt text for all images
- Place images in the `docs/images/` directory
- Use descriptive filenames (e.g., `temporal-flow-diagram.png`)
- Include captions when helpful

## Content Guidelines

### Structure
1. Start with what the user needs to know first
2. Progress from basic to advanced concepts
3. Include practical examples
4. Link to related content
5. End with next steps or related resources

### Language
- Use present tense
- Be concise but thorough
- Define technical terms on first use
- Use consistent terminology throughout
- Avoid jargon when possible, or define it clearly

### Examples
- Show both simple and complex examples when appropriate
- Include comments explaining the "why" not just the "what"
- Use realistic but simple scenarios

## Documentation Types

### Tutorials
- Task-oriented
- Include clear learning objectives
- Provide step-by-step instructions
- Include verification steps

### How-to Guides
- Focus on solving specific problems
- Include prerequisites
- Provide clear, repeatable steps
- Include troubleshooting tips

### Reference
- Be comprehensive and precise
- Include parameter and return value descriptions
- Provide code examples
- Document edge cases and error conditions

### Explanation
- Provide context and background
- Explain the "why" behind concepts
- Use diagrams when helpful
- Link to related reference material

## Metadata
Each documentation file should include YAML front matter:

```yaml
---
title: "Document Title"
description: "Brief description of the content"
weight: 10  # For ordering in navigation
---
```

## Review Process
1. Self-review against this style guide
2. Technical accuracy review
3. Editorial review for clarity and style
4. Final review for consistency with other documentation