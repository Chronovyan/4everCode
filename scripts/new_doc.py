#!/usr/bin/env python3
"""
Documentation Generator for 4ever

This script helps create new documentation pages with proper front matter and structure.
"""

import argparse
import os
import re
import sys
from datetime import datetime
from pathlib import Path
from typing import Optional, Dict, Any

# Configuration
TEMPLATES = {
    'guide': {
        'template': """---
title: '{title}'
description: '{description}'
weight: 100
draft: true
---

# {title}

## Overview

[Brief overview of the topic]

## Prerequisites

- [Prerequisite 1]
- [Prerequisite 2]

## Step-by-Step Guide

### Step 1: [First Step]

[Content]

### Step 2: [Next Step]

[Content]

## Best Practices

- [Best practice 1]
- [Best practice 2]

## Common Pitfalls

- [Common mistake 1]
- [Common mistake 2]

## Next Steps

- [Related topic 1]
- [Related topic 2]

## See Also

- [Related documentation]
- [External resources]
""",
        'description': 'A step-by-step guide with examples and best practices.',
        'default_category': 'guides'
    },
    'reference': {
        'template': """---
title: '{title}'
description: '{description}'
weight: 100
draft: true
---

# {title}

## Overview

[Brief description of the reference topic]

## Syntax

```4ever
// Syntax example
```

## Parameters

| Name | Type | Description |
|------|------|-------------|
| param1 | Type | Description |

## Return Value

[Description of return value]

## Examples

### Basic Usage

```4ever
// Example code
```

### Advanced Usage

```4ever
// More complex example
```

## Notes

[Additional notes or edge cases]

## See Also

- [Related documentation]
""",
        'description': 'API reference documentation with syntax and examples.',
        'default_category': 'reference'
    },
    'concept': {
        'template': """---
title: '{title}'
description: '{description}'
weight: 100
draft: true
---

# {title}

## Introduction

[Brief introduction to the concept]

## Background

[Historical context or background information]

## Core Principles

### Principle 1

[Description]

### Principle 2

[Description]

## Use Cases

- [Use case 1]
- [Use case 2]

## Implementation Details

[Technical details about implementation]

## Best Practices

- [Best practice 1]
- [Best practice 2]

## Related Concepts

- [Related concept 1]
- [Related concept 2]

## Further Reading

- [Link to external resources]
""",
        'description': 'Conceptual documentation explaining a topic in depth.',
        'default_category': 'core'
    },
    'tutorial': {
        'template': """---
title: 'Tutorial: {title}'
description: '{description}'
weight: 100
draft: true
---

# Tutorial: {title}

## Introduction

[Brief introduction to the tutorial]

## Learning Objectives

By the end of this tutorial, you will be able to:

- [Objective 1]
- [Objective 2]
- [Objective 3]

## Prerequisites

- [Prerequisite 1]
- [Prerequisite 2]

## Setup

[Setup instructions]

## Step 1: [First Step]

[Detailed instructions]

```4ever
// Example code
```

## Step 2: [Next Step]

[Detailed instructions]

```4ever
// Example code
```

## Conclusion

[Summary of what was covered]

## Next Steps

- [Next tutorial]
- [Related documentation]

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Common issue | Solution |

## Additional Resources

- [Link to resources]
""",
        'description': 'A hands-on tutorial with step-by-step instructions.',
        'default_category': 'tutorials'
    }
}

def sanitize_filename(title: str) -> str:
    """Convert a title to a URL-friendly filename."""
    # Convert to lowercase
    filename = title.lower()
    # Replace spaces with hyphens
    filename = re.sub(r'\s+', '-', filename)
    # Remove invalid characters
    filename = re.sub(r'[^a-z0-9\-]', '', filename)
    # Remove multiple consecutive hyphens
    filename = re.sub(r'-+', '-', filename).strip('-')
    return f"{filename}.md"

def create_document(
    title: str,
    doc_type: str,
    category: str,
    output_dir: Optional[Path] = None
) -> Path:
    """Create a new documentation file with the given template."""
    if doc_type not in TEMPLATES:
        raise ValueError(f"Unknown document type: {doc_type}. Available types: {', '.join(TEMPLATES.keys())}")
    
    template = TEMPLATES[doc_type]
    
    # Prepare variables for template
    context = {
        'title': title,
        'description': f"Learn about {title} in 4ever",
        'date': datetime.now().strftime('%Y-%m-%d'),
        'year': datetime.now().year
    }
    
    # Render template
    content = template['template'].format(**context)
    
    # Determine output directory
    if output_dir is None:
        output_dir = Path('docs') / category
        output_dir.mkdir(parents=True, exist_ok=True)
    
    # Create output filename
    filename = sanitize_filename(title)
    output_path = output_dir / filename
    
    # Write file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)
    
    return output_path

def main() -> int:
    """Main entry point for the script."""
    parser = argparse.ArgumentParser(description='Create a new documentation page.')
    parser.add_argument('title', help='Title of the documentation page')
    parser.add_argument('--type', '-t', choices=TEMPLATES.keys(), default='guide',
                      help=f'Type of document to create (default: guide)')
    parser.add_argument('--category', '-c', help='Documentation category (e.g., guides, reference)')
    parser.add_argument('--output-dir', '-o', help='Output directory (default: docs/{category})')
    parser.add_argument('--list-types', action='store_true', help='List available document types')
    
    args = parser.parse_args()
    
    if args.list_types:
        print("Available document types:")
        for doc_type, info in TEMPLATES.items():
            print(f"  {doc_type}: {info['description']}")
        return 0
    
    # Determine category
    if args.category:
        category = args.category
    else:
        category = TEMPLATES[args.type]['default_category']
    
    # Create output directory if specified
    output_dir = Path(args.output_dir) if args.output_dir else None
    
    try:
        output_path = create_document(
            title=args.title,
            doc_type=args.type,
            category=category,
            output_dir=output_dir
        )
        print(f"Created new document: {output_path}")
        return 0
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

if __name__ == "__main__":
    sys.exit(main())
