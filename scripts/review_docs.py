#!/usr/bin/env python3
"""
Chronovyan Documentation Review Tool

This script analyzes the documentation for common issues and generates a report.
"""

import os
import re
import sys
import yaml
import json
import logging
from pathlib import Path
from typing import Dict, List, Tuple, Set, Optional, Any
from dataclasses import dataclass, field
from datetime import datetime
import markdown
from bs4 import BeautifulSoup
import frontmatter

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('docs_review.log')
    ]
)

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
REPORT_FILE = 'docs_review_report.md'
EXCLUDE_DIRS = {'site', 'assets', 'node_modules', '.git', '.github'}
EXCLUDE_FILES = {'.DS_Store', 'README.md', 'SUMMARY.md'}

# Regular expressions
HEADER_PATTERN = re.compile(r'^(#+)\s+(.+?)(?:\s*\{#([^}]+)\})?\s*$', re.MULTILINE)
LINK_PATTERN = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
CODE_BLOCK_PATTERN = re.compile(r'```[^`]*?```', re.DOTALL)
INLINE_CODE_PATTERN = re.compile(r'`[^`]+`')

@dataclass
class Issue:
    """Represents an issue found during documentation review."""
    file: Path
    line: Optional[int] = None
    message: str = ""
    context: str = ""
    severity: str = "warning"  # 'error', 'warning', or 'info'
    category: str = "general"  # 'frontmatter', 'links', 'style', 'content', etc.
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert the issue to a dictionary."""
        return {
            'file': str(self.file.relative_to(DOCS_DIR) if self.file.is_relative_to(DOCS_DIR) else self.file),
            'line': self.line,
            'message': self.message,
            'context': self.context,
            'severity': self.severity,
            'category': self.category
        }

@dataclass
class DocumentStats:
    """Statistics for a single document."""
    path: Path
    word_count: int = 0
    code_block_count: int = 0
    header_count: Dict[int, int] = field(default_factory=dict)  # level -> count
    link_count: int = 0
    image_count: int = 0
    issues: List[Issue] = field(default_factory=list)
    has_frontmatter: bool = False
    frontmatter_fields: Dict[str, Any] = field(default_factory=dict)

class DocumentationReviewer:
    """Main class for reviewing documentation."""
    
    def __init__(self, docs_dir: Path):
        self.docs_dir = docs_dir
        self.stats: Dict[Path, DocumentStats] = {}
        self.issues: List[Issue] = []
        self.link_map: Dict[str, List[Tuple[Path, int]]] = {}
        self.anchors: Dict[Path, Set[str]] = {}
    
    def get_markdown_files(self) -> List[Path]:
        """Get all markdown files in the documentation directory."""
        files = []
        
        for root, dirs, filenames in os.walk(self.docs_dir):
            # Skip excluded directories
            dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
            
            for filename in filenames:
                if filename.lower().endswith('.md') and filename not in EXCLUDE_FILES:
                    filepath = Path(root) / filename
                    files.append(filepath)
        
        return files
    
    def analyze_document(self, filepath: Path) -> DocumentStats:
        """Analyze a single markdown document."""
        logging.info(f"Analyzing {filepath.relative_to(self.docs_dir)}")
        
        try:
            content = filepath.read_text(encoding='utf-8')
        except UnicodeDecodeError:
            try:
                content = filepath.read_text(encoding='latin-1')
                self.issues.append(Issue(
                    file=filepath,
                    message="File is not UTF-8 encoded",
                    severity="warning",
                    category="encoding"
                ))
            except Exception as e:
                self.issues.append(Issue(
                    file=filepath,
                    message=f"Failed to read file: {str(e)}",
                    severity="error",
                    category="io"
                ))
                return None
        
        stats = DocumentStats(path=filepath)
        self.stats[filepath] = stats
        
        # Parse frontmatter
        self._parse_frontmatter(filepath, content, stats)
        
        # Remove frontmatter for further analysis
        if stats.has_frontmatter:
            content = content.split('---', 2)[2] if '---' in content else content
        
        # Basic statistics
        self._analyze_basics(filepath, content, stats)
        
        # Check for issues
        self._check_headers(filepath, content, stats)
        self._check_links(filepath, content, stats)
        self._check_code_blocks(filepath, content, stats)
        
        return stats
    
    def _parse_frontmatter(self, filepath: Path, content: str, stats: DocumentStats) -> None:
        """Parse and validate frontmatter."""
        if not content.strip().startswith('---'):
            self.issues.append(Issue(
                file=filepath,
                message="Missing frontmatter",
                severity="warning",
                category="frontmatter"
            ))
            return
        
        try:
            parts = content.split('---', 2)
            if len(parts) < 3:
                self.issues.append(Issue(
                    file=filepath,
                    message="Invalid frontmatter format",
                    severity="error",
                    category="frontmatter"
                ))
                return
            
            frontmatter_content = parts[1].strip()
            frontmatter_data = yaml.safe_load(frontmatter_content)
            
            if not isinstance(frontmatter_data, dict):
                self.issues.append(Issue(
                    file=filepath,
                    message="Frontmatter is not a YAML dictionary",
                    severity="error",
                    category="frontmatter"
                ))
                return
            
            stats.has_frontmatter = True
            stats.frontmatter_fields = frontmatter_data
            
            # Check for required fields
            required_fields = ['title', 'description']
            for field in required_fields:
                if field not in frontmatter_data or not frontmatter_data[field]:
                    self.issues.append(Issue(
                        file=filepath,
                        message=f"Missing required frontmatter field: {field}",
                        severity="error",
                        category="frontmatter"
                    ))
            
            # Check description length
            if 'description' in frontmatter_data:
                desc = frontmatter_data['description']
                if not isinstance(desc, str):
                    self.issues.append(Issue(
                        file=filepath,
                        message="Description must be a string",
                        severity="error",
                        category="frontmatter"
                    ))
                elif len(desc) > 160:  # Recommended max length for meta descriptions
                    self.issues.append(Issue(
                        file=filepath,
                        message=f"Description is too long ({len(desc)} characters, max 160)",
                        severity="warning",
                        category="frontmatter"
                    ))
            
            # Check weight is a number if present
            if 'weight' in frontmatter_data and not isinstance(frontmatter_data['weight'], (int, float)):
                self.issues.append(Issue(
                    file=filepath,
                    message="Weight must be a number",
                    severity="error",
                    category="frontmatter"
                ))
        
        except yaml.YAMLError as e:
            self.issues.append(Issue(
                file=filepath,
                message=f"Invalid YAML in frontmatter: {str(e)}",
                severity="error",
                category="frontmatter"
            ))
        except Exception as e:
            self.issues.append(Issue(
                file=filepath,
                message=f"Error parsing frontmatter: {str(e)}",
                severity="error",
                category="frontmatter"
            ))
    
    def _analyze_basics(self, filepath: Path, content: str, stats: DocumentStats) -> None:
        """Analyze basic document statistics."""
        # Word count (approximate)
        stats.word_count = len(content.split())
        
        # Code blocks
        code_blocks = list(CODE_BLOCK_PATTERN.finditer(content))
        stats.code_block_count = len(code_blocks)
        
        # Headers
        headers = list(HEADER_PATTERN.finditer(content))
        stats.header_count = {}
        
        for match in headers:
            level = len(match.group(1))  # Number of # characters
            stats.header_count[level] = stats.header_count.get(level, 0) + 1
        
        # Links and images
        links = list(LINK_PATTERN.finditer(content))
        stats.link_count = len(links)
        stats.image_count = sum(1 for link in links if link.group(2).lower().endswith(('.png', '.jpg', '.jpeg', '.gif', '.svg')))
    
    def _check_headers(self, filepath: Path, content: str, stats: DocumentStats) -> None:
        """Check for header-related issues."""
        lines = content.split('\n')
        
        for i, line in enumerate(lines, 1):
            match = HEADER_PATTERN.match(line)
            if match:
                level = len(match.group(1))
                text = match.group(2).strip()
                
                # Check header levels (only h1-h4 should be used)
                if level > 4:
                    self.issues.append(Issue(
                        file=filepath,
                        line=i,
                        message=f"Header level too deep (h{level}): use h1-h4 only",
                        context=line.strip(),
                        severity="warning",
                        category="style"
                    ))
                
                # Check header text formatting
                if text.endswith(':'):
                    self.issues.append(Issue(
                        file=filepath,
                        line=i,
                        message="Header ends with colon (avoid punctuation in headers)",
                        context=line.strip(),
                        severity="warning",
                        category="style"
                    ))
    
    def _check_links(self, filepath: Path, content: str, stats: DocumentStats) -> None:
        """Check for link-related issues."""
        # This is a simplified version - in a real implementation, we'd check if links exist
        links = list(LINK_PATTERN.finditer(content))
        
        for match in links:
            link_text = match.group(1)
            link_url = match.group(2)
            
            # Skip empty links
            if not link_url.strip():
                self.issues.append(Issue(
                    file=filepath,
                    message="Empty link URL",
                    context=f"[{link_text}]({link_url})",
                    severity="error",
                    category="links"
                ))
                continue
            
            # Skip web URLs and anchors
            if link_url.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
            
            # Check if the linked file exists
            target_path = (filepath.parent / link_url).resolve()
            
            if not target_path.exists():
                self.issues.append(Issue(
                    file=filepath,
                    message=f"Broken link: {link_url}",
                    context=f"[{link_text}]({link_url})",
                    severity="error",
                    category="links"
                ))
    
    def _check_code_blocks(self, filepath: Path, content: str, stats: DocumentStats) -> None:
        """Check for code block issues."""
        code_blocks = list(CODE_BLOCK_PATTERN.finditer(content))
        
        for i, match in enumerate(code_blocks, 1):
            code_block = match.group(0)
            lines = code_block.split('\n')
            
            # Check for language specification
            if len(lines) > 0 and '```' in lines[0]:
                lang_spec = lines[0].strip().strip('`').strip()
                if not lang_spec or lang_spec == '```':
                    self.issues.append(Issue(
                        file=filepath,
                        message=f"Code block {i} has no language specified",
                        context=lines[0].strip() if len(lines) > 0 else "",
                        severity="warning",
                        category="code"
                    ))
    
    def generate_report(self, output_file: str = None) -> str:
        """Generate a markdown report of the review findings."""
        if not output_file:
            output_file = REPORT_FILE
        
        # Count issues by severity and category
        issue_counts = {
            'error': 0,
            'warning': 0,
            'info': 0
        }
        
        category_counts = {}
        
        for issue in self.issues:
            issue_counts[issue.severity] += 1
            category_counts[issue.category] = category_counts.get(issue.category, 0) + 1
        
        # Generate report
        report = [
            "# Chronovyan Documentation Review Report",
            f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}",
            "",
            "## Summary",
            f"- Documents analyzed: {len(self.stats)}",
            f"- Total issues: {len(self.issues)}",
            f"  - Errors: {issue_counts['error']}",
            f"  - Warnings: {issue_counts['warning']}",
            f"  - Info: {issue_counts['info']}",
            "",
            "## Issues by Category"
        ]
        
        # Add category counts
        for category, count in sorted(category_counts.items()):
            report.append(f"- **{category.replace('-', ' ').title()}**: {count}")
        
        # Add detailed issues
        report.extend([
            "",
            "## Detailed Issues",
            ""
        ])
        
        # Group issues by file
        issues_by_file = {}
        for issue in self.issues:
            rel_path = str(issue.file.relative_to(self.docs_dir) if issue.file.is_relative_to(self.docs_dir) else issue.file)
            if rel_path not in issues_by_file:
                issues_by_file[rel_path] = []
            issues_by_file[rel_path].append(issue)
        
        # Add issues for each file
        for file_path, file_issues in sorted(issues_by_file.items()):
            report.append(f"### {file_path}")
            
            for issue in sorted(file_issues, key=lambda x: (x.severity, x.line or 0)):
                line_info = f" (line {issue.line})" if issue.line else ""
                context = f"\n    > {issue.context}" if issue.context else ""
                report.append(
                    f"- **{issue.severity.upper()}**{line_info}: {issue.message}  "
                    f"*[{issue.category}]*{context}"
                )
            
            report.append("")
        
        # Add document statistics
        report.extend([
            "## Document Statistics",
            "",
            "| Document | Words | Headers | Code Blocks | Links | Images |",
            "|----------|------:|--------:|------------:|------:|"
        ])
        
        for doc_path, stats in sorted(self.stats.items(), key=lambda x: str(x[0])):
            rel_path = doc_path.relative_to(self.docs_dir) if doc_path.is_relative_to(self.docs_dir) else doc_path
            headers = sum(stats.header_count.values()) if stats.header_count else 0
            report.append(
                f"| {rel_path} | {stats.word_count:,} | {headers} | {stats.code_block_count} | "
                f"{stats.link_count} | {stats.image_count} |"
            )
        
        report_text = '\n'.join(report)
        
        # Write to file
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(report_text)
        
        return report_text
    
    def run(self) -> bool:
        """Run the documentation review."""
        logging.info("Starting documentation review...")
        
        # Get all markdown files
        files = self.get_markdown_files()
        
        if not files:
            logging.error("No markdown files found in the documentation directory.")
            return False
        
        # Analyze each file
        for filepath in files:
            self.analyze_document(filepath)
        
        # Generate report
        report_path = self.generate_report()
        logging.info(f"Documentation review complete. Report saved to {report_path}")
        
        # Return success if no errors were found
        return not any(issue.severity == 'error' for issue in self.issues)

def main():
    """Main entry point."""
    reviewer = DocumentationReviewer(DOCS_DIR)
    success = reviewer.run()
    
    # Print a summary
    error_count = sum(1 for issue in reviewer.issues if issue.severity == 'error')
    warning_count = sum(1 for issue in reviewer.issues if issue.severity == 'warning')
    
    print("\nDocumentation Review Summary:")
    print(f"- Documents analyzed: {len(reviewer.stats)}")
    print(f"- Issues found: {len(reviewer.issues)}")
    print(f"  - Errors: {error_count}")
    print(f"  - Warnings: {warning_count}")
    print(f"\nReport generated: {REPORT_FILE}")
    
    if error_count > 0:
        print("\n\033[91m❌ Documentation review failed with errors.\033[0m")
        return 1
    elif warning_count > 0:
        print("\n\033[93m⚠️  Documentation review completed with warnings.\033[0m")
        return 0
    else:
        print("\n\033[92m✅ Documentation review completed successfully!\033[0m")
        return 0

if __name__ == "__main__":
    sys.exit(main())
