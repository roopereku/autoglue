from argparse import ArgumentParser
import pathlib
import subprocess
import shutil
import os

# Find the Autoglue root path and CMake.
root_path = os.path.abspath(os.path.dirname(__file__))
cmake_path = shutil.which("cmake")

if len(cmake_path) == 0:
    print("Error: Failed to find CMake")
    exit(1)

class Subsystem:
    def __init__(self, desc, path):
        self.name = pathlib.PurePath(path).name
        self.desc = desc
        self.path = f"{root_path}/{path}"

    def build(self, generator=False, backend=False, debug=False):
        print(f"Building {self.desc}")

        self.build_path = f"{self.path}/build"
        pathlib.Path(self.build_path).mkdir(exist_ok=True)

        cmake_opts = [
            cmake_path,
            "-S", self.path, "-B", self.build_path,
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
        ]

        if backend:
            cmake_opts.append(f"-DAUTOGLUE_BUILD_{self.name.upper()}_BACKEND=ON")

        if generator:
            cmake_opts.append(f"-DAUTOGLUE_BUILD_{self.name.upper()}_GENERATOR=ON")

        if debug:
            cmake_opts.append(f"-DCMAKE_BUILD_TYPE=Debug")

        configure_result = subprocess.run(
            cmake_opts,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        if configure_result.returncode != 0:
            print(f"Configuring {self.desc} failed:")
            print(configure_result.stderr)
            return False

        build_result = subprocess.run([cmake_path, "--build", self.build_path])

        if build_result.returncode != 0:
            print(f"Building {self.desc} failed")
            return False

        install_result = subprocess.run(
            [cmake_path, "--install", self.build_path, "--prefix", f"{root_path}/prefix"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        if install_result.returncode != 0:
            print(f"Installing prefix for {self.desc} failed:")
            print(install_result.stderr)
            return False

        return True

core = Subsystem("Autoglue Core", "autoglue")
if not core.build([]):
    exit(1)

optional_subsystems = [
    Subsystem("Autoglue Clang", "clang"),
    Subsystem("Autoglue C#", "csharp"),
    Subsystem("Autoglue Java", "java"),
    Subsystem("Autoglue XML", "xml"),
]

def main():
    arg_parser = ArgumentParser(
        prog="Autoglue building utility"
    )

    arg_parser.add_argument(f"--debug", action="store_true")

    for entry in optional_subsystems:
        arg_parser.add_argument(f"--{entry.name}", action="store_true")
        arg_parser.add_argument(f"--{entry.name}-generator", action="store_true")
        arg_parser.add_argument(f"--{entry.name}-backend", action="store_true")

    args = vars(arg_parser.parse_args())

    for entry in optional_subsystems:
        generator = args[entry.name] or args[f"{entry.name}_generator"]
        backend = args[entry.name] or args[f"{entry.name}_backend"]

        if not generator and not backend:
            continue

        if not entry.build(generator=generator, backend=backend, debug=args["debug"]):
            exit(1)

if __name__== "__main__":
    main()
