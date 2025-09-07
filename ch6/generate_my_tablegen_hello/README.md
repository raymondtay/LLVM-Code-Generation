# TableGen Hello (CMake + llvm-tblgen)

This is a **minimal, runnable** project that shows how to:
1) Write a small `.td` file,
2) Run `llvm-tblgen` during the build to generate JSON data,
3) Read that generated data from a C++ program.

**Note:** If you modify the `.td.` file, then you need to `make clean` first and re-build.

> This pattern is useful when you want to **drive C++ code using declarative `.td` data** without writing a custom TableGen backend.

---

## Prerequisites

- **LLVM** with `llvm-tblgen` available in `PATH` (tested with LLVM 17–20).  
  On macOS (Homebrew):
  ```bash
  brew install llvm@20
  echo 'export PATH="/opt/homebrew/opt/llvm@20/bin:$PATH"' >> ~/.zshrc
  source ~/.zshrc
  which llvm-tblgen
  ```

- **CMake** (>= 3.16) and a C++17 compiler (Clang/GCC/MSVC).

---

## Build & Run

```bash
mkdir -p build && cd build
cmake ..
cmake --build . -j
./tablegen-hello
```

You should see something like:
```
Loaded 2 record(s) from: <build>/generated/hello.json
- Alice (Age=30)
- Bob   (Age=40)
```

---

## Project Layout

```
tablegen-hello/
├─ CMakeLists.txt
├─ README.md
├─ tablegen/
│  └─ Hello.td
└─ src/
   └─ main.cpp
```

- `Hello.td` holds a tiny TableGen schema and two records.
- The CMake build runs `llvm-tblgen` to produce `build/generated/hello.json`.
- The C++ program reads that JSON and prints out the records.

---

## Notes

- If you'd prefer embedding generated data into a C++ header at **compile time** rather than reading at runtime, a simple pattern is to add a CMake step that converts the JSON to a header (e.g., using `xxd -i` when available) and then `#include` it. For large projects you can also write a **custom TableGen backend** to emit C++ directly, but this example keeps things minimal.

- This example intentionally uses **no third-party JSON library**; it does a minimal parse with regex tailored to the TableGen `-dump-json` output for the records we generate. In production, prefer a proper JSON library (e.g., `nlohmann/json`).

Enjoy!
