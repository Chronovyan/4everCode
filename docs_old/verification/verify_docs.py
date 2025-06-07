#!/usr/bin/env python3
"""
Chronovyan Documentation Verification Tool

This script helps verify that documentation matches the codebase by:
1. Scanning markdown files for code blocks
2. Checking for broken links and images
3. Verifying code examples against the codebase
4. Checking for TODOs and FIXMEs
5. Validating front matter in markdown files
6. Checking for dead links in the documentation
7. Verifying code examples compile and run
8. Enforcing style guide compliance
"""

import argparse
import json
import os
import re
import subprocess
import sys
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple, Union

import yaml
from dataclasses import dataclass, field

@dataclass
class DocumentIssue:
    type: str
    file: str
    line: int
    message: str
    context: str = ""
    code: str = ""
    severity: str = "warning"  # 'error', 'warning', or 'info'
    suggestion: str = ""  # Optional suggestion for fixing the issue

def get_code_context(content: str, block: str, context_lines: int = 3) -> str:
    """Extract context around a code block for better error reporting."""
    lines = content.split('\n')
    block_start = content.find(block)
    if block_start == -1:
        return ''
        
    # Find the line numbers
    line_num = content.count('\n', 0, block_start) + 1
    start_line = max(1, line_num - context_lines)
    end_line = min(len(lines), line_num + block.count('\n') + context_lines)
    
    # Format the context with line numbers
    context = []
    for i in range(start_line - 1, end_line):
        prefix = '> ' if i + 1 == line_num else '  '
        context.append(f"{i+1:4d}{prefix}{lines[i]}")
    
    return '\n'.join(context)

def has_documentation_comment(content: str, block: str) -> bool:
    """Check if a code block has a documentation comment before it."""
    block_start = content.find(block)
    if block_start == -1:
        return False
        
    # Look for comments in the 5 lines before the block
    lines = content[:block_start].split('\n')[-5:]
    return any(line.strip().startswith(('//', '#')) for line in lines)

