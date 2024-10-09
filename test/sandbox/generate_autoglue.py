import sys, os
import subprocess

# Import the Autoglue build utility.
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from build import *

# Build all subsystems.
for entry in optional_subsystems:
    entry.build(generator=True, backend=True)
