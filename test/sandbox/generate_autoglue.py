import sys, os
import subprocess

# Import the Autoglue build utility.
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from build import *

# Build all subsystems.
for entry in optional_subsystems:
    entry.build(generator=True, backend=True)

# Build the sandbox.
subprocess.run([
    cmake_path,
    "-S", ".", "-B", "build",
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
])

subprocess.run([cmake_path, "--build", "build"])

# Generate Autoglue bindings.
all_subsystems = [core] + optional_subsystems
bin_path = os.path.abspath("build/autogluetest")

for entry in all_subsystems:
    output_path = f"output/{entry.name}/"
    compile_commands_path = f"{entry.build_path}/compile_commands.json"

    print(f"Generate bindings for {entry.desc} to {output_path}")
    os.makedirs(output_path, exist_ok=True)

    cwd = os.getcwd()
    os.chdir(output_path)
    subprocess.run([bin_path, compile_commands_path])
    os.chdir(cwd)
