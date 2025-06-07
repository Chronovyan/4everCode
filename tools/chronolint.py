#!/usr/bin/env python3
"""
Chronovyan Linter

A static analysis tool that enforces the Chronovyan Style Guide.
Checks for common issues, style violations, and potential bugs.
"""

import re
import sys
from pathlib import Path
from typing import List, Dict, Optional, Pattern, Tuple
import ast
import tokenize
from dataclasses import dataclass

# Severity levels
ERROR = "ERROR"
WARNING = "WARNING"
INFO = "INFO"

@dataclass
class LintViolation:
    """Represents a single linting violation."""
    file_path: Path
    line: int
    column: int
    code: str
    message: str
    severity: str
    context: str = ""

class ChronoLinter:
    """Main linter class that implements all the checks."""
    
    def __init__(self):
        self.violations: List[LintViolation] = []
        self.config = {
            'max_line_length': 120,
            'indent_size': 4,
            'require_type_hints': True,
            'forbid_implicit_any': True,
            'enforce_naming_conventions': True,
        }
    
    def check_file(self, file_path: Path) -> None:
        """Run all checks on a single file."""
        if not file_path.exists():
            self._add_violation(
                file_path, 0, 0, "file-not-found", f"File not found: {file_path}", ERROR
            )
            return
        
        try:
            content = file_path.read_text(encoding='utf-8')
            lines = content.splitlines()
            
            # Run checks that work on the raw text
            self._check_line_lengths(file_path, lines)
            self._check_trailing_whitespace(file_path, lines)
            self._check_indentation(file_path, lines)
            
            # Parse the file for more complex checks
            try:
                tree = ast.parse(content, str(file_path))
                self._check_ast(file_path, tree, lines)
            except SyntaxError as e:
                self._add_violation(
                    file_path, e.lineno or 1, e.offset or 1,
                    f"syntax-error", f"Syntax error: {e.msg}", ERROR
                )
                
            # Check for common temporal patterns
            self._check_temporal_patterns(file_path, content, lines)
            
        except Exception as e:
            self._add_violation(
                file_path, 0, 0, "linter-error", 
                f"Error processing file: {str(e)}", ERROR
            )
    
    def _check_line_lengths(self, file_path: Path, lines: List[str]) -> None:
        """Check that lines don't exceed the maximum length."""
        max_length = self.config['max_line_length']
        for i, line in enumerate(lines, 1):
            if len(line.rstrip('\r\n')) > max_length:
                self._add_violation(
                    file_path, i, max_length + 1, "line-too-long",
                    f"Line exceeds {max_length} characters",
                    WARNING, line
                )
    
    def _check_trailing_whitespace(self, file_path: Path, lines: List[str]) -> None:
        """Check for trailing whitespace at the end of lines."""
        for i, line in enumerate(lines, 1):
            if line.rstrip('\r\n') != line.rstrip():
                self._add_violation(
                    file_path, i, len(line.rstrip()) + 1, "trailing-whitespace",
                    "Trailing whitespace", INFO, line
                )
    
    def _check_indentation(self, file_path: Path, lines: List[str]) -> None:
        """Check for consistent indentation."""
        indent_size = self.config['indent_size']
        for i, line in enumerate(lines, 1):
            if not line.strip() or line.isspace():
                continue
                
            leading_spaces = len(line) - len(line.lstrip())
            if leading_spaces % indent_size != 0:
                self._add_violation(
                    file_path, i, 1, "bad-indent",
                    f"Indentation is not a multiple of {indent_size} spaces",
                    WARNING, line
                )
    
    def _check_ast(self, file_path: Path, tree: ast.AST, lines: List[str]) -> None:
        """Run checks that require AST parsing."""
        for node in ast.walk(tree):
            # Check for missing type hints in function definitions
            if isinstance(node, ast.FunctionDef):
                self._check_function_type_hints(file_path, node, lines)
    
    def _check_function_type_hints(self, file_path: Path, node: ast.FunctionDef, lines: List[str]) -> None:
        """Check that functions have proper type hints."""
        if self.config['require_type_hints']:
            # Check return type
            if not node.returns and not node.name.startswith('_'):
                self._add_violation(
                    file_path, node.lineno, node.col_offset + 4, "missing-return-type",
                    f"Function '{node.name}' is missing a return type annotation",
                    WARNING, lines[node.lineno - 1]
                )
            
            # Check parameter types
            for arg in node.args.args:
                if arg.arg == 'self':
                    continue
                if not arg.annotation and not node.name.startswith('_'):
                    self._add_violation(
                        file_path, node.lineno, node.col_offset + 4, "missing-param-type",
                        f"Parameter '{arg.arg}' in function '{node.name}' is missing a type annotation",
                        WARNING, lines[node.lineno - 1]
                    )
    
    def _check_temporal_patterns(self, file_path: Path, content: str, lines: List[str]) -> None:
        """Check for common temporal programming patterns and anti-patterns."""
        # Check for missing PARADOX_CHECK in temporal operations
        temporal_keywords = [
            'branch_timeline', 'merge_timelines', 'weave_pattern',
            'temporal_jump', 'paradox_check', 'stabilize'
        ]
        
        for i, line in enumerate(lines, 1):
            if any(keyword in line for keyword in temporal_keywords):
                # Check if this line is inside a PARADOX_CHECK block
                context = self._get_context(lines, i)
                if 'PARADOX_CHECK' not in context and 'paradox_check' not in context:
                    self._add_violation(
                        file_path, i, 1, "missing-paradox-check",
                        "Temporal operation should be wrapped in PARADOX_CHECK",
                        WARNING, line
                    )
    
    def _get_context(self, lines: List[str], line_num: int, context_lines: int = 5) -> str:
        """Get context around a line number."""
        start = max(0, line_num - context_lines - 1)
        end = min(len(lines), line_num + context_lines)
        return '\n'.join(lines[start:end])
    
    def _add_violation(
        self, 
        file_path: Path, 
        line: int, 
        column: int, 
        code: str, 
        message: str, 
        severity: str,
        context: str = ""
    ) -> None:
        """Add a violation to the list."""
        self.violations.append(
            LintViolation(
                file_path=file_path,
                line=line,
                column=column,
                code=code,
                message=message,
                severity=severity,
                context=context
            )
        )

def main() -> int:
    """Main entry point for the linter."""
    if len(sys.argv) < 2:
        print("Usage: chronolint.py <file1> [file2 ...]")
        return 1
    
    linter = ChronoLinter()
    
    # Process each file
    for file_pattern in sys.argv[1:]:
        for file_path in Path('.').glob(file_pattern):
            if file_path.is_file() and file_path.suffix == '.chrono':
                linter.check_file(file_path)
    
    # Print all violations
    for violation in linter.violations:
        print(
            f"{violation.file_path}:{violation.line}:{violation.column}: "
            f"{violation.severity} {violation.code}: {violation.message}"
        )
        if violation.context:
            print(f"  {violation.context.strip()}")
    
    # Return non-zero exit code if there are errors
    return 1 if any(v.severity == ERROR for v in linter.violations) else 0

if __name__ == "__main__":
    sys.exit(main())
