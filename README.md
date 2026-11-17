# GradeTrack

GradeTrack is a desktop CGPA manager built with C++17 and Qt 6. Organize courses by semester, record credit units and letter grades, and view semester GPA, cumulative GPA, total credits, and semester count.

## Features

- Add semesters and register courses with credit units and grades from A to F.
- Edit or remove an individual course attempt.
- Calculate semester GPA and cumulative GPA using a 5-point grade scale (A = 5 through F = 0), weighted by credit units.
- Save records as JSON in Qt's application data directory so they remain available when the app is reopened.

## Requirements

- CMake 3.21 or newer
- Qt 6 with the Widgets component
- A C++17 compiler
- Ninja (for the commands below)

On Windows, the project can be built from the MSYS2 UCRT64 environment with Qt 6, CMake, and Ninja installed.

## Build

From the project directory, configure and build:

```sh
cmake -S . -B build -G Ninja
cmake --build build
```

Run the application:

```sh
./build/GradeTrack
```

On Windows/MSYS2, run `./build/GradeTrack.exe`.

## Tests

Build the test target as part of the project, then run:

```sh
ctest --test-dir build --output-on-failure
```

The current test covers editing and removing a course attempt and rejection of an invalid course index.

## Project layout

```text
src/
  calculations/   CGPA calculation and JSON serialization
  models/         Course and semester models
  ui/             Qt desktop interface
tests/            CTest test executable
```