class DocumentationVerifier:
    def __init__(self, root_dir: str = '.'):
        self.root_dir = Path(root_dir).resolve()
        self.docs_dir = self.root_dir / 'docs'
        self.code_dir = self.root_dir / 'src'
        self.issues: List[DocumentIssue] = []
        self.valid_links: Set[str] = set()
        self.processed_files: Set[Path] = set()
        
        # Known valid sections from mkdocs.yml
        self.known_sections = {
            'Home', 'Getting Started', 'Language Guide', 'Tutorials', 
            'API Reference', 'Standard Library', 'Glossary', 'Roadmap',
            'Contributing'
        }
        
        # Chronovyan-specific patterns
        self.chronovyan_keywords = {
            'ANTECEDENCE', 'CONCURRENCY', 'CONSEQUENCE', 'CHRONON', 'AETHEL',
            'WEAVE', 'UNWEAVE', 'TEMPORAL', 'PARADOX', 'ANCHOR', 'REBEL',
            'SEEKER', 'STABILIZE', 'FLUX', 'ORDER', 'CHRONOSTREAM', 'PARADOX_CHECK'
        }
        
        # Style guide patterns
        self.style_patterns = {
            'missing_paradox_check': (
                r'(?:branch_timeline|merge_timelines|weave_pattern|temporal_jump|stabilize)\s*\(',
                'Temporal operations should be wrapped in PARADOX_CHECK',
                'warning'
            ),
            'missing_error_handling': (
                r'DECLARE\s+[A-Za-z_]\w*\s*:\s*[A-Z]+\s*=',
                'Consider adding error handling for resource declarations',
                'info'
            ),
            'temporal_resource_cleanup': (
                r'RESOURCE\s+[A-Za-z_]\w*',
                'Ensure resources are properly released in CLEANUP blocks',
                'warning'
            ),
            'naming_convention': (
                r'DECLARE\s+([A-Za-z_][A-Za-z0-9_]*)\s*:',
                'Variables should follow camelCase, Constants should be UPPER_SNAKE_CASE',
                'warning'
            )
        }
        
        # Resource management patterns
        self.resource_patterns = {
            'resource_declaration': r'RESOURCE\s+[A-Za-z_][A-Za-z0-9_]*',
            'temporal_operation': r'TEMPORAL\s+[A-Za-z_][A-Za-z0-9_]*',
            'weave_pattern': r'WEAVE\s*\([^)]+\)',
            'paradox_check': r'PARADOX_CHECK\s*\(',
            'temporal_assignment': r'[A-Za-z_][A-Za-z0-9_]*\s*<~\s*'
        }
        
    def find_markdown_files(self) -> List[Path]:
        """Find all markdown files in the documentation directory."""
        markdown_files = []
        for ext in ['*.md', '*.markdown']:
            markdown_files.extend(Path(self.docs_dir).rglob(ext))
        return markdown_files
        
        # Required front matter fields for different doc types
        self.required_front_matter = {
            'tutorials/': ['title', 'description', 'prerequisites', 'learning_objectives'],
            'api/': ['title', 'description', 'parameters', 'returns', 'raises'],
            'guides/': ['title', 'description', 'last_updated', 'author']
        }
        
    def find_markdown_files(self) -> List[Path]:
        """Find all markdown files in the docs directory."""
        files = list(self.docs_dir.rglob('*.md'))
        # Sort files by path for consistent processing
        return sorted(files, key=lambda p: str(p).lower())
    
    def validate_front_matter(self, file_path: Path, content: str, front_matter: dict) -> None:
        """Validate front matter fields in a markdown file.
        
        Args:
            file_path: Path to the markdown file
            content: Full content of the file
            front_matter: Parsed front matter as a dictionary
        """
        if not front_matter:
            self.issues.append(DocumentIssue(
                type='documentation',
                file=str(file_path.relative_to(self.root_dir)),
                line=1,
                message='Missing front matter',
                severity='high',
                code='FM101',
                suggestion='Add YAML front matter with required fields',
                context='Start of file'
            ))
            return
        
        # Required fields for all documents
        required_fields = {
            'title': 'Document title',
            'description': 'Brief description of the document',
            'date_created': 'YYYY-MM-DD',
            'status': 'draft | in-progress | review | published'
        }
        
        # Additional required fields for temporal operation documents
        if any(keyword in content for keyword in ['PARADOX_CHECK', 'REB::', 'WEAVE_PATTERN']):
            required_fields.update({
                'temporal_impact': 'low | medium | high',
                'paradox_risk': 'true | false',
                'reviewed_by': 'Name of reviewer'
            })
        
        # Check for missing required fields
        for field, description in required_fields.items():
            if field not in front_matter:
                self.issues.append(DocumentIssue(
                    type='documentation',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message=f'Missing recommended front matter field: {field}',
                    severity='high' if field in ['title', 'description'] else 'medium',
                    code=f'FM102',
                    suggestion=f'Add {field} to front matter. Expected: {description}',
                    context='Front matter section'
                ))
        
        # Validate status field values
        if 'status' in front_matter and front_matter['status'] not in ['draft', 'in-progress', 'review', 'published']:
            self.issues.append(DocumentIssue(
                type='documentation',
                file=str(file_path.relative_to(self.root_dir)),
                line=1,
                message='Invalid status value in front matter',
                severity='warning',
                code='FM103',
                suggestion='Status must be one of: draft, in-progress, review, published',
                context=f'status: {front_matter["status"]}'
            ))
        
        # Check for review date if status is published
        if front_matter.get('status') == 'published' and 'last_reviewed' not in front_matter:
            self.issues.append(DocumentIssue(
                type='documentation',
                file=str(file_path.relative_to(self.root_dir)),
                line=1,
                message='Published document missing last_reviewed date',
                severity='warning',
                code='FM104',
                suggestion='Add last_reviewed: YYYY-MM-DD to front matter',
                context='Front matter section'
            ))
    
    def extract_code_blocks(self, file_path: Path) -> List[Dict]:
        """Extract code blocks from a markdown file."""
        content = file_path.read_text(encoding='utf-8')
        code_blocks = []
        
        # Check for TODOs and FIXMEs
        for i, line in enumerate(content.split('\n'), 1):
            if 'TODO' in line.upper() or 'FIXME' in line.upper():
                self.issues.append(DocumentIssue(
                    type='todo_fixme',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=i,
                    message='TODO/FIXME found',
                    context=line.strip(),
                    severity='info'
                ))
        
        # Pattern to match code blocks with optional language specifier
        pattern = re.compile(r'```(\w*)\n(.*?)```', re.DOTALL)
        
        for match in pattern.finditer(content):
            lang = match.group(1) or 'text'
            code = match.group(2).strip()
            line_number = content[:match.start()].count('\n') + 1
            
            # Check for syntax errors in code blocks
            if lang in ['python', 'chronoscript']:
                self._check_code_syntax(code, file_path, line_number, lang)
            
            code_blocks.append({
                'language': lang,
                'code': code,
                'line_number': line_number
            })
            
        return code_blocks
        
    def _check_code_syntax(self, code: str, file_path: Path, line_number: int, lang: str) -> None:
        """Check syntax of code blocks."""
        try:
            if lang == 'python':
                compile(code, '<string>', 'exec')
            # Add more language-specific syntax checks here
        except SyntaxError as e:
            self.issues.append(DocumentIssue(
                type='syntax_error',
                file=str(file_path.relative_to(self.root_dir)),
                line=line_number + e.lineno - 1 if e.lineno else line_number,
                message=f'Syntax error in {lang} code: {str(e)}',
                code=code.split('\n')[e.lineno-1] if e.lineno else code.split('\n')[0],
                severity='error'
            ))
    
    def check_links(self, file_path: Path) -> List[DocumentIssue]:
        """Check for broken links and images in a markdown file."""
        issues: List[DocumentIssue] = []
        content = file_path.read_text(encoding='utf-8')
        
        # Patterns to match markdown links and images
        link_pattern = re.compile(r'\[(.*?)\]\((.*?)(?:\s+".*?")?\)')
        image_pattern = re.compile(r'!\[(.*?)\]\((.*?)(?:\s+".*?")?\)')
        
        def process_link(link_text: str, link_url: str, line_number: int, is_image: bool = False) -> None:
            # Skip empty or special links
            if not link_url or link_url.startswith(('http://', 'https://', 'mailto:', '#')):
                return
                
            # Handle relative links
            if link_url.startswith('/'):
                # Absolute path from repo root
                target_path = self.root_dir / link_url[1:]
            else:
                # Relative to current file
                target_path = file_path.parent / link_url
            
            # Clean up the path (handle '../' and './')
            target_path = target_path.resolve()
            
            # Check if target exists
            if not target_path.exists():
                issues.append(DocumentIssue(
                    type='broken_image_link' if is_image else 'broken_link',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=line_number,
                    message=f'Broken {"image " if is_image else ""}link: {link_url}',
                    context=link_text[:100],
                    severity='error'
                ))
            else:
                # Track valid links for dead link detection
                self.valid_links.add(str(target_path.relative_to(self.root_dir)))
        
        # Process regular links
        for match in link_pattern.finditer(content):
            link_text = match.group(1)
            link_url = match.group(2)
            line_number = content[:match.start()].count('\n') + 1
            process_link(link_text, link_url, line_number)
        
        # Process image links
        for match in image_pattern.finditer(content):
            alt_text = match.group(1)
            image_url = match.group(2)
            line_number = content[:match.start()].count('\n') + 1
            process_link(alt_text, image_url, line_number, is_image=True)
        
        return issues
    
    def verify_code_blocks(self, file_path: Path, code_blocks: List[Dict]) -> None:
        """Verify code blocks against the codebase."""
        for block in code_blocks:
            if block['language'] == 'chronoscript':
                self._verify_chronoscript_code(file_path, block)
            elif block['language'] == 'python':
                self._verify_python_code(file_path, block)
            # Add more language-specific verifiers as needed
    
    def _verify_chronoscript_code(self, file_path: Path, block: Dict) -> None:
        """Verify Chronoscript code blocks with Chronovyan-specific checks."""
        # Get the content of the code block
        code = block['code']
        lines = code.splitlines()
        
        # Validate code style for this block
        self._validate_code_style(file_path, code, lines)
        
        # Check resource management patterns
        for line_num, line in enumerate(lines, 1):
            # Check for resource declarations without proper management
            if 'RESOURCE' in line and 'MANAGE' not in line:
                self.issues.append(DocumentIssue(
                    type='unmanaged_resource',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=block['line_number'] + line_num - 1,
                    message='Resource declared without proper management',
                    code=line.strip(),
                    severity='warning'
                ))
            
            # Check for temporal operations without paradox handling
            if any(op in line for op in ['WEAVE', 'UNWEAVE', 'SHIFT']):
                has_paradox_check = any('PARADOX_CHECK' in l for l in lines[line_num:line_num+5])
                if not has_paradox_check:
                    self.issues.append(DocumentIssue(
                        type='missing_paradox_check',
                        file=str(file_path.relative_to(self.root_dir)),
                        line=block['line_number'] + line_num - 1,
                        message='Temporal operation without nearby paradox check',
                        code=line.strip(),
                        severity='warning'
                    ))
        
        # Check for proper resource cleanup
        if 'RESOURCE' in code and 'RELEASE' not in code:
            self.issues.append(DocumentIssue(
                type='missing_resource_cleanup',
                file=str(file_path.relative_to(self.root_dir)),
                line=block['line_number'],
                message='Resource declared but not released',
                severity='warning'
            ))
            
        # Check for temporal debt accumulation
        if 'AETHEL' in code and 'CHRONON' in code:
            aethel_idx = code.find('AETHEL')
            chronon_idx = code.find('CHRONON')
            if abs(aethel_idx - chronon_idx) < 20:  # If they appear close together
                self.issues.append(DocumentIssue(
                    type='temporal_debt_risk',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=block['line_number'] + code[:max(aethel_idx, chronon_idx)].count('\n'),
                    message='Potential temporal debt accumulation (AETHEL and CHRONON used closely)',
                    severity='warning'
                ))
        
    def _verify_python_code(self, file_path: Path, block: Dict) -> None:
        """Verify Python code blocks."""
        code = block['code']
        
        # Check for common Python issues
        if 'print(' in code and 'from __future__ import print_function' not in code:
            self.issues.append(DocumentIssue(
                type='python_print',
                file=str(file_path.relative_to(self.root_dir)),
                line=block['line_number'],
                message='Use of print() in code example',
                severity='warning',
                code=code.split('\n')[0][:50] + ('...' if '\n' in code else '')
            ))
    
    def verify_document(self, file_path: Path) -> None:
        """Verify a single documentation file with enhanced checks."""
        try:
            # Try UTF-8 first, fall back to UTF-8 with BOM, then to latin-1
            try:
                content = file_path.read_text(encoding='utf-8')
            except UnicodeDecodeError:
                try:
                    content = file_path.read_text(encoding='utf-8-sig')
                except UnicodeDecodeError:
                    content = file_path.read_text(encoding='latin-1')
                    self.issues.append(DocumentIssue(
                        type='encoding_issue',
                        file=str(file_path.relative_to(self.root_dir)),
                        line=1,
                        message='File uses non-UTF-8 encoding (latin-1 fallback)',
                        severity='warning',
                        context='Consider converting to UTF-8 for better compatibility',
                        suggestion='Convert file to UTF-8 encoding',
                        code='ENC001'
                    ))

            relative_path = file_path.relative_to(self.root_dir)
            print(f"Verifying {relative_path}...")

            # Parse front matter
            front_matter = {}
            if content.startswith('---\n'):
                try:
                    end = content.find('---', 3)
                    if end > 0:
                        front_matter_text = content[3:end].strip()
                        front_matter = yaml.safe_load(front_matter_text) or {}
                except yaml.YAMLError as e:
                    self.issues.append(DocumentIssue(
                        type='documentation',
                        file=str(file_path.relative_to(self.root_dir)),
                        line=1,
                        message=f'Invalid YAML in front matter: {str(e)}',
                        severity='error',
                        code='invalid_front_matter',
                        suggestion='Fix the YAML syntax in the front matter',
                        context='Front matter section'
                    ))
            
            # Validate front matter with Chronovyan-specific rules
            self.validate_front_matter(file_path, content, front_matter)

            # Check for broken links and images
            self.issues.extend(self.check_links(file_path))

            # Extract and verify code blocks
            code_blocks = self.extract_code_blocks(file_path)
            self.verify_code_blocks(file_path, code_blocks)

            # Check for dead links and Chronovyan-specific linking patterns
            self._check_dead_links(file_path, content)

            # Additional Chronovyan-specific validations
            self._validate_chronovyan_content(file_path, content)

            # Mark file as processed
            self.processed_files.add(file_path)

        except Exception as e:
            self.issues.append(DocumentIssue(
                type='processing_error',
                file=str(file_path.relative_to(self.root_dir)),
                line=1,
                message=f'Error processing file: {str(e)}',
                severity='error',
                context=str(e)
            ))
        finally:
            # Mark file as processed even if there was an error
            self.processed_files.add(file_path)

    def _validate_chronovyan_content(self, file_path: Path, content: str) -> None:
        """Perform Chronovyan-specific content validations."""
        relative_path = str(file_path.relative_to(self.docs_dir)).replace('\\', '/')
        lines = content.split('\n')
        
        # Check if this is a documentation file
        is_docs = any(relative_path.startswith(p) for p in ['docs/', 'tutorials/', 'guides/'])
        
        if is_docs:
            # Documentation-specific validations
            self._validate_documentation_style(file_path, content, relative_path, lines)
        else:
            # Code-specific validations
            self._validate_code_style(file_path, content, lines)
    
    def _validate_code_style(self, file_path: Path, content: str, lines: List[str]) -> None:
        """Validate code style according to the Chronovyan style guide."""
        for i, line in enumerate(lines, 1):
            # Skip comments and empty lines
            if line.strip().startswith('//') or not line.strip():
                continue
                
            # Check for style guide violations
            for check_name, (pattern, message, severity) in self.style_patterns.items():
                if re.search(pattern, line):
                    # Get context (previous 2 lines and next 1 line)
                    context_start = max(0, i - 3)
                    context_end = min(len(lines), i + 2)
                    context = '\n'.join(f">{l}" if j+1 == i else f"  {l}" 
                                        for j, l in enumerate(lines[context_start:context_end], context_start + 1))
                    
                    # Add the issue with safe path handling
                    try:
                        file_path_str = str(file_path.relative_to(self.root_dir))
                    except ValueError:
                        file_path_str = str(file_path)
                        
                    self.issues.append(DocumentIssue(
                        type='style_guide',
                        file=file_path_str,
                        line=i,
                        message=message,
                        context=context,
                        severity=severity,
                        code=check_name
                    ))
    def _validate_documentation_style(self, file_path: Path, content: str, relative_path: str, lines: List[str]) -> None:
        """Validate documentation style according to the Chronovyan documentation guide."""
        # Check for required front matter
        if not content.startswith('---\n'):
            self.issues.append(DocumentIssue(
                type='documentation',
                file=str(file_path.relative_to(self.root_dir)),
                line=1,
                message='Missing front matter (YAML header)',
                severity='error',
                code='missing_front_matter',
                suggestion='Add YAML front matter with required fields',
                context='Start of file'
            ))
            # Pass empty dict as front matter since it's missing
            self.validate_front_matter(file_path, content, {})
        else:
            # Extract and validate front matter
            try:
                front_matter = yaml.safe_load(content.split('---\n', 2)[1])
                self.validate_front_matter(file_path, content, front_matter)
            except yaml.YAMLError as e:
                self.issues.append(DocumentIssue(
                    type='documentation',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message=f'Invalid YAML in front matter: {str(e)}',
                    severity='error',
                    code='invalid_front_matter',
                    suggestion='Fix the YAML syntax in the front matter',
                    context='Front matter section'
                ))
                # Pass empty dict as front matter since it's invalid
                self.validate_front_matter(file_path, content, {})
            
            # Check for proper section headers
            self._validate_section_headers(file_path, content, relative_path, lines)
            
            # Check for code block formatting
            self._validate_code_blocks(file_path, content, lines)
    
    def _check_recommended_fields(self, file_path: Path, front_matter: Dict, fields: List[str]) -> None:
        """Check for recommended fields in front matter."""
        for field in fields:
            if field not in front_matter:
                self.issues.append(DocumentIssue(
                    type='documentation',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message=f'Recommended field missing: {field}',
                    severity='info',
                    code=f'recommended_{field}'
                ))
    
    def _validate_section_headers(self, file_path: Path, content: str, relative_path: str, lines: List[str]) -> None:
        """Validate section headers in documentation."""
        # Check for proper header levels (no skipping levels)
        header_pattern = re.compile(r'^(#+)\s+(.*)')
        current_level = 1
        
        for i, line in enumerate(lines, 1):
            match = header_pattern.match(line)
            if match:
                level = len(match.group(1))
                if level > current_level + 1:
                    self.issues.append(DocumentIssue(
                        type='documentation',
                        file=str(file_path),
                        line=i,
                        message=f'Header level jumps from {current_level} to {level}',
                        severity='warning',
                        code='header_level_jump'
                    ))
                current_level = level
    
    def _validate_code_blocks(self, file_path: Path, content: str, lines: List[str]) -> None:
        """Validate code blocks in documentation."""
        in_code_block = False
        code_block_start = 0
        
        for i, line in enumerate(lines, 1):
            if line.strip().startswith('```'):
                if in_code_block:
                    # End of code block
                    in_code_block = False
                    
                    # Check if code block has a language specified
                    if not lines[code_block_start].strip().startswith('```chronoscript'):
                        self.issues.append(DocumentIssue(
                            type='documentation',
                            file=str(file_path),
                            line=code_block_start + 1,
                            message='Code block should specify a language (e.g., ```chronoscript)',
                            severity='warning',
                            code='missing_language_specifier'
                        ))
                else:
                    # Start of code block
                    in_code_block = True
                    code_block_start = i - 1  # Convert to 0-based index
    
    def _check_resource_cleanup(self, file_path: Path, content: str) -> None:
        """Check that resources are properly cleaned up."""
        lines = content.split('\n')
        resource_decls = []
        
        # First pass: Find all resource declarations
        for i, line in enumerate(lines, 1):
            if re.search(r'DECLARE\s+[A-Za-z_]\w*\s*:\s*RESOURCE', line):
                var_name = re.search(r'DECLARE\s+([A-Za-z_]\w*)', line)
                if var_name:
                    resource_decls.append((i, var_name.group(1)))
        
        # Second pass: Check cleanup
        for line_num, var_name in resource_decls:
            # Look for CLEANUP or FINALLY blocks that release this resource
            cleanup_found = False
            for i in range(line_num, min(line_num + 20, len(lines))):
                if f'CLEANUP' in lines[i] or 'FINALLY' in lines[i]:
                    # Check if this resource is released in the next few lines
                    for j in range(i, min(i + 10, len(lines))):
                        if f'release({var_name})' in lines[j] or f'cleanup({var_name})' in lines[j]:
                            cleanup_found = True
                            break
                    break
            
            if not cleanup_found:
                self.issues.append(DocumentIssue(
                    type='resource_management',
                    file=str(file_path),
                    line=line_num,
                    message=f'Resource {var_name} may not be properly released',
                    context=lines[line_num-1].strip(),
                    severity='warning',
                    code='missing_cleanup'
                ))
    
    def _check_error_handling(self, file_path: Path, content: str) -> None:
        """Check for proper error handling in temporal operations."""
        lines = content.split('\n')
        
        for i, line in enumerate(lines, 1):
            # Check for temporal operations without proper error handling
            if any(op in line for op in ['branch_timeline', 'merge_timelines', 'weave_pattern']):
                # Check if this is inside a TRY block
                context_start = max(0, i - 5)
                context = '\n'.join(lines[context_start:i])
                
                if 'TRY' not in context and 'CATCH' not in context:
                    self.issues.append(DocumentIssue(
                        type='error_handling',
                        file=str(file_path),
                        line=i,
                        message='Temporal operations should be wrapped in TRY/CATCH blocks',
                        context=line.strip(),
                        severity='warning',
                        code='missing_error_handling'
                    ))

        # Check for proper warning blocks for advanced topics
        if any(keyword in content for keyword in self.advanced_keywords):
            if '!!! warning' not in content and '!!! danger' not in content:
                self.issues.append(DocumentIssue(
                    type='missing_advanced_warning',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message='Advanced temporal operations should include warning blocks',
                    severity='warning'
                ))

        # Check for proper code block annotations
        for match in self.code_block_pattern.finditer(content):
            lang = match.group(1)
            if lang == 'chronoscript' and 'chronoscript' not in relative_path.lower():
                self.issues.append(DocumentIssue(
                    type='chronoscript_outside_dedicated_section',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=content[:match.start()].count('\n') + 1,
                    message='Chronoscript code should be in dedicated .chrono files',
                    severity='warning'
                ))

    def _check_dead_links(self, file_path: Path, content: str) -> None:
        """Check for dead links and Chronovyan-specific linking patterns."""
        relative_path = str(file_path.relative_to(self.docs_dir)).replace('\\', '/')

        # Check for proper cross-references in tutorials
        if 'tutorials/' in relative_path and 'README.md' not in relative_path:
            # Check for next/previous chapter links
            if 'next_chapter' not in content.lower() and 'previous_chapter' not in content.lower():
                self.issues.append(DocumentIssue(
                    type='missing_chapter_navigation',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message='Tutorial chapters should include next/previous chapter navigation',
                    severity='info'
                ))

        # Check for proper API references
        if 'api/' in relative_path:
            # Verify that all public API functions have usage examples
            if '## Examples' not in content and '## Example' not in content:
                self.issues.append(DocumentIssue(
                    type='missing_examples',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message='API documentation should include usage examples',
                    severity='warning'
                ))

        # Check for proper linking to core concepts
        core_concepts = ['CHRONON', 'AETHEL', 'WEAVE', 'PARADOX']
        for concept in core_concepts:
            if concept in content and f'../core/{concept.lower()}.md' not in content:
                self.issues.append(DocumentIssue(
                    type='missing_concept_link',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message=f'Document mentions {concept} but lacks link to core concept documentation',
                    severity='info'
                ))

        # Check for proper temporal pattern documentation
        if any(pattern in content for pattern in ['WEAVE_PATTERN', 'TEMPORAL_LOOP']):
            if '## Pattern Details' not in content:
                self.issues.append(DocumentIssue(
                    type='missing_pattern_details',
                    file=str(file_path.relative_to(self.root_dir)),
                    line=1,
                    message='Temporal patterns should include a "Pattern Details" section',
                    severity='warning'
                ))

    def generate_report(self) -> str:
        """Generate a verification report."""
        if not self.issues:
            return "# [PASS] Documentation Verification Report\n\nNo issues found in documentation!"

        report = ["# [REPORT] Documentation Verification Report\n"]
        report.append(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")

        # Count issues by type and severity
        issue_counts = {}
        severity_counts = {'error': 0, 'warning': 0, 'info': 0}

        for issue in self.issues:
            issue_type = issue.type
            issue_counts[issue_type] = issue_counts.get(issue_type, 0) + 1
            # Handle any severity value, defaulting to 'info' for unknown values
            severity = issue.severity.lower() if issue.severity.lower() in {'error', 'warning', 'info'} else 'info'
            severity_counts[severity] = severity_counts.get(severity, 0) + 1

        # Add summary
        report.append("## 📊 Summary\n")
        report.append(f"- **Total Issues**: {len(self.issues)}")
        report.append(f"- **Errors**: {severity_counts['error']} ⚠️")
        report.append(f"- **Warnings**: {severity_counts['warning']} ⚠️")
        report.append(f"- **Info**: {severity_counts['info']} ℹ️\n")

        # Group issues by file and type
        issues_by_file: Dict[str, Dict[str, List[DocumentIssue]]] = {}
        for issue in self.issues:
            if issue.file not in issues_by_file:
                issues_by_file[issue.file] = {}
            if issue.type not in issues_by_file[issue.file]:
                issues_by_file[issue.file][issue.type] = []
            issues_by_file[issue.file][issue.type].append(issue)

        # Sort files by number of issues (highest first)
        sorted_files = sorted(issues_by_file.items(), 
                           key=lambda x: sum(len(issues) for issues in x[1].values()),
                           reverse=True)

        # Generate report by file
        for file_path, issues_by_type in sorted_files:
            total_issues = sum(len(issues) for issues in issues_by_type.values())
            report.append(f"## 📄 {file_path} ({total_issues} issues)\n")

            # Sort issue types by count (highest first)
            for issue_type, issues in sorted(issues_by_type.items(), 
                                          key=lambda x: len(x[1]), 
                                          reverse=True):
                count = len(issues)
                report.append(f"### {issue_type.replace('_', ' ').title()} ({count} {'issue' if count == 1 else 'issues'})\n")

                for issue in issues[:10]:  # Show first 10 of each type
                    line_info = f"Line {issue.line}" if issue.line > 0 else ""
                    severity_emoji = "❌" if issue.severity == 'error' else "⚠️" if issue.severity == 'warning' else "ℹ️"
                    report.append(f"- {severity_emoji} **{line_info}**: {issue.message}")
                    if issue.context:
                        report.append(f"  - Context: {issue.context}")
                    if issue.code:
                        report.append(f"  - Code: `{issue.code}`")

                if len(issues) > 10:
                    report.append(f"  - ... and {len(issues) - 10} more")
                report.append("")
        # Add issue type summary
        report.append("## 📋 Issue Types\n")
        for issue_type, count in sorted(issue_counts.items(), key=lambda x: x[1], reverse=True):
            report.append(f"- `{issue_type}`: {count}")

        # Add footer
        report.append("\n---\n")
        report.append("Generated by [Chronovyan Documentation Verifier](https://github.com/Chronovyan/Chronovyan.github.io)")

        return '\n'.join(report)


    def print_issues(self) -> None:
        """Print all found issues in a readable format with proper encoding handling."""
        if not self.issues:
            print("\nNo issues found!")
            return

        # Group issues by file for better organization
        issues_by_file = {}
        for issue in self.issues:
            if issue.file not in issues_by_file:
                issues_by_file[issue.file] = []
            issues_by_file[issue.file].append(issue)


        print(f"\nFound {len(self.issues)} issues across {len(issues_by_file)} files:")

        for file, file_issues in issues_by_file.items():
            print(f"\n{file}:")
            print("=" * (len(file) + 1))

            for issue in file_issues:
                try:
                    # Handle encoding for the message and context
                    message = issue.message.encode('utf-8', errors='replace').decode('utf-8')
                    context = issue.context.encode('utf-8', errors='replace').decode('utf-8') if issue.context else ""
                    suggestion = issue.suggestion.encode('utf-8', errors='replace').decode('utf-8') if hasattr(issue, 'suggestion') and issue.suggestion else ""

                    print(f"\n[{issue.severity.upper()}] Line {issue.line}")
                    print(f"  {message}")

                    if context:
                        print("  Context:")
                        for line in context.split('\n'):
                            print(f"    {line}")

                    if suggestion:
                        print("  Suggestion:")
                        for line in suggestion.split('\n'):
                            print(f"    {line}")

                    print("-" * 80)

                except Exception as e:
                    print(f"\n[ERROR] Failed to display issue: {str(e)}")
                    print(f"  Type: {issue.type}")
                    print(f"  File: {issue.file}")
                    print(f"  Line: {issue.line}")
                    print("-" * 80)
def main():
    """Main entry point for the documentation verification tool."""
    try:
        parser = argparse.ArgumentParser(description='Verify Chronovyan documentation and code style')
        parser.add_argument('--strict', action='store_true', help='Treat warnings as errors')
        parser.add_argument('--check-style', action='store_true', help='Enable style guide checks')
        parser.add_argument('--fix', action='store_true', help='Automatically fix fixable issues')
        parser.add_argument('--format', choices=['text', 'json', 'github'], default='text', 
                         help='Output format (default: text)')
        parser.add_argument('files', nargs='*', help='Files to check (default: all .md and .chrono files)')
        
        args = parser.parse_args()
        
        verifier = DocumentationVerifier()
        
        # If no files specified, scan the docs directory and source files
        if not args.files:
            markdown_files = verifier.find_markdown_files()
            # Also find .chrono files if style checking is enabled
            if args.check_style:
                chrono_files = list(Path('.').rglob('*.chrono'))
                markdown_files.extend(chrono_files)
        else:
            markdown_files = [Path(f) for f in args.files]
        
        print(f"Found {len(markdown_files)} files to verify.")
        
        # Process each file
        total_files = len(markdown_files)
        for i, file_path in enumerate(markdown_files, 1):
            print(f"\rProcessing file {i}/{total_files}: {file_path.relative_to(verifier.root_dir) if file_path.is_absolute() else file_path}", end="")
            
            if file_path.suffix == '.md':
                verifier.verify_document(file_path)
            elif file_path.suffix == '.chrono' and args.check_style:
                content = file_path.read_text(encoding='utf-8')
                verifier._validate_code_style(file_path, content, content.split('\n'))
        
        print("\nVerification complete!\n")
        
        # Generate and print the report
        report = verifier.generate_report()
        
        # Save report to file
        report_path = verifier.root_dir / 'docs' / 'verification' / 'VERIFICATION_REPORT.md'
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(report, encoding='utf-8')
        
        # Print report with proper encoding
        try:
            print(report)
        except UnicodeEncodeError:
            # Fallback for terminals that don't support Unicode
            print(report.encode('ascii', 'replace').decode('ascii'))
        
        print(f"\nVerification report saved to: {report_path.relative_to(verifier.root_dir)}")
        
        # Print any issues found
        if verifier.issues:
            error_count = sum(1 for issue in verifier.issues if issue.severity == 'error')
            warning_count = sum(1 for issue in verifier.issues if issue.severity == 'warning')
            info_count = sum(1 for issue in verifier.issues if issue.severity == 'info')
            
            print(f"\nFound {error_count} errors, {warning_count} warnings, and {info_count} info messages.")
            
            # Return non-zero exit code if there are errors or warnings in strict mode
            if args.strict and (error_count > 0 or warning_count > 0):
                return 1
            return 0 if error_count == 0 else 1
        else:
            print("\nNo issues found!")
            return 0
            
    except Exception as e:
        print(f"\nError during verification: {str(e)}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
