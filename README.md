# OpenFHE Examples

A sandbox for OpenFHE experiments that builds OpenFHE in Debug mode next to the repo, so you can step from your code into the library's source in VS Code (lldb on macOS, gdb on Linux).

```text
openfhe-debug/
├── openfhe-development/        OpenFHE source + build (kept: debug info points here)
├── openfhe-install/            OpenFHE Debug install, used by CMakeLists.txt
└── openfhe-examples-testing/   this repo
```

## Requirements

- macOS: `xcode-select --install`, `brew install cmake`
- Linux (Ubuntu/Debian): `sudo apt install build-essential git gdb` and CMake ≥ 3.25 (`pip install "cmake>=3.25"` on Ubuntu 22.04)
- VS Code with the C/C++ extension (`ms-vscode.cpptools`)

## Setup (once per machine)

```bash
./setup_openfhe.sh [tag]   # default v1.6.1, ~10-20 min
```

Run it on every new machine; don't copy `build/` or `openfhe-install/`, because their debug info holds the source paths of the machine that built them.

## Build and run

| Program | Circuit |
|---|---|
| `ckks.cpp` | bootstrap → multiply → rotate |
| `ckks_fbt.cpp` | functional bootstrapping (experimental): RLWE → CKKS → LUT f(x) = x² mod 16 → RLWE |
| `bfv.cpp`, `bgv.cpp` | leveled add, multiply, rotate (OpenFHE has no BFV/BGV bootstrapping) |

Each `<name>.cpp` builds to `build/<name>`. To add a program, add its name to the `foreach` in `CMakeLists.txt`.

```bash
./build.sh [1]   # builds and runs all four; 1 = clean rebuild
```

## Debug in VS Code

1. Open this folder and one of the programs, set breakpoints (the `[BP]` lines are good starts).
2. With that file open, press **F5**. It builds first, then debugs it.
3. **F11** step into, **F10** step over, **Shift+F11** step out, **F5** continue.

To break inside OpenFHE, open files from `../openfhe-development/src`.

| Operation | Implementation (under `../openfhe-development/src/pke/lib`) |
|---|---|
| `EvalBootstrapSetup` / `EvalBootstrapKeyGen` | `scheme/ckksrns/ckksrns-fhe.cpp` |
| `EvalBootstrap` | `ckksrns-fhe.cpp`: ModRaise → `EvalCoeffsToSlots` → approximate mod reduction → `EvalSlotsToCoeffs` |
| `EvalFBTSetup` / `EvalFBT` | `ckksrns-fhe.cpp`; RLWE ↔ CKKS conversion in `schemelet/rlwe-mp.cpp` |
| Chebyshev series | `scheme/ckksrns/ckksrns-advancedshe.cpp` (`EvalChebyshevSeriesPS`) |
| CKKS `EvalMult`, rescale | `schemerns/rns-leveledshe.cpp`, `scheme/ckksrns/ckksrns-leveledshe.cpp` |
| BFV `EvalMult` (scale by t/Q) | `scheme/bfvrns/bfvrns-leveledshe.cpp` (`LeveledSHEBFVRNS::EvalMult`) |
| BGV `EvalMult`, modulus switching | `schemerns/rns-leveledshe.cpp`, `scheme/bgvrns/bgvrns-leveledshe.cpp` (`ModReduceInternalInPlace`) |
| Relinearize, rotations | `keyswitch/keyswitch-hybrid.cpp` |
| Encrypt / Decrypt | `schemerns/rns-pke.cpp`, `scheme/bfvrns/bfvrns-pke.cpp`, `encoding/ckkspackedencoding.cpp`, `encoding/packedencoding.cpp` |
| NTT / RNS math | `src/core/lib/math/` |
