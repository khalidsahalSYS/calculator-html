# Windows Desktop Calculator (C + HTML5)

A small 64-bit Windows calculator. The responsive HTML/CSS/JavaScript interface runs in a standalone Microsoft Edge app window when Edge is installed. A native C executable serves the page and performs every arithmetic operation through a loopback-only HTTP API; it does not evaluate calculations in JavaScript. If Edge is unavailable, the app opens the local page in the default browser.

## Build (64-bit Windows)

### MSVC

1. Install Visual Studio Build Tools with **Desktop development with C++** (including the Windows 10/11 SDK).
2. Open **x64 Native Tools Command Prompt for VS** in the project directory.
3. Run:

```bat
build.bat
```

This produces `build\calculator.exe` and copies `index.html` beside it. To compile manually:

```bat
cl /nologo /O2 /W4 /TC /D_CRT_SECURE_NO_WARNINGS src\server.c src\calculator.c /Febuild\calculator.exe /link /SUBSYSTEM:WINDOWS ws2_32.lib shell32.lib
copy index.html build\index.html
```

### MinGW-w64 GCC

From an x64 MinGW-w64 shell, run:

```sh
mkdir -p build
x86_64-w64-mingw32-gcc -std=c11 -O2 -Wall -Wextra -D_CRT_SECURE_NO_WARNINGS -mwindows src/server.c src/calculator.c -o build/calculator.exe -lws2_32 -lshell32
cp index.html build/index.html
```

## Run

Launch `build\calculator.exe`. It serves the UI and calculation endpoint at `127.0.0.1:8765`; keep `index.html` alongside the executable. Close the calculator server from Task Manager after closing the browser window. The app needs no WebView SDK or third-party library. If port 8765 is already occupied, close the other application and retry.

## Design

- `src/calculator.c` contains the four arithmetic operations, validates finite values, and returns a specific division-by-zero status.
- `src/server.c` is a Windows loopback HTTP server. `POST /calculate` accepts JSON such as `{"a":12,"b":3,"op":"/"}` and returns a result or an error. It binds only to `127.0.0.1`.
- `index.html` is the responsive calculator UI. Button and keyboard input are supported.
