# Windows 64-bit Calculator Project

This project is a small calculator program written in C with modular logic and automated tests.

## Structure

- `src/calculator.h` — function declarations
- `src/calculator.c` — math implementation
- `src/app.c` — CLI entry point
- `tests/test_calculator.c` — unit tests
- `Makefile` — build and test automation
- `.github/workflows/build-test.yml` — CI pipeline

## Build and test

On a system with MinGW cross-compiler installed:

```bash
make test
make app
```

This builds and runs the tests, then builds the Windows 64-bit application `build/app.exe`.

## Example run

```bash
./build/app.exe 10 + 5
```

Expected output:

```text
15
```

## Required MinGW compiler

```bash
sudo apt-get install mingw-w64
```

Then build:

```bash
make app
```

## Windows target

The MinGW compiler uses:

```bash
-m64
```

so the output is a native 64-bit Windows executable.
