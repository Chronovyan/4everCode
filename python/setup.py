from setuptools import setup

if __name__ == "__main__":
    # This is a minimal setup.py that defers to pyproject.toml
    # It's maintained for compatibility with tools that don't support PEP 621 yet
    setup(
        entry_points={
            'console_scripts': [
                'chronovyan=chronovyan.cli:main',
            ],
        }
    )
