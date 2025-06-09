# 4ever Documentation Restructuring Plan

## Current Issues

1. **Duplicate Content**
   - Multiple documents covering similar topics
   - Inconsistent terminology and examples

2. **Structural Problems**
   - No clear hierarchy
   - Missing cross-references
   - Inconsistent formatting

3. **Content Gaps**
   - Undefined core concepts
   - Missing examples
   - Incomplete API references

## Proposed Structure

```
docs/
├── core/
│   ├── 01_core_concepts.md          # Philosophy and fundamentals
│   ├── 02_variables.md              # Unified variables documentation
│   ├── 03_control_flow.md           # Temporal control structures
│   ├── 04_resource_management.md    # Aethel and Chronon
│   └── 05_standard_library.md       # Core libraries
├── guides/
│   ├── getting_started.md
│   ├── temporal_patterns.md
│   └── best_practices.md
└── reference/
    ├── syntax_cheatsheet.md
    └── api/
        ├── core_types.md
        └── runtime_api.md
```

## Implementation Steps

1. **Phase 1: Consolidation**
   - [x] Merge variable documentation
   - [ ] Merge duplicate concept documents
   - [ ] Remove deprecated files

2. **Phase 2: Standardization**
   - [ ] Create style guide
   - [ ] Standardize terminology
   - [ ] Implement consistent formatting

3. **Phase 3: Enhancement**
   - [ ] Add missing examples
   - [ ] Implement cross-references
   - [ ] Add versioning information

## Naming Conventions

### Variables
- Use `CONF` and `REB` prefixes consistently
- Prefer `camelCase` for variable names
- Use descriptive, intention-revealing names

### Documents
- Use `snake_case.md` for filenames
- Include version and last updated in frontmatter
- Use consistent header levels

## Next Steps

1. Review the new variables documentation
2. Identify any missing concepts
3. Plan the consolidation of remaining documents

---
*Last Updated: 2025-06-09*
