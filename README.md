# PicMover_server

## Build Instructions

To compile and build the project,
run the following commands:

```bash
mkdir build && cd build
cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . --config Release   # or Debug
```

This will generate build files using CMake and build the project using Ninja.
You can switch between `Release` and `Debug` configurations as needed.

## Code Formatting

The project uses **clang-format** for consistent code style.
A `.clang-format` file is already provided.
To format the source files, run:

```bash
clang-format -i ./src/*.cxx -i ./src/*.hxx
```

This will automatically format all C++ source (`.cxx`) and header (`.hxx`) files under the `src` directory.

## Code Checking

The project uses **clang-tidy** for static code analysis.
Before running clang-tidy,
make sure to generate the `compile_commands.json` file by configuring the project with CMake:

```bash
mkdir build && cd build
cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

Then run clang-tidy:

```bash
clang-tidy -p build -header-filter='src/.*' src/*.cxx src/*.hxx
```

This will analyze all source and header files in the `src` folder and report potential issues based on the configured checks.
