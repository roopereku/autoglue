import sys, os
import subprocess

# Import the Autoglue build utility.
sys.path.append(os.path.join(os.path.dirname(__file__), "../.."))
from build import *

# Build all subsystems.
for entry in optional_subsystems:
    if not entry.build(generator=True, backend=True, debug=True):
        print("Build failed")
        exit(1)

# Configure the sandbox.
subprocess.run([
    cmake_path,
    "-S", ".", "-B", "build",
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
])

# Build the sandbox.
build_result = subprocess.run([cmake_path, "--build", "build"])
if build_result.returncode != 0:
    exit(1)

# Generate Autoglue bindings.
all_subsystems = [core] + optional_subsystems
bin_path = os.path.abspath("build/autogluetest")

for entry in all_subsystems:
    output_path = f"output/{entry.name}/"
    compile_commands_path = f"{entry.build_path}/compile_commands.json"
    xml_path = os.path.abspath(f"{output_path}/hierarchy.xml")

    print(f"Generate bindings for {entry.desc} to {output_path}")
    os.makedirs(output_path, exist_ok=True)

    cwd = os.getcwd()
    os.chdir(output_path)
    subprocess.run([bin_path, compile_commands_path, xml_path])
    os.chdir(cwd)
