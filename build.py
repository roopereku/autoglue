from argparse import ArgumentParser
from pathlib import Path
import subprocess
import shutil
import os

optional_subsystems = {
    "csharp" : "Autoglue C#",
    "java" : "Autoglue Java"
}

arg_parser = ArgumentParser(
    prog="Autoglue building utility"
)

arg_parser.add_argument(f"--all", action="store_true")

for key in optional_subsystems:
    arg_parser.add_argument(f"--{key}", action="store_true")
    arg_parser.add_argument(f"--{key}-generator", action="store_true")
    arg_parser.add_argument(f"--{key}-backend", action="store_true")

args = vars(arg_parser.parse_args())
cmake_path = shutil.which("cmake")

def build_subsystem(cmake_path, subsystem_path, description, config_opts):
    build_path = f"{subsystem_path}/build"
    Path(build_path).mkdir(exist_ok=True)

    configure_result = subprocess.run(
        [cmake_path, "-S", subsystem_path, "-B", build_path] + config_opts,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    if configure_result.returncode == 1:
        print(f"Configuring {description} failed:")
        print(configure_result.stderr)
        exit(1)

    build_result = subprocess.run(
        [cmake_path, "--build", build_path],
        stderr=subprocess.PIPE,
        text=True
    )

    if build_result.returncode == 1:
        print(f"Building {description} failed:")
        print(build_result.stderr)
        exit(1)

    install_result = subprocess.run(
        [cmake_path, "--install", build_path, "--prefix", f"{build_path}/prefix"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True
    )

    if install_result.returncode == 1:
        print(f"Installing prefix for {description} failed:")
        print(install_result.stderr)
        exit(1)

if len(cmake_path) == 0:
    print("Error: Failed to find CMake")
    exit(1)

build_subsystem(cmake_path, "autoglue", "Autoglue Core", [])

for key in optional_subsystems:
    config_opts = []

    if args[key] or args[f"{key}_backend"]:
        config_opts.append(f"-DAUTOGLUE_BUILD_{key.upper()}_BACKEND=ON")

    if args[key] or args[f"{key}_generator"]:
        config_opts.append(f"-DAUTOGLUE_BUILD_{key.upper()}_GENERATOR=ON")

    build_subsystem(cmake_path, key, optional_subsystems[key], config_opts)
