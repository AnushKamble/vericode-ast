import sys
from pathlib import Path

# Add python package to sys.path
root_dir = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(root_dir / "python"))
